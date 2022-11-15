# Written by Kai Seshimo (Semester 1, 2022) 

import os 
import datetime
from calibration import get_calibrated_PAR

# str_list format: [year, month, day, hour, minute, second, id, A0, A1,.., A5, battery, signal]
# where all data in string 
#   -> A0,A1,..,A5 is the analog read values of Arduino MKR NB 1500
#   -> signal is included in Azure records, but not in SD records
COLUMN_YEAR_RAW    = 0
COLUMN_MONTH_RAW   = 1
COLUMN_DAY_RAW     = 2
COLUMN_HOUR_RAW    = 3
COLUMN_MINUTE_RAW  = 4
COLUMN_SECOND_RAW  = 5
COLUMN_ID_RAW      = 6
COLUMN_A0_RAW      = 7
COLUMN_A1_RAW      = 8
COLUMN_A2_RAW      = 9
COLUMN_A3_RAW      = 10
COLUMN_A4_RAW      = 11
COLUMN_A5_RAW      = 12
COLUMN_BATTERY_RAW = 13
COLUMN_SIGNAL_RAW  = 14

# dev_list format: [date, time, id, A0, A1, A2, A3, A4, A5] 
# where date and time in datetime format and others as float
COLUMN_DATE_DEV = 0
COLUMN_TIME_DEV = 1
COLUMN_ID_DEV   = 2

# PAR_list format: [date, time, DEV1PAR1, DEV1PAR2,.., DEV1PAR6, DEV2PAR1,.., DEV2PAR5, DEV3PAR1,..,DEV6PAR5] 
# where date and time in datetime format and others as float
COLUMN_DATE_PAR = 0
COLUMN_TIME_PAR = 1

def check_file_closed(file_names):
    while True:
        num_closed = 0
        for file_name in file_names:
            if (os.path.isfile(file_name)):
                try:
                    file = open(file_name, "r+") 
                    file.close() 
                except IOError:
                    input(file_name + " is still open. Please close it first and hit Enter key") 
                else:
                    num_closed += 1
            else:
                num_closed += 1
        if (num_closed == len(file_names)):
            break

#str_csv_list -> dev_list
def get_dev_list(str_list):
    dev_list = []
    dev_list.append(datetime.date(int(str_list[COLUMN_YEAR_RAW]) + 2000, int(str_list[COLUMN_MONTH_RAW]), int(str_list[COLUMN_DAY_RAW])))
    dev_list.append(datetime.time(int(str_list[COLUMN_HOUR_RAW]), int(str_list[COLUMN_MINUTE_RAW]), 0))
    if (float(str_list[COLUMN_ID_RAW]) != 0):
        dev_list.append(int(str_list[COLUMN_ID_RAW]))
    else:
        dev_list.append(0)
    for idx in range(COLUMN_ID_RAW + 1, len(str_list)):
        dev_list.append(float(str_list[idx]))
    return dev_list

#Dev1_lists + Dev2_lists + ... + Dev6_lists -> PAR_lists
def get_PAR_lists(combined_dev_lists, num_devices, num_ports, period): 
    PAR_lists = []
    last_minutes = -10
    idx = -1
    for dev_list in combined_dev_lists:
        current_minutes = dev_list[COLUMN_TIME_DEV].minute
        diff_minutes = current_minutes - last_minutes
        if (current_minutes < last_minutes):
            diff_minutes += 60 

        if (diff_minutes >= period): 
            PAR_lists.append([0] * (num_devices * num_ports + 2)) 
            idx += 1
            PAR_lists[idx][COLUMN_DATE_PAR] = dev_list[COLUMN_DATE_DEV]
            PAR_lists[idx][COLUMN_TIME_PAR] = dev_list[COLUMN_TIME_DEV]
            last_minutes = current_minutes
            
        device_id = dev_list[COLUMN_ID_DEV]
        for x in range(0, num_ports):
            PAR_lists[idx][num_ports * (device_id - 1) + 2 + x] = get_calibrated_PAR(dev_list[x + 3], device_id, x)
    return PAR_lists

