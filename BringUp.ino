#include "BringUpTestBits.h"

void v3BringUp(void){
  static boolean waiting_for_ok = false;
  static boolean first_call = true;
  
  uint32_t tests_to_run = eeprom_read_dword((uint32_t *) 4);     // the test enable bit vector is at address 4
  
  // don't do anything else in this task unless the user says OK, if the tasks calls for that sort of thing
  if(waiting_for_ok && !user_ok_flag){
    return;
  }
   
  user_ok_flag = false;  // reset user flag  
  
  if(tests_to_run != 0){  
    if(tests_to_run & (1UL << OUTPUTS_TEST)){          
      if(waiting_for_ok){
        // we're back, the user must have typed 'ok'
        // clear the pending test flag
        printTestFooterHeader();
        Serial.println(F("LED Test Complete - Resetting.")); 
        printTestFooterTrailer();
        tests_to_run &= ~(1UL << OUTPUTS_TEST); // clear the bit in RAM        
        eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
        checkSuiteComplete(tests_to_run, true);
        doReset();
      }
      else{      
        enableTestAllOutputs();
        printTestBannerHeader();
        Serial.println(F("LED Test: Type 'ok' <enter> after verifying all pins work"));      
        printTestBannerTrailer();  
        waiting_for_ok = true;      
      }
    }       
    else if(tests_to_run & (1UL << CC3000_FIRMWARE_PATCH)){
      enableCC3000Patch();    
      printTestBannerHeader();
      Serial.println(F("CC3000 Patch: Please wait while CC3000 Firmware is updated"));   
      printTestBannerTrailer();     
      firmwareUpdateCC3000();
      tests_to_run &= ~(1UL << CC3000_FIRMWARE_PATCH);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
      printTestFooterHeader();
      Serial.println(F("CC3000 Firmware Patch Complete."));
      printTestFooterTrailer();
      checkSuiteComplete(tests_to_run, true);
      doReset();
    }
    else if(tests_to_run & (1UL << CC3000_TEST)){
      enableTestCC3000(); 
      printTestBannerHeader();
      Serial.println(F("CC3000 Test: Will connect to network and ping the server"));
      printTestBannerTrailer();        
      testCC3000();      
      tests_to_run &= ~(1UL << CC3000_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
      printTestFooterHeader();
      Serial.println(F("CC3000 Test Complete."));
      printTestFooterTrailer();
      checkSuiteComplete(tests_to_run, true);
      doReset();
    }
    else if(tests_to_run & (1UL << SDCARD_TEST)){      
      enableTestSdCard();
      printTestBannerHeader();
      Serial.println(F("SD Card Test: Will write a file to the SD card and read it back. Verify 'testing 1, 2, 3.' is printed")); 
      printTestBannerTrailer();     
      testSdCard();
      tests_to_run &= ~(1UL << SDCARD_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
      
      printTestFooterHeader();
      Serial.println(F("SD Card Test Complete."));
      printTestFooterTrailer();
      checkSuiteComplete(tests_to_run, true);
    }
    else if(tests_to_run & (1UL << SPIFLASH_TEST)){
      enableTestSpiFlashQuick();
      printTestBannerHeader();
      Serial.println(F("SPI Flash Test: Will write a pattern to the SPI Flash and read it back. Verify '0 errors' is printed"));  
      printTestBannerTrailer();   
      testSpiFlash();
      tests_to_run &= ~(1UL << SPIFLASH_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
      
      printTestFooterHeader();
      Serial.println(F("SPI Flash Test Complete."));
      printTestFooterTrailer();
      checkSuiteComplete(tests_to_run, true);   
    }
    else if(tests_to_run & (1UL << WATCHDOG_TEST)){
      tests_to_run &= ~(1UL << WATCHDOG_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom  
      printTestBannerHeader();
      Serial.println(F("Tiny Watchdog Test [should blink throughout remainder of test suite]")); 
      printTestBannerTrailer();
      checkSuiteComplete(tests_to_run, false);      
      initTinyWatchdog();      
    }    
    else if(tests_to_run & (1UL << RFM69_TEST)){          
      if(waiting_for_ok){
        // we're back, the user must have typed 'ok'
        // clear the pending test flag
        printTestFooterHeader();
        Serial.println(F("RFM69 Test Complete.")); 
        printTestFooterTrailer();       
                
        tests_to_run &= ~(1UL << RFM69_TEST); // clear the bit in RAM        
        eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom     
        waiting_for_ok = false;   
        checkSuiteComplete(tests_to_run, true);                
      }
      else{          
        enableTestRfm69receive();
        printTestBannerHeader();
        Serial.println(F("RFM69 Test: Type 'ok' <enter> after verifying packets received"));     
        printTestBannerTrailer();    
        waiting_for_ok = true;      
      }
    }   
    else if(tests_to_run & (1UL << EXTERNAL_XTAL_TEST)){          
      if(waiting_for_ok){
        // we're back, the user must have typed 'ok'
        // clear the pending test flag
        waiting_for_ok = false;
        tests_to_run &= ~(1UL << EXTERNAL_XTAL_TEST); // clear the bit in RAM                                   
        eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom     
        
        printTestFooterHeader();
        Serial.println(F("External Crystal Test Complete.")); 
        printTestFooterTrailer();         
        checkSuiteComplete(tests_to_run, true);                
      }
      else{          
        printTestBannerHeader();
        Serial.println(F("External Crystal Test: Type 'ok' <enter> after verifying 2Hz LED"));
        printTestBannerTrailer(); 
        enableTestExternalCrystal();
        waiting_for_ok = true;      
      }
    }   
    else{
    
    }
  }
  first_call = false;
}

void printTestBannerHeader(){
  Serial.println(F("########################################################################"));
  Serial.print(  F("# "));
}
void printTestBannerTrailer(){
  Serial.println(F("########################################################################"));
  Serial.println();
}

void printTestFooterHeader(){
  Serial.println(F("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"));
  Serial.print(  F("| "));
}
void printTestFooterTrailer(){
  Serial.println(F("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"));
  Serial.println();
}

void checkSuiteComplete(uint32_t test_vector, boolean do_reset){
  if(test_vector == 0){
    printTestBannerHeader();
    Serial.println(F("Test Suite Complete - Resetting.")); 
    printTestBannerTrailer();
    if(do_reset){
      doReset();   
    }
  } 
}
