/*
 * Narrow Band network and MQTT functions to connect to Azure.
 * It publishes a message to "devices/{deviceId}/messages/events/" topic
 * and subscribes to messages on the "devices/{deviceId}/messages/devicebound/#" topic.
 * 
 * Modified by Kai Seshimo (Semester 1, 2022) 
 */

#include <ArduinoECCX08.h>
#include <utility/ECCX08SelfSignedCert.h>
#include <ArduinoMqttClient.h>
#include <MKRNB.h>
#include <ArduinoBearSSL.h>

/////// Enter your sensitive data in arduino_secrets.h
const char pinnumber[]   = SECRET_PINNUMBER;
const char apn[]         = GPRS_APN; 
const char login[]       = GPRS_LOGIN; 
const char password[]    = GPRS_PASSWORD;

const char broker[]      = SECRET_BROKER;
String     deviceId      = SECRET_DEVICE_ID;

NB nbAccess;
GPRS gprs;
NBModem modem;

NBClient      nbClient;            // Used for the TCP socket connection
NBScanner scannerNetworks;
BearSSLClient sslClient(nbClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient); 

//not used
volatile int sensorPeriodAzure, transPeriodAzure;

//Set ECCX08 and MQTT settings
void AZURE_init() {
  if (!ECCX08.begin()) {
      #ifdef DEBUG_DEVICE
      Serial.println("No ECCX08 present!");
      #endif 
      return; 
  }
  // reconstruct the self signed cert
  ECCX08SelfSignedCert.beginReconstruction(0, 8);
  ECCX08SelfSignedCert.setCommonName(ECCX08.serialNumber());
  ECCX08SelfSignedCert.endReconstruction();

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, ECCX08SelfSignedCert.bytes(), ECCX08SelfSignedCert.length());
  // Set the client id used for MQTT as the device id
  mqttClient.setId(deviceId);

  // Set the username to "<broker>/<device id>/api-version=2018-06-30" and empty password
  String username;
  username += broker;
  username += "/";
  username += deviceId;
  username += "/api-version=2018-06-30";
  
  mqttClient.setUsernamePassword(username, "");

  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  //mqttClient.onMessage(onMessageReceived); 

  #ifdef DEBUG_DEVICE
  int period = DEBUG_PUBLISH_PERIOD_MINUTE;
  #else
  int period = PUBLISH_PERIOD_MINUTE;
  #endif

  if (period <= 5) {
    mqttClient.setKeepAliveInterval(10 * 60 * 1000);
  } else {
    mqttClient.setKeepAliveInterval(20 * 60 * 1000);
  }
}

//Not used. This can decrease the power consumption, but this might damage the modem, so 
//call only before nighttime sleep  
void AZURE_end() {
  delay(50);  
  nbAccess.shutdown(); 
  delay(50); 
  ECCX08.end(); 
  delay(50); 
}

void AZURE_restore() {
  nbAccess.begin(pinnumber, apn, true, true);
}

//get the current time from the cellular module
unsigned long getTime() {
  return nbAccess.getTime();
}

//get the current local time from the cellular module
unsigned long getLocalTime() {
  return nbAccess.getLocalTime(); 
}

//Return true if the device connects to Azure successfully; otherwise return false.  
//set NB_only true if you do not need to connect to Azure. 
bool AZURE_connect(bool NB_only) { 
  if (nbAccess.status() != NB_READY) { //|| gprs.status() != GPRS_READY) { 
    if (!connectNB()) {
      #ifdef DEBUG_DEVICE
      Serial.println("NB connection failed"); 
      #endif 
      return false; 
    }
  }
  delay(100); 
  
  if (!NB_only && !mqttClient.connected()) {
    delay(100); //this is necessary as the modem is sensitive
    int Signal = scannerNetworks.getSignalStrength().toInt(); //signal strength in 0-31 scale. 31 means power > 51dBm
    #ifdef DEBUG_DEVICE
    delay(100); 
    Serial.print("signal strength: "); 
    Serial.println(Signal); 
    #endif
    //the default threshold is 0 
    if (Signal <= SIGNAL_THRESHOLD || Signal == 99) {
      //if Signal is 0, the modem is dead. Run TestModem.ino 
      return false; 
    }
    delay(100); //this is necessary as the modem is sensitive
     
    if (!connectMQTT()) {
      #ifdef DEBUG_DEVICE
      Serial.println("MQTT connection failed");
      #endif
      return false;
    }
  } 
  return true; 
}

