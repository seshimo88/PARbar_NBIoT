/*
 * RTC functions to synchronize time in IoT devices.
 * 
 * Modified by Kai Seshimo (Semester 1, 2022) 
 */

#include <RTCZero.h>
#include <TimeLib.h>

/* Create an rtc object */
RTCZero rtc;

//Start and Stop times of the operation. Sleep if outside of the operational time
int startMinutes, startHours, stopMinutes, stopHours;

//Second to synchronize devices. The device uploads the data to Azure if the second of 
//RTC is between 0 and secondToSync.  
const int secondToSync = 10; 

//last time to publish the data to Azure or save the data to SD
int lastPublish, lastSave; 

void RTC_setManually(int hour, int minute, int second, int day, int month, int year){
  delay(5);
  rtc.begin(); // initialize RTC
  delay(5);
  rtc.setTime(hour, minute, second);
  delay(5);
  rtc.setDate(day, month, year);
  delay(5); 
}


//Set RTC from compiler when unploading. 
//This cannot be repeated except the time to upload the codes. 
void RTC_setFromCompile() {
  delay(5);
  rtc.begin(); // initialize RTC
  delay(5);
  RTC_setTimeFromCompile();
  delay(5);
  RTC_setDateFromCompile();
  delay(5); 
  #ifdef DEBUG_DEVICE
  Serial.println("RTC from compile"); 
  #endif
}

//Set RTC from epoch time received from the cellular module. 
bool RTC_setFromNB() {
  int yearTmp = 0, monthTmp = 0, dayTmp = 0, hourTmp = 0, minuteTmp = 0, secondTmp = 0;
  delay(100);   
  unsigned long epoch = getLocalTime(); 
  delay(100);
  if (epoch) {
    epoch += TIME_DIFFERENCE * 3600;
    yearTmp = year(epoch) - 2000; 
    monthTmp = month(epoch);
    dayTmp = day(epoch);
    hourTmp = hour(epoch); 
    minuteTmp = minute(epoch);
    secondTmp = second(epoch);
    #ifdef DEBUG_DEVICE
    Serial.println("RTC from NB: " + String(epoch)); 
    Serial.println(String(monthTmp) + "/" + String(dayTmp) + "/" + String(yearTmp)); 
    Serial.println(String(hourTmp) + ":" + String(minuteTmp) + ":" + String(secondTmp)); 
    #endif 
    delay(5);
    rtc.begin(); // initialize RTC
    delay(5);
    rtc.setTime(hourTmp, minuteTmp, secondTmp);
    delay(5);
    rtc.setDate(dayTmp, monthTmp, yearTmp);
    delay(5); 
    return true; 
  } else {
    return false; 
  }
}

