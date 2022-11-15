/*
 SMS sender

 This sketch, for the MKR NB 1500 board, sends an SMS message
 you enter in the serial monitor. Connect your Arduino with the
 SIM card, open the serial monitor, and wait for
 the "READY" message to appear in the monitor. Next, type a
 message to send and press "return". Make sure the serial
 monitor is set to send a newline when you press return.

 Circuit:
 * MKR NB 1500 board
 * Antenna
 * SIM card that can send SMS

 created 25 Feb 2012
 by Tom Igoe
*/

// Include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h" 

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
const char PINNUMBER[] = SECRET_PINNUMBER;
const char APN[] = GPRS_APN; 
const char USERNAME[] = GPRS_LOGIN; 
const char PASSWORD[] = GPRS_PASSWORD;

// initialize the library instance
NB nbAccess;
NB_SMS sms;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  //while(!Serial); 
  delay(3000); 
  Serial.println("SMS Messages Sender");

  // connection state
  bool connected = false;
  digitalWrite(LED_BUILTIN, HIGH); 
  while (!connected) {
    Serial.println(String("trying") + String(PINNUMBER));
    if (nbAccess.begin(PINNUMBER, APN, false, true) == NB_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);  
    }
  }
  Serial.println("NB initialized");

  // send the message
  sms.beginSMS(PHONE_NUMBER);
  sms.print("hello, IoT device is ready.");
  sms.endSMS();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Message sent"); 
  
}

void loop() { 
}
