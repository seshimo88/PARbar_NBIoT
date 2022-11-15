/*
 * Simple rading of battery-level.
 * 
 * Written by Kai Seshimo (Semester 1, 2022) 
 */

//read the battery level
float BATTERY_get() {
  analogReference(AR_DEFAULT);
  delay(10); 
  int adcReading = analogRead(ADC_BATTERY);
  //convert to a voltage:
  float batteryVoltage = adcReading * (3.3 / 4095.0);
  float percentage = (batteryVoltage / 3.3) * 100;
  if (percentage > 100) {
    percentage = 100;
  }
  return percentage; 
} 
