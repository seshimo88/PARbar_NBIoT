/*
 * SD card module to write the data. 
 * 
 * Modified by Kai Seshimo (Semester 1, 2022) 
 */

#include <SdFat.h>
SdFat sd;
SdFile myFile;

bool SD_write() {
  if (!sd.begin(SELECT_PIN, SPI_HALF_SPEED)) {
    #ifdef DEBUG_DEVICE
    Serial.println("SD Card Initializing Failed");
    #endif
    return false; 
  }
  char file_name[25];
  char header[15];
  #ifdef DEBUG_DEVICE
  strcpy(header, "debug_sd_dev");
  sprintf(file_name, "%s%d", header, DEVICE_NUMBER);
  #else 
  strcpy(header, "sd_dev");
  sprintf(file_name, "%s%d", header, DEVICE_NUMBER);
  #endif 
  strcat(file_name, ".txt");
  if (myFile.open(file_name, O_RDWR | O_CREAT | O_AT_END)) {
    int yearTmp = RTC_getYear(); 
    int monthTmp = RTC_getMonth(); 
    int dayTmp = RTC_getDay(); 
    int hourTmp = RTC_getHour(); 
    int minuteTmp = RTC_getMinute();
    int secondTmp = RTC_getSecond();  
    float parTmp[6];
    for (int x = 0; x < 6; x++) {
      parTmp[x] = PAR_getAvgForSD(x + 1); 
    } 
    float batteryTmp = BATTERY_get();

    myFile.print(yearTmp); 
    myFile.print(",");
    myFile.print(monthTmp); 
    myFile.print(",");
    myFile.print(dayTmp);
    myFile.print(",");
    myFile.print(hourTmp); 
    myFile.print(",");
    myFile.print(minuteTmp); 
    myFile.print(",");
    myFile.print(secondTmp); 
    myFile.print(",");
    myFile.print(DEVICE_NUMBER);  
    myFile.print(",");
    for (int x = 0; x < 6; x++) {
      myFile.print(parTmp[x]);
      myFile.print(",");
    }
    myFile.println(batteryTmp);
    myFile.close();
  
    #ifdef DEBUG_DEVICE
    Serial.println(String(file_name) + " has a record of below:");
    Serial.print(yearTmp); 
    Serial.print(",");
    Serial.print(monthTmp); 
    Serial.print(",");
    Serial.print(dayTmp); 
    Serial.print(",");
    Serial.print(hourTmp); 
    Serial.print(",");
    Serial.print(minuteTmp); 
    Serial.print(",");
    Serial.print(secondTmp); 
    Serial.print(",");
    Serial.print(DEVICE_NUMBER);  
    Serial.print(",");
    for (int x = 0; x < 6; x++) {
      Serial.print(parTmp[x]);
      Serial.print(",");
    }
    Serial.println(batteryTmp);
    Serial.println(); 
    #endif
    return true; 
  } else {
    #ifdef DEBUG_DEVICE
    Serial.println("File Opening Failed");
    #endif
    return false; 
  }
}