//Return true if the device is connected to Narrow Band network successfully; otherwise return false. 
bool connectNB() {
  int count = 0;
  bool result = false; 
  #ifdef DEBUG_DEVICE
  Serial.println("Connecting 4G...");
  delay(100); 
  #endif
  while (1) {
    if (nbAccess.begin(pinnumber, apn, false, true) == NB_READY) {
      //if (gprs.attachGPRS() == GPRS_READY) {
      result = true; 
      #ifdef DEBUG_DEVICE
      delay(100); 
      Serial.println("You're connected to 4G");
      #endif
      break;  
      //}
    } else if (count >= 1) { 
      #ifdef DEBUG_DEVICE
      Serial.println("Connection to 4G failed");
      #endif
      break; 
    }
    count++;  
    #ifdef DEBUG_DEVICE
    delay(100); 
    Serial.println("." + String(count));
    #endif
    delay(1000);
  }
  return result; 
}

//Return true if the device is connected to Azure successfully; otherwise return false.
bool connectMQTT() {
  #ifdef DEBUG_DEVICE
  Serial.println("Connecting MQTT broker...");
  delay(100); 
  #endif
  int count = 0;
  bool result = false; 
  while (1) { 
    if (mqttClient.connect(broker, 8883)) {
      #ifdef DEBUG_DEVICE
      delay(100); 
      Serial.println("You're connected to the MQTT broker");
      #endif
      result = true; 
      delay(100); 
      mqttClient.subscribe("devices/" + deviceId + "/messages/devicebound/#");
      break; 
    } else if (count >= 0) { //one trial is enough, but can be changed. 
      #ifdef DEBUG_DEVICE
      Serial.println(mqttClient.connectError());
      Serial.println("Connection to MQTT broker failed");
      #endif
      break; 
    }
    // failed, retry 
    count++; 
    #ifdef DEBUG_DEVICE
    delay(100); 
    Serial.print(".");
    Serial.print(mqttClient.connectError());
    Serial.print(".");
    Serial.println(count);
    #endif
    delay(5000);
  }
  return result;  
}

//Return true if the message is sent to Azure; otherwise false. 
//Additional data can be sent by simplying adding mttClient.print() between beginMessage() and endMessage(). 
bool AZURE_sendData() {
  int yearTmp = RTC_getYear(); 
  int monthTmp = RTC_getMonth(); 
  int dayTmp = RTC_getDay(); 
  int hourTmp = RTC_getHour(); 
  int minuteTmp = RTC_getMinute();
  int secondTmp = RTC_getSecond();
  #ifdef DEBUG_DEVICE
  int deviceNum = 0; 
  #else
  int deviceNum = DEVICE_NUMBER;  
  #endif 
  float parTmp[6];
  for (int x = 0; x < 6; x++) {
    parTmp[x] = PAR_getAvgForAzure(x + 1); 
  }  
  float batteryTmp = BATTERY_get();

  delay(100); 
  int Signal = scannerNetworks.getSignalStrength().toInt(); //signal strength in 0-31 scale. 31 means power > 51dBm 
  if (Signal <= SIGNAL_THRESHOLD || Signal == 99) {
    #ifdef DEBUG_DEVICE
    delay(100); 
    Serial.println("signal is too weak: " + String(Signal));
    #endif
    return false; 
  }
  delay(100); 
  
  // send message, the Print interface can be used to set the message contents 
  mqttClient.beginMessage("devices/" + deviceId + "/messages/events/");
  mqttClient.print(yearTmp); 
  mqttClient.print(",");
  mqttClient.print(monthTmp); 
  mqttClient.print(",");
  mqttClient.print(dayTmp); 
  mqttClient.print(","); 
  mqttClient.print(hourTmp); 
  mqttClient.print(",");
  mqttClient.print(minuteTmp); 
  mqttClient.print(",");
  mqttClient.print(secondTmp); 
  mqttClient.print(",");
  mqttClient.print(deviceNum);  
  mqttClient.print(",");
  for (int x = 0; x < 6; x++) {
    mqttClient.print(parTmp[x]);
    mqttClient.print(",");
  }
  mqttClient.print(batteryTmp);
  mqttClient.print(",");
  mqttClient.print(Signal);
  mqttClient.endMessage();

  #ifdef DEBUG_DEVICE
  delay(100); 
  Serial.println("Data sent to Azure:"); 
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
  Serial.print(deviceNum);  
  Serial.print(",");
  for (int x = 0; x < 6; x++) {
    Serial.print(parTmp[x]);
    Serial.print(",");
  }
  Serial.print(batteryTmp); 
  Serial.print(",");
  Serial.println(Signal);
  Serial.println(); 
  #endif
  return true; 
}

