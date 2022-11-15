# Written by Kai Seshimo (Semester 1, 2022) 

MIN_VOLTAGE   = 0
MAX_VOLTAGE   = 2000 #should be 500 as the maximum voltage from SQ110 is 400. If the reading is greater than 1000mV, the PAR sensor is not connected
CALIBRATION_DEV1_A0 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0  
CALIBRATION_DEV1_A1 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0  
CALIBRATION_DEV1_A2 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV1_A3 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV1_A4 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV1_A5 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 

CALIBRATION_DEV2_A0 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV2_A1 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV2_A2 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV2_A3 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV2_A4 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV2_A5 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 

CALIBRATION_DEV3_A0 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV3_A1 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV3_A2 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV3_A3 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV3_A4 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV3_A5 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 

CALIBRATION_DEV4_A0 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV4_A1 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV4_A2 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV4_A3 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV4_A4 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV4_A5 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 

CALIBRATION_DEV5_A0 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV5_A1 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV5_A2 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV5_A3 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV5_A4 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV5_A5 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 

CALIBRATION_DEV6_A0 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV6_A1 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV6_A2 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV6_A3 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV6_A4 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 
CALIBRATION_DEV6_A5 = lambda x: x * 1 if MIN_VOLTAGE < x and x < MAX_VOLTAGE else 0 

def get_calibrated_PAR(x, dev_num, port_num): 
    if (dev_num == 1):
        if (port_num == 0):
            return CALIBRATION_DEV1_A0(x)
        elif (port_num == 1):
            return CALIBRATION_DEV1_A1(x)
        elif (port_num == 2):
            return CALIBRATION_DEV1_A2(x)
        elif (port_num == 3):
            return CALIBRATION_DEV1_A3(x)
        elif (port_num == 4):
            return CALIBRATION_DEV1_A4(x)
        elif (port_num == 5):
            return CALIBRATION_DEV1_A5(x)
        else:
            return 0
    elif (dev_num == 2):
        if (port_num == 0):
            return CALIBRATION_DEV2_A0(x)
        elif (port_num == 1):
            return CALIBRATION_DEV2_A1(x)
        elif (port_num == 2):
            return CALIBRATION_DEV2_A2(x)
        elif (port_num == 3):
            return CALIBRATION_DEV2_A3(x)
        elif (port_num == 4):
            return CALIBRATION_DEV2_A4(x)
        elif (port_num == 5):
            return CALIBRATION_DEV2_A5(x)
        else:
            return 0
    elif (dev_num == 3):
        if (port_num == 0):
            return CALIBRATION_DEV3_A0(x)
        elif (port_num == 1):
            return CALIBRATION_DEV3_A1(x)
        elif (port_num == 2):
            return CALIBRATION_DEV3_A2(x)
        elif (port_num == 3):
            return CALIBRATION_DEV3_A3(x)
        elif (port_num == 4):
            return CALIBRATION_DEV3_A4(x)
        elif (port_num == 5):
            return CALIBRATION_DEV3_A5(x)
        else:
            return 0
    elif (dev_num == 4):
        if (port_num == 0):
            return CALIBRATION_DEV4_A0(x)
        elif (port_num == 1):
            return CALIBRATION_DEV4_A1(x)
        elif (port_num == 2):
            return CALIBRATION_DEV4_A2(x)
        elif (port_num == 3):
            return CALIBRATION_DEV4_A3(x)
        elif (port_num == 4):
            return CALIBRATION_DEV4_A4(x)
        elif (port_num == 5):
            return CALIBRATION_DEV4_A5(x)
        else:
            return 0
    elif (dev_num == 5):
        if (port_num == 0):
            return CALIBRATION_DEV5_A0(x)
        elif (port_num == 1):
            return CALIBRATION_DEV5_A1(x)
        elif (port_num == 2):
            return CALIBRATION_DEV5_A2(x)
        elif (port_num == 3):
            return CALIBRATION_DEV5_A3(x)
        elif (port_num == 4):
            return CALIBRATION_DEV5_A4(x)
        elif (port_num == 5):
            return CALIBRATION_DEV5_A5(x)
        else:
            return 0
    elif (dev_num == 6):
        if (port_num == 0):
            return CALIBRATION_DEV6_A0(x)
        elif (port_num == 1):
            return CALIBRATION_DEV6_A1(x)
        elif (port_num == 2):
            return CALIBRATION_DEV6_A2(x)
        elif (port_num == 3):
            return CALIBRATION_DEV6_A3(x)
        elif (port_num == 4):
            return CALIBRATION_DEV6_A4(x)
        elif (port_num == 5):
            return CALIBRATION_DEV6_A5(x)
        else:
            return 0
    else: 
        return 0


