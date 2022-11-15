/*
 * --------------------Base Settings-------------------------- 
 */
//CHECK. If network is weak, comment out this. (Default: defined)
#define USE_AZURE
//It is expected to use SD all time. (Default: defined)                                  
#define USE_SD                                           

//CHECK. Period in millisecond to measure PAR. (Default: 500, Maximum: 1000)
const int MEASURE_PERIOD_MILLISECOND  = 500;  
//CHECK. Period in minute to write the average PAR on SD. (Default: 3)                   
const int SAVE_PERIOD_MINUTE          = 3;  
//CHECK. Period in minute to send the average PAR to Azure. (Default: 3)                     
const int PUBLISH_PERIOD_MINUTE       = 3;                        

/*
 * --------------------Debugging Settings-------------------------
 */
//CHECK. Uncomment this for debugging with PC (Default: undefined)
//#define DEBUG_DEVICE  

//Period in millisecond to measure PAR during debugging. (Default: 5000)
const int DEBUG_MEASURE_PERIOD_MILLISECOND  = 5000; 
//Period in minute to write the average PAR on SD during debugging. (Default: 1)                              
const int DEBUG_SAVE_PERIOD_MINUTE    = 1;
//Period in minute to send the average PAR to Azure during debugging. (Default: 1) 
const int DEBUG_PUBLISH_PERIOD_MINUTE = 1; 

/*
 * --------------------Azure IoT Hub Settings------------------
 */
#define SECRET_BROKER       "XXX.azure-devices.net"  
//CHECK.
#define SECRET_DEVICE_ID    "XXX" 
//CHECK. Must be the same number as DEVICE_ID                           
const int DEVICE_NUMBER      = 0;                                

//Time differnce from GSM. (e.g. GSM+10 for QLD, AUU, Default: 10)
const int TIME_DIFFERENCE    = 10;   
//Threshold of the signal strength. This is mainly for debugging (Default: 0)                            
const int SIGNAL_THRESHOLD   = 0;   

/*
 * --------------------SIM Settings----------------------------
 */
//PIN number of SIM which can be found in secrets.xlsx 
#define SECRET_PINNUMBER "XXXX"                           
#define GPRS_APN         "XXX"               
#define GPRS_LOGIN       ""
#define GPRS_PASSWORD    ""

/*
 * --------------------Pin Settings----------------------------
 */
const int PAR_PINS[] = {A0, A1, A2, A3, A4, A5}; 
const int SELECT_PIN = 4; //SPI select for SD card
const int LED_PIN = 1; 
const int SWITCH_PIN = 2;
