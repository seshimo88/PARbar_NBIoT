/*
 * Data logger for PAR sensors and SQ110. 
 * There are free two I2C ports, one analog pin and several digital pins for additinal sensorss. 
 * Please look at the manuals under PARbar_NBIoT -> Manual for more information. 
 * 
 * Written by Kai Seshimo (Semester 1, 2022) 
 */

#include "arduino_secrets.h"

bool isTimeCorrect, isDataSaved, isLedOn;
int count; 

void setup() {
  #ifdef DEBUG_DEVICE
  Serial.begin(115200);
  #endif
  delay(1000 * 5); 
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  // NEVER EVER use RESET_N
//  pinMode(SARA_RESETN, OUTPUT);
//  digitalWrite(SARA_RESETN, LOW);
  
  analogReadResolution(12);
   
//  #ifndef USE_AZURE 
//  RTC_setFromCompile();
//  #else 
  RTC_setManually(10, 1, 1, 1, 1, 1); //fake time 
//  #endif
  
  wakeUp();
}

void loop() {
  //sleep if the battery level is low
  #ifndef DEBUG_DEVICE
  if (BATTERY_get() < 81) {
    sleepSeconds(3600 * 1);  //1 Hour 
    delay(100);   
    wakeUp();
  #else 
  if (0) {
  #endif
  } else {
    if (RTC_isTimeToSleep()) {
      #ifdef DEBUG_DEVICE
      Serial.println("sleep"); 
      #endif
      sleepSeconds(RTC_getSleepSeconds()); 
      delay(100); 
      wakeUp(); 
    } 
    
    //turn on LED if swtich button is pushed && last transmission of data is successful
    if (digitalRead(SWITCH_PIN) == HIGH) {
      if (isDataSaved && !isLedOn) {
        digitalWrite(LED_PIN, HIGH);
        isLedOn = true; 
      } else if (!isDataSaved && isLedOn) {
        digitalWrite(LED_PIN, LOW);
        isLedOn = false; 
      }
    } else {
      if (isLedOn) {
        digitalWrite(LED_PIN, LOW);
        isLedOn = false;
      }
    }
  
    //take PAR measurements 
    PAR_accumulate();
    
    //publish sensor values to Azure if the period is passed
    #ifdef USE_AZURE
    if (count < 60 / SAVE_PERIOD_MINUTE && RTC_isTimeToPublish()) { 
      AZURE_init(); //needed as this can stabalize the internet connection
      if (AZURE_connect(false)) { 
        if (!isTimeCorrect) {
            isTimeCorrect = RTC_setFromNB();  
        }
        //send data to Azure 
        isDataSaved = AZURE_sendData();
      } else {
        isDataSaved = false;  
      } 
      PAR_resetForAzure();
      RTC_resetPublishTime(); 
    } 
    if (!isDataSaved) {
      count++; 
    } else {
      count = 0; 
    }
    #endif

    //save sensor values on SD if the period is passed
    #ifdef USE_SD
    if (RTC_isTimeToSave()) { 
      #ifndef USE_AZURE
      if (!isTimeCorrect && AZURE_connect(true)) { //connect to NB only
          isTimeCorrect = RTC_setFromNB();  
      }
      #endif
      if (SD_write()) {
        #ifndef USE_AZURE
        isDataSaved = true; 
        #endif    
      } else {
        #ifndef USE_AZURE
        isDataSaved = false; 
        #endif
      }
      PAR_resetForSD();
      RTC_resetSaveTime();
    } 
    #endif
  }

  #ifdef DEBUG_DEVICE
  delay(DEBUG_MEASURE_PERIOD_MILLISECOND);
  #else
  //LowPower.sleep is not used here to avoid unplugging the battery. 
  //JST connection of the battery is fragile. 
  delay(MEASURE_PERIOD_MILLISECOND); 
  #endif
}

void blinkLED() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(500); 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500); 
  digitalWrite(LED_BUILTIN, LOW);
  delay(500); 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500); 
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
}

void sleepSeconds(int seconds) { 
  if (isLedOn) {   
    digitalWrite(LED_PIN, LOW);
    isLedOn = false; 
    delay(100);
  }
  delay(1000 * seconds); 
}

void wakeUp() { 
  #ifndef DEBUG_DEVICE
  if (BATTERY_get() < 81) {
    return; 
  }
  #else 
  Serial.println("station: " + String(DEVICE_NUMBER)); 
  #endif
  
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(10); 
  
  #ifdef USE_AZURE
  bool isNBOnly = false; 
  AZURE_init();
  #else 
  bool isNBOnly = true;
  #endif  
  if (AZURE_connect(isNBOnly)) { 
    isTimeCorrect = RTC_setFromNB(); 
    isDataSaved = true;
    blinkLED(); 
  } else {
    isTimeCorrect = false; 
    isDataSaved = false;
  }
  
  delay(10); 
  RTC_setStartAndStop();
  delay(10);
  RTC_resetPublishTime();
  delay(10); 
  RTC_resetSaveTime();
  delay(10); 
  PAR_resetForSD();
  delay(10); 
  PAR_resetForAzure();
  delay(10); 
  isLedOn = false;
  count = 0;
  digitalWrite(LED_BUILTIN, LOW); 
  delay(10); 
}
