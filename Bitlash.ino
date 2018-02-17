#include "BringUpTestBits.h"
extern uint16_t num_pages_to_test;

boolean user_ok_flag = false;

numvar menu(void) { 
  Serial.println(F("test1 - testAllOutputs"));
  Serial.println(F("        sequentially pulse each output to a LOW state"));
  Serial.println(F("test4 - testSdCard"));
  Serial.println(F("        Create a new file on the SD card and read back its contents"));
  Serial.println(F("test5a- testRfm69transmit"));
  Serial.println(F("        Transmit radio packets, and print them out on acknowledgment"));
  Serial.println(F("test5b- testRfm69receive"));
  Serial.println(F("        Receive radio packets, and print them out on receipt"));
  Serial.println(F("test6 - testSpiFlash Complete"));
  Serial.println(F("        Erase all, write an 'random' pattern, and verify it."));
  Serial.println(F("test7 - testSpiFlash Quick"));
  Serial.println(F("        Erase all, write a one page 'random' pattern, and verify it."));  
  Serial.println(F("test8 - External 32.768kHz Crystal"));
  Serial.println(F("        Blink the onboard LED at 2Hz based on the 32.768kHz crystal."));
  Serial.println(F("initwdt  - Setup Tiny Watchdog to run"));
  Serial.println(F("           set up Tiny Watchdog to run on next reset/startup"));
  Serial.println(F("startwdt - Pet the Tiny Watchdog"));
  Serial.println(F("           Enable petting the Tiny Watchdog (automatically happens after initwdt and reset"));
  Serial.println(F("stopwdt  - Ignore the Tiny Watchdog"));
  Serial.println(F("           Disable petting the Tiny Watchdog to induce a reset if it's active")); 
  Serial.println(F("v3light  - Run the V3 Light Test Suite"));
  Serial.println(F("           Includes:  testAllOutputs, testSdCard, and Watchdog tests")); 
  Serial.println(F("v3heavy  - Run the V3 Heavy Test Suite"));
  Serial.println(F("           Includes:  v3light, plus testSpiFlash, testRfm69, and test XTAL"));   
  Serial.println(F("exit  - terminateTests"));
  Serial.println(F("        if the test(s) in progress can be terminated, terminate it"));
  Serial.println(F("* Note: Tests will generall crash / halt if relevant hardware is not installed on board"));
  Serial.println();
}

numvar enableTestAllOutputs(void) { 
  terminateAllTests();
  testAllOutputs_enabled = true;
  return 0;
}

numvar enableTerminateTests(void) {
  terminateAllTests();
  terminateTests_enabled = true;
  return 0;
}

numvar enableTestSdCard(void){
  terminateAllTests();
  testSdCard_enabled = true;
  return 0;
}

numvar enableTestSpiFlashComplete(void){
  terminateAllTests();
  testSpiFlash_enabled = true;
  num_pages_to_test = 1024;  
  return 0; 
}

numvar enableTestSpiFlashQuick(void){
  terminateAllTests();
  testSpiFlash_enabled = true;
  num_pages_to_test = 1;  
  return 0; 
}

numvar enableTestRfm69transmit(void){
  terminateAllTests(); 
  testRfm69transmit_enabled = true;
  return 0;
}

numvar enableTestRfm69receive(void){
  terminateAllTests();
  testRfm69receive_enabled = true;
  return 0;
}

numvar enableTestExternalCrystal(void){
  terminateAllTests();
  testExternalCrystal_enabled = true;
  return 0;  
}

numvar initTinyWatchdog(void){
  terminateAllTests();
  if(eeprom_read_byte((uint8_t *) 0) != 0x73){
    eeprom_write_byte((uint8_t *) 0, 0x73);  
  }
  Serial.println(F("Press Reset Button to start using Tiny Watchdog"));
  
  for(;;); // spin forever
  
  return 0;
}

numvar startTinyWatchdog(void){
  terminateAllTests();
  usingTinyWatchdog = true;
  return 0;
}

numvar stopTinyWatchdog(void){
  terminateAllTests();
  if(eeprom_read_byte((uint8_t *) 0) != 0xFF){
    eeprom_write_byte((uint8_t *) 0, 0xFF);
  }   
  usingTinyWatchdog = false;
  Serial.println(F("Tiny Watchdog disabled."));
  Serial.println(F("  If Tiny Watchdog is active, a restart will happen within 5 seconds unless you execute 'startwdt'. "));
  Serial.println(F("  Tiny Watchdog will not be activated after restart."));
  return 0;
}

numvar userOK(void){
  Serial.println(F("> OK"));
  user_ok_flag = true;
  return 0; 
}

numvar runV3LightTests(void){
  Serial.println(F("Executing V3 Light Test Suite"));
  uint32_t old_test_bits = eeprom_read_dword((uint32_t *) 4);
  uint32_t test_bits = 0;
  
  test_bits |= (1UL << OUTPUTS_TEST);
  test_bits |= (1UL << SDCARD_TEST);
  test_bits |= (1UL << WATCHDOG_TEST);
  
  Serial.print("Test Bits = ");
  Serial.println(test_bits, HEX);
  if(old_test_bits != test_bits){
    eeprom_write_dword((uint32_t *) 4, test_bits);
  }
  
  if(usingTinyWatchdog){
    stopTinyWatchdog();
    for(;;);
  }  
  
  return 0; 
}

numvar runV3HeavyTests(void){
  Serial.println(F("Executing V3 Heavy Test Suite"));
  uint32_t old_test_bits = eeprom_read_dword((uint32_t *) 4);
  uint32_t test_bits = 0;
  
  test_bits |= (1UL << OUTPUTS_TEST);
  test_bits |= (1UL << SDCARD_TEST);
  test_bits |= (1UL << WATCHDOG_TEST);
  test_bits |= (1UL << SPIFLASH_TEST);
  test_bits |= (1UL << EXTERNAL_XTAL_TEST);
  test_bits |= (1UL << RFM69_TEST);
  
  Serial.print("Test Bits = ");
  Serial.println(test_bits, HEX);
  if(old_test_bits != test_bits){
    eeprom_write_dword((uint32_t *) 4, test_bits);
  }
  
  if(usingTinyWatchdog){
    stopTinyWatchdog();
    for(;;);
  }    
  
  return 0; 
}

void terminateAllTests(){
  terminateTests_enabled = true;
  terminateTests(); 
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
  addBitlashFunction("test8", (bitlash_function) enableTestExternalCrystal);
  addBitlashFunction("initwdt",  (bitlash_function) initTinyWatchdog);    
  addBitlashFunction("startwdt",  (bitlash_function) startTinyWatchdog);    
  addBitlashFunction("stopwdt",  (bitlash_function) stopTinyWatchdog);
  addBitlashFunction("v3light", (bitlash_function) runV3LightTests);
  addBitlashFunction("v3heavy", (bitlash_function) runV3HeavyTests);  
  addBitlashFunction("ok", (bitlash_function) userOK);
  addBitlashFunction("exit",  (bitlash_function) enableTerminateTests);
  
  Serial.println();
  if(!executing_test_suite){
    menu();
  }
}

void bitlashTask(void){
  runBitlash();
}
