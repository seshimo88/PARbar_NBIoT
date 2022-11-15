/*
 * Analog read functinos for PAR sensors and SQ110.
 * 
 * Written by Kai Seshimo (Semester 1, 2022) 
 */

//Azure and SD records are seperate so that 
//a user can set different periods for Azure and SD  
int parTotalForAzure[] = {0, 0, 0, 0, 0, 0};
int parTotalForSD[]    = {0, 0, 0, 0, 0, 0};  
int parCountForAzure = 0, parCountForSD = 0; 

//The maximum reading for A0-A5 is set to 1V
void PAR_accumulate() {
  analogReference(AR_INTERNAL1V0); //set to 1V
  delay(10); 
  for (int x = 0; x < 6; x++) {
    int value = analogRead(PAR_PINS[x]);
    parTotalForAzure[x] += value; 
    parTotalForSD[x] += value; 
    #ifdef DEBUG_DEVICE
    Serial.println("A" + String(x) + "(mV): " + String(value * (1.0 / 4095.0) * 1000));
    #endif
  }
  #ifdef DEBUG_DEVICE
  Serial.println(".");
  #endif
  parCountForSD++;
  parCountForAzure++;  
}

float PAR_getAvgForSD(int x) {
  if (x >= 1 && x <= 6) {
    return (parTotalForSD[x - 1] / parCountForSD) * (1.0 / 4095.0) * 1000; 
  } else {
    return 0; 
  }
}

float PAR_getAvgForAzure(int x) {
  if (x >= 1 && x <= 6) {
    return (parTotalForAzure[x - 1] / parCountForAzure) * (1.0 / 4095.0) * 1000; 
  } else {
    return 0; 
  }
}

void PAR_resetForSD() {
  for (int x = 0; x < 6; x++) {
    parTotalForSD[x] = 0;  
  }
  parCountForSD = 0; 
}

void PAR_resetForAzure() {
  for (int x = 0; x < 6; x++) {
    parTotalForAzure[x] = 0;  
  }
  parCountForAzure = 0; 
}
