# Written by Kai Seshimo (Semester 1, 2022) 

from heapq import merge
import os
import csv 
from record_translator import check_file_closed, get_PAR_dict_from_PAR_str_lists, get_PAR_lists_from_PAR_dict

FILE_NAME_SD_DEV1  = "sd_dev1.txt"
FILE_NAME_SD_DEV2  = "sd_dev2.txt"
FILE_NAME_SD_DEV3  = "sd_dev3.txt"
FILE_NAME_SD_DEV4  = "sd_dev4.txt"
FILE_NAME_SD_DEV5  = "sd_dev5.txt"
FILE_NAME_SD_DEV6  = "sd_dev6.txt"
FILE_NAMES_SD = [FILE_NAME_SD_DEV1, FILE_NAME_SD_DEV2, FILE_NAME_SD_DEV3, FILE_NAME_SD_DEV4, \
            FILE_NAME_SD_DEV5, FILE_NAME_SD_DEV6] 

def add_headers(writer, num_devices, num_ports):
    header = ['DATE', 'TIME']
    for x in range(0, num_devices):
        for y in range(0, num_ports):
            header.append('Device' + str(x + 1) + ':PAR' + str(y + 1))
    writer.writerow(header)

def merge_sd_records(file_name_PAR, num_devices, num_ports, period):
    dirname, filename = os.path.split(os.path.abspath(__file__))
    os.chdir(dirname)
    if not os.path.isdir('data'):
        os.mkdir('data')
    os.chdir('data')
    # cwd = os.getcwd()
    # print("Current working directory: {0}".format(cwd))
    file_names = FILE_NAMES_SD.copy()
    file_names.append(file_name_PAR)
    check_file_closed(file_names)
    
    print("Start merging records from SD to " + file_name_PAR + "...")
    PAR_str_lists = []
    try:
        with open(file_name_PAR, newline='') as file:
            reader = csv.reader(file)
            PAR_str_lists = list(reader)
    except Exception as error: 
            print("Couldn't open " + file_name_PAR)
            print(error)
            return
    
    combined_sd_raw_lists = []
    for file_name in FILE_NAMES_SD:
        if (os.path.isfile(file_name)):
            try:
                with open(file_name, newline='') as file:
                    print("-> " + file_name + " will be merged to " + file_name_PAR + " shortly")
                    reader = csv.reader(file)
                    sd_raw_lists = list(reader)
                    for sd_raw_list in sd_raw_lists:
                        if (len(sd_raw_list)):
                            combined_sd_raw_lists.append(sd_raw_list)
            except Exception as error: 
                    print("Couldn't open " + file_name)
                    print(error)
                    return 
        else:
            print("-> " + file_name + " does not exist under 'data' folder")
    
    PAR_dict = get_PAR_dict_from_PAR_str_lists(PAR_str_lists[1:], combined_sd_raw_lists, num_devices, num_ports)
    PAR_lists = get_PAR_lists_from_PAR_dict(PAR_dict, num_devices, num_ports, period) 
    try: 
        with open(file_name_PAR, 'w', encoding='UTF8', newline='') as file:
            writer = csv.writer(file)
            add_headers(writer, num_devices, num_ports) 
            for row in PAR_lists:
                writer.writerow(row)
    except Exception as error: 
        print("Couldn't open " + file_name_PAR)
        print(error)
        return 
    
    print("Done! " + "Records on SD are merged to " + file_name_PAR + ".")
    print("\n")
    return PAR_lists
            
if __name__ == "__main__":
    FILE_NAME_PAR   = "PARdata.csv"
    NUM_DEVICES = 6
    NUM_PORTS = 6
    PUBLISH_PERIOD_MINUTE = 3
    merge_sd_records(FILE_NAME_PAR, NUM_DEVICES, NUM_PORTS, PUBLISH_PERIOD_MINUTE) 