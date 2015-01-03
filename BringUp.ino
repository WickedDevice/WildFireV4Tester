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
        Serial.println(F("LED Test Complete - Resetting.")); 
        tests_to_run &= ~(1UL << OUTPUTS_TEST); // clear the bit in RAM        
        eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
        doReset();
      }
      else{      
        enableTestAllOutputs();
        Serial.println(F("LED Test: Type 'ok' <enter> after verifying all pins work"));        
        waiting_for_ok = true;      
      }
    }       
    else if(tests_to_run & (1UL << CC3000_FIRMWARE_PATCH)){
      enableCC3000Patch();    
      Serial.println(F("CC3000 Patch: Please wait while CC3000 Firmware is updated"));      
      firmwareUpdateCC3000();
      tests_to_run &= ~(1UL << CC3000_FIRMWARE_PATCH);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
      doReset();
    }
    else if(tests_to_run & (1UL << CC3000_TEST)){
      enableTestCC3000(); 
      Serial.println(F("CC3000 Test: Will connect to network and ping the server"));      
      testCC3000();      
      tests_to_run &= ~(1UL << CC3000_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
      doReset();
    }
    else if(tests_to_run & (1UL << SDCARD_TEST)){      
      enableTestSdCard();
      Serial.println(F("SD Card Test: Will write a file to the SD card and read it back. Verify 'testing 1, 2, 3.' is printed"));      
      testSdCard();
      tests_to_run &= ~(1UL << SDCARD_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
    }
    else if(tests_to_run & (1UL << SPIFLASH_TEST)){
      enableTestSpiFlashQuick();
      Serial.println(F("SPI Flash Test: Will write a pattern to the SPI Flash and read it back. Verify '0 errors' is printed"));  
      testSpiFlash();
      tests_to_run &= ~(1UL << SPIFLASH_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
    }
    else if(tests_to_run & (1UL << WATCHDOG_TEST)){
      tests_to_run &= ~(1UL << WATCHDOG_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom      
      initTinyWatchdog();
    }    
    else if(tests_to_run & (1UL << RFM69_TEST)){          
      if(waiting_for_ok){
        // we're back, the user must have typed 'ok'
        // clear the pending test flag
        Serial.println(F("RFM69 Test Complete."));         
        tests_to_run &= ~(1UL << RFM69_TEST); // clear the bit in RAM        
        eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom     
        waiting_for_ok = false;   
      }
      else{          
        enableTestRfm69receive();
        Serial.println(F("RFM69 Test: Type 'ok' <enter> after verifying packets received"));        
        waiting_for_ok = true;      
      }
    }   
    else if(tests_to_run & (1UL << EXTERNAL_XTAL_TEST)){          
      if(waiting_for_ok){
        // we're back, the user must have typed 'ok'
        // clear the pending test flag
        waiting_for_ok = false;
        tests_to_run &= ~(1UL << EXTERNAL_XTAL_TEST); // clear the bit in RAM     
        Serial.println(F("External Crystal Test Complete."));                 
        Serial.println(F("Test Suite Complete - Resetting."));                         
        eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom     
        doReset();
      }
      else{          
        Serial.println(F("External Crystal Test: Type 'ok' <enter> after verifying 2Hz LED"));
        enableTestExternalCrystal();
        waiting_for_ok = true;      
      }
    }   
    else{
    
    }
  }
  first_call = false;
}


