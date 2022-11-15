/*

 This example tests to see if the modem of the
 MKR NB 1500 board is working correctly. You do not need
 a SIM card for this example.

 Circuit:
 * MKR NB 1500 board
 * Antenna

 Created 12 Jun 2012
 by David del Peral
 modified 21 Nov 2012
 by Tom Igoe
*/

// libraries
#include <MKRNB.h>

// modem verification object
NBModem modem;
NB nbAccess;

void setup() {   
  Serial.begin(115200);
  while(!Serial);
  int adcReading = analogRead(ADC_BATTERY);
   
  //convert to a voltage:
  float batteryVoltage = adcReading * (3.3 / 1023.0);
  //and a percentage:
  float percentage = (batteryVoltage / 3.3) * 100;
  Serial.println("Battery %: " + String(percentage));
  
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(10); 
  
  Serial.println("Starting modem test...");
  
  if (modem.begin()) {
    Serial.println("modem.begin() succeeded");
    digitalWrite(LED_BUILTIN, LOW); 
    delay(10);  
  } else { 
    Serial.println("ERROR, no modem answer.");
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(5000); 
}

void loop() {
}
