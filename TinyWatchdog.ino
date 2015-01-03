void setupTinyWatchdog(void){
  if(eeprom_read_byte((uint8_t *) 0) == 0x73){
     Serial.println(F("Tiny Watchdog Initialization... Complete (confirm back side blinking)"));
     tinyWDT.begin(500, 60000); // window 1-60s
     usingTinyWatchdog = true;
  }  
}

void tinyWatchdogTask(void){
  if(usingTinyWatchdog){    
     tinyWDT.pet();
  }
}