//Return true if RTC's minutes is a multiple of PUBLISH_PERIOD; otherwise return false
bool RTC_isTimeToPublish() {
  delay(5);
  int seconds = rtc.getSeconds(); 
  delay(5);
  int minutes = rtc.getMinutes(); 
  delay(5);
  int hours = rtc.getHours();
  delay(5);  
  int current = seconds + minutes * 60 + hours * 3600; 
  delay(5);
  #ifdef DEBUG_DEVICE
  int period = DEBUG_PUBLISH_PERIOD_MINUTE;
  #else 
  int period = PUBLISH_PERIOD_MINUTE;
  #endif
  if (current - lastPublish > secondToSync * 2 && 0 <= seconds && seconds <= secondToSync) { 
    if (minutes % period == 0) {
      if (hours == 7 && (0 <= minutes || minutes <= 10) {
        return false; 
      } else {
        return true;
      } 
    } else {
      return false; 
    }
  } else {
    return false; 
  }
}

void RTC_resetPublishTime() {
  delay(5);
  int seconds = rtc.getSeconds(); 
  delay(5);
  int minutes = rtc.getMinutes(); 
  delay(5);
  int hours = rtc.getHours(); 
  delay(5);
  lastPublish = seconds + minutes * 60 + hours * 3600;
  delay(5);
}

//Return true if RTC's minutes is a multiple of SAVE_PERIOD; otherwise return false
bool RTC_isTimeToSave() {
  delay(5); 
  int seconds = rtc.getSeconds(); 
  delay(5);
  int minutes = rtc.getMinutes(); 
  delay(5);
  int current = seconds + minutes * 60 + rtc.getHours() * 3600; 
  delay(5);
  #ifdef DEBUG_DEVICE
  int period = DEBUG_SAVE_PERIOD_MINUTE;
  #else 
  int period = SAVE_PERIOD_MINUTE;
  #endif 
  if (current - lastSave > secondToSync * 2 && 0 <= seconds && seconds <= secondToSync) {  
    if (minutes % period == 0) {
      return true; 
    } else {
      return false; 
    }
  } else {
    return false; 
  }
}

void RTC_resetSaveTime() {
  delay(5);
  int seconds = rtc.getSeconds(); 
  delay(5);
  int minutes = rtc.getMinutes(); 
  delay(5);
  int hours = rtc.getHours(); 
  delay(5);
  lastSave = seconds + minutes * 60 + hours * 3600;
  delay(5);
}

//Return true if RTC's time is in night's sleep time; otherwise return false
bool RTC_isTimeToSleep() {
  delay(5);
  int currentHours = rtc.getHours();
  delay(5);
  int currentMinutes = rtc.getMinutes(); 
  delay(5);
  if (currentHours > startHours && currentHours < stopHours) {
    return false; 
  } else if (currentHours == startHours && currentMinutes >= startMinutes){
    return false; 
  } else if (currentHours == stopHours && currentMinutes < stopMinutes) {
    return false; 
  } 
  return true; 
}

//Get the length of sleeping until the start time of the operation or wakeup
int RTC_getSleepSeconds() {
  int sleepHours = 0, sleepMinutes = 0;
  sleepMinutes += 60 - rtc.getMinutes() + startMinutes; 
  delay(5); 
  sleepHours += startHours - rtc.getHours() - 1; 
  delay(5);
  if (rtc.getHours() > startHours) { 
    sleepHours += 24;
  } 
  int totalSleepSeconds = sleepHours * 3600 + sleepMinutes * 60;
  if (totalSleepSeconds > 1) {
    return totalSleepSeconds;  
  } else {
    return 0; 
  }
}

int RTC_getYear() {
  return rtc.getYear(); 
}

int RTC_getMonth() {
  return rtc.getMonth(); 
}

int RTC_getDay() {
  return rtc.getDay(); 
}

int RTC_getHour() {
  return rtc.getHours(); 
}

int RTC_getMinute() {
  return rtc.getMinutes(); 
}

int RTC_getSecond() {
  return rtc.getSeconds(); 
}

/*
 * Set the operation's start and stop time
 * Reference: https://sunrisesunset.willyweather.com.au/qld/darling-downs/gatton.html
 */
void RTC_setStartAndStop() {
  int m = rtc.getMonth();
  delay(5);
  if (m == 1) {
    startHours = 4;
    startMinutes = 40;
    stopHours = 19;
    stopMinutes = 20;     
  } else if (m == 2) {
    startHours = 5;
    startMinutes = 3;
    stopHours = 19;
    stopMinutes = 6;   
  } else if (m == 3) {
    startHours = 5;
    startMinutes = 22;
    stopHours = 18;
    stopMinutes = 37;   
  } else if (m == 4) {
    startHours = 5;
    startMinutes = 38;
    stopHours = 18;
    stopMinutes = 4; 
  } else if (m == 5) {
    startHours = 5;
    startMinutes = 55;
    stopHours = 17;
    stopMinutes = 40;   
  } else if (m == 6) {
    startHours = 6;
    startMinutes = 9;
    stopHours = 17;
    stopMinutes = 34;   
  } else if (m == 7) {
    startHours = 6;
    startMinutes = 9;
    stopHours = 17;
    stopMinutes = 44;   
  } else if (m == 8) {
    startHours = 5;
    startMinutes = 50;
    stopHours = 18;
    stopMinutes = 0;   
  } else if (m == 9) {
    startHours = 5;
    startMinutes = 18;
    stopHours = 18;
    stopMinutes = 14;   
  } else if (m == 10) {
    startHours = 4;
    startMinutes = 44;
    stopHours = 18;
    stopMinutes = 29;   
  } else if (m == 11) {
    startHours = 4;
    startMinutes = 22;
    stopHours = 18;
    stopMinutes = 50;   
  } else if (m == 12) {
    startHours = 4;
    startMinutes = 21;
    stopHours = 19;
    stopMinutes = 12;   
  } else {
    startHours = 5;
    startMinutes = 0;
    stopHours = 19;
    stopMinutes = 0;   
  }
}

// set the rtc time from the compile time:
void RTC_setTimeFromCompile() {
  // get the compile time string:
  String compileTime = String(__TIME__);
  // break the compile time on the colons:
  int h = compileTime.substring(0, 2).toInt();
  int m = compileTime.substring(3, 5).toInt();
  int s = compileTime.substring(6, 8).toInt();

  // set the time from the derived numbers:
  rtc.setTime(h, m, s);
}

// set the rtc time from the compile date:
void RTC_setDateFromCompile() {
  String months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  // get the compile date:
  String compileDate = String(__DATE__);
  // get the date substring
  String monthStr = compileDate.substring(0, 3);

  int m = 0;    // variable for the date as an integer
  // see which month matches the month string:
  for (int i = 0; i < 12; i++) {
    if (monthStr == months[i]) {
      // the first month is 1, but its array position is 0, so:
      m = i + 1;
      // no need to continue the rest of the for loop:
      break;
    }
  }

  // get the day and year as substrings, and convert to numbers:
  int d = compileDate.substring(4, 6).toInt();
  int y = compileDate.substring(9, 11).toInt();
  // set the date from the derived numbers:
  rtc.setDate(d, m, y);
}

//not used
void RTC_setTime(int h, int m, int s) {
  rtc.setTime(h, m, s);
}

//not used
void RTC_setDate(int d, int m, int y) {
  rtc.setDate(d, m, y);
}

//not used
void RTC_setStart(int h, int m) {
  startHours = h; 
  startMinutes = m; 
}

//not used
void RTC_setStop(int h, int m) {
  stopHours = h; 
  stopMinutes = m; 
}
