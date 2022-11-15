# Written by Kai Seshimo (Semester 1, 2022) 

from download_azure_records import download_azure_records
from merge_sd_records import merge_sd_records
from convert_PAR_to_PAI import convert_PAR_to_PAI

FILE_NAME_PAR = "PARdata.csv" #file name to save PAR data 

DEV_NUM_SQ110           = 6   #device number (1 to 6) to which SQ110 is connected
PORT_NUM_SQ110          = 1   #port number (0 to 5) to which SQ110 is connected

PUBLISH_PERIOD_MINUTE   = 3   #data transmission period in minute (Default: 3)
TIME_DIFFERENCE         = 10  #time difference in hour from GST (Default: 10)

LONGITUDE_DEGREE        = 152.33307151919604 #longitude of the field in degree
LATITUDE_DEGREE         = -27.56270886720687 #latitude of the field in degree

NUM_DEVICES             = 6   #number of total IoT devices. (Default: 6)
NUM_PORTS               = 6   #number of total ports on each device (Default: 6)

def main():
    PAR_lists = download_azure_records(FILE_NAME_PAR, NUM_DEVICES, NUM_PORTS, PUBLISH_PERIOD_MINUTE)
    convert_PAR_to_PAI(PAR_lists, FILE_NAME_PAR, NUM_DEVICES, NUM_PORTS, DEV_NUM_SQ110, PORT_NUM_SQ110, \
                        TIME_DIFFERENCE, LONGITUDE_DEGREE, LATITUDE_DEGREE, False)

    print("\n")
    answer = input("Do you have new SD records to merge? (Type yes or no)  - Your input: ")
    if (answer == ("yes")):
        PAR_lists = merge_sd_records(FILE_NAME_PAR, NUM_DEVICES, NUM_PORTS, PUBLISH_PERIOD_MINUTE) 
        convert_PAR_to_PAI(PAR_lists, FILE_NAME_PAR, NUM_DEVICES, NUM_PORTS, DEV_NUM_SQ110, PORT_NUM_SQ110, \
                            TIME_DIFFERENCE, LONGITUDE_DEGREE, LATITUDE_DEGREE, True)
    else:
        print("No new SD records")


if __name__ == "__main__":
    main()
