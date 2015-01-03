void setupTinyWatchdog(void){
  if(eeprom_read_byte((uint8_t *) 0) == 0x73){
     Serial.println(F("Tiny Watchdog Initialization... Complete (confirm back side blinking)"));
     tinyWDT.begin(500, 5000); // window 0.5-5 seconds
     usingTinyWatchdog = true;
  }  
}

void tinyWatchdogTask(void){
  if(usingTinyWatchdog){    
     tinyWDT.pet();
  }
}