#PAR_str_lists + SD_str_lists -> PAR_dict = {[datetime]: PAR_list}
def get_PAR_dict_from_PAR_str_lists(PAR_str_lists, combined_sd_raw_lists, num_devices, num_ports):
    PAR_dict = {}
    for PAR_str_list in PAR_str_lists:
        try:
            date = datetime.datetime.strptime(PAR_str_list[COLUMN_DATE_PAR], "%m/%d/%Y").date()
        except Exception as _: 
            date = datetime.datetime.strptime(PAR_str_list[COLUMN_DATE_PAR], "%Y-%m-%d").date()
        time = datetime.datetime.strptime(PAR_str_list[COLUMN_TIME_PAR], "%H:%M:%S").time()
        key = datetime.datetime.combine(date, time)
        key = key.replace(second=0)
        if not (key in PAR_dict):
            value = []
            value.append(date)
            value.append(time)
            for idx in range(2, len(PAR_str_list)):
                value.append(float(PAR_str_list[idx]))
            PAR_dict[key] = value
        else:
            value_copy = PAR_dict[key].copy()
            for idx in range(2, len(PAR_str_list)):
                if not (value_copy[idx]):
                    value_copy[idx] = float(PAR_str_list[idx])
            PAR_dict[key] = value_copy
    
    for sd_raw_list in combined_sd_raw_lists:
        dev_list = get_dev_list(sd_raw_list)
        device_id = dev_list[COLUMN_ID_DEV]
        key = datetime.datetime.combine(dev_list[COLUMN_DATE_DEV], dev_list[COLUMN_TIME_DEV])
        key = key.replace(second=0)
        if not (key in PAR_dict):
            value = [0] * (num_devices * num_ports + 2)
            value[COLUMN_DATE_PAR] = dev_list[COLUMN_DATE_DEV] 
            value[COLUMN_TIME_PAR] = dev_list[COLUMN_TIME_DEV] 
            for x in range(0, num_ports):
                value[num_ports * (device_id - 1) + 2 + x] = get_calibrated_PAR(dev_list[x + 3], device_id, x)
            PAR_dict[key] = value
        else:
            value_copy = PAR_dict[key].copy()
            for x in range(0, num_ports):
                value_copy[num_ports * (device_id - 1) + 2 + x] = get_calibrated_PAR(dev_list[x + 3], device_id, x)
            PAR_dict[key] = value_copy

    return PAR_dict

def get_PAR_lists_from_PAR_str_lists(PAR_str_lists):
    PAR_lists = []
    for PAR_str_list in PAR_str_lists:
        try:
            date = datetime.datetime.strptime(PAR_str_list[COLUMN_DATE_PAR], "%m/%d/%Y").date()
        except Exception as _: 
            date = datetime.datetime.strptime(PAR_str_list[COLUMN_DATE_PAR], "%Y-%m-%d").date()
        time = datetime.datetime.strptime(PAR_str_list[COLUMN_TIME_PAR], "%H:%M:%S").time()
        PAR_list = []
        PAR_list.append(date)
        PAR_list.append(time)
        for idx in range(2, len(PAR_str_list)):
            PAR_list.append(float(PAR_str_list[idx]))
        PAR_lists.append(PAR_list)
    return PAR_lists

#PAR_dict -> PAR_lists
def get_PAR_lists_from_PAR_dict(PAR_dict, num_devices, num_ports, period):
    PAR_lists = []
    last_minutes = -10
    idx = -1 
    for key in sorted(PAR_dict):
        current_minutes = key.minute
        diff_minutes = current_minutes - last_minutes
        if (current_minutes < last_minutes):
            diff_minutes += 60 

        if (diff_minutes >= period): 
            PAR_lists.append([0] * (num_devices * num_ports + 2)) 
            idx += 1
            PAR_lists[idx][COLUMN_DATE_PAR] = PAR_dict[key][COLUMN_DATE_DEV] 
            PAR_lists[idx][COLUMN_TIME_PAR] = PAR_dict[key][COLUMN_TIME_DEV] 
            last_minutes = current_minutes
        
        for x in range(0, num_devices * num_ports):
            value = PAR_dict[key][x + 2]
            if (value):
                PAR_lists[idx][x + 2] = value
    return PAR_lists
