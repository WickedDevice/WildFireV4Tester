extern uint16_t num_pages_to_test;

numvar menu(void) { 
  Serial.println(F("test1 - testAllOutputs"));
  Serial.println(F("        sequentially pulse each output to a LOW state"));
  Serial.println(F("test2 - testCC3000"));
  Serial.println(F("        connect to network, resolve DNS, and ping a server"));
  Serial.println(F("test3 - firmwareUpdateCC3000"));
  Serial.println(F("        Update the Firmware on the CC3000 to 1.32"));
  Serial.println(F("test4 - testSdCard"));
  Serial.println(F("        Create a new file on the SD card and read back its contents"));
  Serial.println(F("test5a- testRfm69transmit"));
  Serial.println(F("        Transmit radio packets, and print them out on acknowledgment"));
  Serial.println(F("test5b- testRfm69receive"));
  Serial.println(F("        Receive radio packets, and print them out on receipt"));
  Serial.println(F("test6 - testSpiFlash Complete"));
  Serial.println(F("        Erase all, write an 'random' pattern, and verify it"));
  Serial.println(F("test7 - testSpiFlash Quick"));
  Serial.println(F("        Erase all, write a one page 'random' pattern, and verify it"));  
  Serial.println(F("exit  - terminateTests"));
  Serial.println(F("        if the test(s) in progress can be terminated, terminate it"));
  Serial.println();
}

numvar enableTestAllOutputs(void) { 
  testAllOutputs_enabled = true;
  return 0;
}

numvar enableTerminateTests(void) { 
  terminateTests_enabled = true;
  return 0;
}

numvar enableTestSdCard(void){
  testSdCard_enabled = true;
  return 0;
}

numvar enableTestSpiFlashComplete(void){
  testSpiFlash_enabled = true;
  num_pages_to_test = 1024;  
  return 0; 
}

numvar enableTestSpiFlashQuick(void){
  testSpiFlash_enabled = true;
  num_pages_to_test = 1;  
  return 0; 
}

numvar enableTestRfm69transmit(void){
  testRfm69transmit_enabled = true;
  return 0;
}

numvar enableTestRfm69receive(void){
  testRfm69receive_enabled = true;
  return 0;
}

void setupBitlash(void){
  initBitlash(115200);
  
  addBitlashFunction("menu", (bitlash_function) menu);  
  addBitlashFunction("test1", (bitlash_function) enableTestAllOutputs);
  addBitlashFunction("test4", (bitlash_function) enableTestSdCard);
  addBitlashFunction("test5a", (bitlash_function) enableTestRfm69transmit); 
  addBitlashFunction("test5b", (bitlash_function) enableTestRfm69receive);  
  addBitlashFunction("test6", (bitlash_function) enableTestSpiFlashComplete);
  addBitlashFunction("test7", (bitlash_function) enableTestSpiFlashQuick);    
  addBitlashFunction("exit",  (bitlash_function) enableTerminateTests);
  
  Serial.println();
  menu();
}

void bitlashTask(void){
  runBitlash();
}