//not used. Return true if the modem is alive; otherwise return false.
bool checkModem() {
  if (modem.begin()) {
    #ifdef DEBUG_DEVICE
    Serial.println("modem is active!");
    #endif
    return true; 
  } else {
    #ifdef DEBUG_DEVICE
    Serial.println("ERROR, no modem answer.");
    #endif
    return false; 
  }
}

//not used. 
void AZURE_onMessageReceived(int messageSize) {
  #ifdef DEBUG_DEVICE
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  #endif
  char input[40];
  int i = 0;  
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    input[i] = (char)mqttClient.read();
    i++; 
  }
  input[i] = '\0';
  #ifdef DEBUG_DEVICE
  Serial.println(input);
  #endif
  int tmp; 
  int type = analyseMessage(input, &tmp); 
  if (type == 6) {
    sensorPeriodAzure = tmp;
  } else if (type == 7) {
    transPeriodAzure = tmp; 
  }
}

//not used. 
int analyseMessage(char input[], int *tmp) {
  int tmpHours, tmpMinutes, tmpSeconds, tmpDay, tmpMonth, tmpYear; 
  removeSpace(input);   
  if (memcmp(input,"time", 4) == 0) {
//    printTime(rtc.getDay(), rtc.getMonth(), rtc.getYear(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()); 
//    printTime(rtc.getDay(), rtc.getMonth(), rtc.getYear(), startHours, startMinutes, seconds); 
//    printTime(rtc.getDay(), rtc.getMonth(), rtc.getYear(), stopHours, stopMinutes, seconds); 
    return 1; 
  } else if (memcmp(input,"setstarttime:", 13) == 0) {
    readTime(13, input, &tmpHours, &tmpMinutes, &tmpSeconds);
    RTC_setStart(tmpHours, tmpMinutes); 
    return 2; 
  } else if (memcmp(input,"setstoptime:", 12) == 0) {
    readTime(12, input, &tmpHours, &tmpMinutes, &tmpSeconds);
    RTC_setStop(tmpHours, tmpMinutes); 
    return 3; 
  } else if (memcmp(input, "setdate:", 8) == 0) {
    readDate(8, input, &tmpDay, &tmpMonth, &tmpYear);
    RTC_setDate(tmpDay, tmpMonth, tmpYear); 
    return 4; 
  } else if (memcmp(input, "settime:", 8) == 0) {
    readTime(8, input, &tmpHours, &tmpMinutes, &tmpSeconds);
    RTC_setTime(tmpHours, tmpMinutes, tmpSeconds); 
    return 5;    
  } else if (memcmp(input, "setsensor:", 10) == 0) {
    *tmp = readPeriod(10, input);
    return 6;
  } else if (memcmp(input, "setazure:", 9) == 0) {
    *tmp = readPeriod(9, input);
    return 7;
  }
  return 0; 
}

//not used. 
void removeSpace(char input[]) {
  int k = 0;
  for(int i = 0; input[i]; i++) {
    input[i] = input[i + k];    
    if (input[i] == ' '|| input[i] == '\t') {
      k++;
      i--;
    }
  }
}

//not used. 
void readTime(int idx, char input[], volatile int *hours, volatile int *minutes, volatile int *seconds) {
  char *tmp = input + idx;
  sscanf(tmp, "%d:%d:%d", hours, minutes, seconds);
  if (*hours < 0 || *hours > 23) {
    *hours = 0; 
  } 
  if (*minutes < 0 || *minutes > 59) {
    *minutes = 0; 
  } 
  *seconds = 0; 
}

//not used. 
void readDate(int idx, char input[], int *dayTmp, int *monthTmp, int *yearTmp) {
  char *tmp = input + idx;
  sscanf(tmp, "%d:%d:%d", dayTmp, monthTmp, yearTmp);
  if (*dayTmp < 1 || *dayTmp > 31) {
    *dayTmp = 1; 
  } 
  if (*monthTmp < 1 || *monthTmp > 12) {
    *monthTmp = 1; 
  } 
  if (*yearTmp < 1 || *yearTmp > 99) {
    *yearTmp = 1; 
  }
}

//not used. 
int readPeriod(int idx, char input[]) {
  char *tmp = input + idx;
  int periodTmp; 
  sscanf(tmp, "%d", &periodTmp);
  //minimum period for sensor is 10 seconds
  if (periodTmp < 10 && idx == 10) {
    periodTmp = 10; 
  //minimum period for Azure is 60 seconds
  } else if (periodTmp < 60 && idx == 9) {
    periodTmp = 60;
  }
  return periodTmp; 
}
