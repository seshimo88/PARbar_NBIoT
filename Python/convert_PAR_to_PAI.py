# Written by Kai Seshimo (Semester 1, 2022) 

import os 
from astropy.coordinates import get_sun, AltAz
from astropy.time import Time
import astropy.coordinates as coord
from astropy.time import Time
import astropy.units as u # pip install astropy
import csv
import datetime
import math
from record_translator import check_file_closed, get_PAR_lists_from_PAR_str_lists

FILE_NAME_PAI        = "PAIdata.csv"
FILE_NAME_TR         = "Transmittance.csv"

CHI  = 0.96 #leaf angle distribution (chi)
ABS  = 0.9  #leaf absorbtivity (abs)
BIGA = 0.283 + 0.785 * ABS - 0.159 * (ABS ** 2) #bigA

def add_headers_PAI(writer, num_devices, num_ports, dev_num_sq110, port_num_sq110):
    if (os.path.isfile(FILE_NAME_PAI)):
        with open(FILE_NAME_PAI, "r") as file:
            if (len(file.readlines())):
                return 
    header = ['DATE', 'TIME']
    for x in range(0, num_devices):
        for y in range(0, num_ports):
            if (x + 1 == dev_num_sq110 and y == port_num_sq110):
                header.append('SQ110')
            else:
                header.append('Device' + str(x + 1) + ':PAI' + str(y))
    writer.writerow(header)

def add_headers_TR(writer, num_devices, num_ports, dev_num_sq110, port_num_sq110):
    if (os.path.isfile(FILE_NAME_TR)):
        with open(FILE_NAME_TR, "r") as file:
            if (len(file.readlines())):
                return 
    header = ['DATE', 'TIME']
    for x in range(0, num_devices):
        for y in range(0, num_ports):
            if (x + 1 == dev_num_sq110 and y == port_num_sq110):
                header.append('SQ110')
            else:
                header.append('Device' + str(x + 1) + ':Tr' + str(y))
    writer.writerow(header)

def get_UTC_date_and_time(date, time, time_difference):
    date_and_time = datetime.datetime.combine(date, time)
    UTC_hour = date_and_time.hour - time_difference
    if (UTC_hour < 0):
        UTC_hour += 24
    date_and_time = date_and_time.replace(hour = UTC_hour)
    return date_and_time

def get_zenith_angle_radian(date, time, loc, time_difference):
    UTC_time = Time(get_UTC_date_and_time(date, time, time_difference).strftime('%Y-%m-%d %H:%M')) 
    altaz = AltAz(obstime = UTC_time, location = loc)
    zen_ang = get_sun(UTC_time).transform_to(altaz).zen
    zen_ang_degree = zen_ang.to_string(decimal = True)
    zen_ang_radian = math.pi * float(zen_ang_degree) / 180
    return zen_ang_radian

def convert_PAR_to_PAI(PAR_lists, file_name_PAR, num_devices, num_ports, dev_num_sq110, port_num_sq110, \
                        time_difference, longitude_degree, latitude_degree, overwrite):
    dirname, filename = os.path.split(os.path.abspath(__file__))
    os.chdir(dirname)
    if not os.path.isdir('data'):
        os.mkdir('data')
    os.chdir('data')
    file_names = [file_name_PAR, FILE_NAME_PAI, FILE_NAME_TR]
    check_file_closed(file_names)
    
    print("Start Converting " + file_name_PAR + " to " + FILE_NAME_PAI + "...")

    # PAR_str_lists = []
    # try:
    #     with open(file_name_PAR, newline='') as file:
    #         reader = csv.reader(file)
    #         PAR_str_lists = list(reader)
    # except Exception as error: 
    #         print("Couldn't open " + file_name_PAR)
    #         print(error)
    #         return 
    # # print(PAR_str_lists[1:4])
    # PAR_lists = get_PAR_lists_from_PAR_str_lists(PAR_str_lists[1:])

    # print(PAR_lists[0:4])
    date_and_time_lists = [] #2D matrix
    Transmittance_lists = [] #2D matrix
    PAI_lists = [] #2D matrix 

    Zenith_angle_list = [] 
    loc = coord.EarthLocation(lon = longitude_degree * u.deg, lat = latitude_degree * u.deg)
    
    for x in range(0, len(PAR_lists)):
        date_and_time_list = []
        date = PAR_lists[x][0]
        date_and_time_list.append(date) 
        time = PAR_lists[x][1]
        date_and_time_list.append(time) 
        date_and_time_lists.append(date_and_time_list)
        
        PAR_above = PAR_lists[x][(dev_num_sq110 - 1) * num_ports + port_num_sq110 + 2]

        zenith_angle_radian = get_zenith_angle_radian(date, time, loc, time_difference)
       
        PAR_above_max = 2550 * math.cos(zenith_angle_radian)
        Zenith_angle_list.append(zenith_angle_radian)
        ratio = PAR_above / PAR_above_max

        Transmittance_list = [] 
        for y in range(2, len(PAR_lists[x])):
            if (PAR_above):
                Transmittance_list.append(PAR_lists[x][y] / PAR_above) #PAR above exists
            else:
                Transmittance_list.append(0) #PAR above doesn't exist
        Transmittance_lists.append(Transmittance_list)

        fb = 1.395 + ratio * (-14.43 + ratio * (48.57 + ratio * (-59.024 + 24.835 * ratio)))

        K = ((CHI ** 2 + math.tan(zenith_angle_radian) ** 2) ** 0.5) / (CHI + 1.744 * (CHI + 1.182) ** -0.733)

        PAI_list = []
        for y in range(0, len(Transmittance_list)):
            if (Transmittance_list[y]):
                PAI = (((1 - 1 / (2 * K))) * fb - 1) * math.log(Transmittance_list[y]) / (BIGA * (1 - 0.47 * fb))
            else:
                PAI = 0
            PAI_list.append(PAI)
        PAI_lists.append(PAI_list)

    if (overwrite):
        file_type = 'w'
    else:
        file_type = 'a'

    try: 
        with open(FILE_NAME_PAI, file_type, encoding='UTF8', newline='') as file:
            writer = csv.writer(file)
            add_headers_PAI(writer, num_devices, num_ports, dev_num_sq110, port_num_sq110) 
            for x in range(0, len(date_and_time_lists)):
                row = date_and_time_lists[x] + PAI_lists[x]
                writer.writerow(row)
    except Exception as error: 
        print("Couldn't open " + file_name_PAR)
        print(error)
        return 

    try: 
        with open(FILE_NAME_TR, file_type, encoding='UTF8', newline='') as file:
            writer = csv.writer(file)
            add_headers_TR(writer, num_devices, num_ports, dev_num_sq110, port_num_sq110) 
            for x in range(0, len(date_and_time_lists)):
                row = date_and_time_lists[x] + Transmittance_lists[x] 
                row.append(Zenith_angle_list[x])
                writer.writerow(row)
    except Exception as error: 
        print("Couldn't open " + FILE_NAME_TR)
        print(error)
        return 

    print("Done!" + " " + FILE_NAME_PAI + " and " + FILE_NAME_TR + " can be found under 'data' folder.") 
