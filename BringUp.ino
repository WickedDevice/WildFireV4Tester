#include "BringUpTestBits.h"

void v3BringUp(void){
  static boolean waiting_for_ok = false;
  
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
        soft_reset();
      }
      else{      
        Serial.println(F("LED Test: Type 'ok' <enter> after verifying all pins work"));
        enableTestAllOutputs();
        waiting_for_ok = true;      
      }
    }       
    else if(tests_to_run & (1UL << CC3000_FIRMWARE_PATCH)){
      Serial.println(F("CC3000 Patch: Please wait while CC3000 Firmware is updated"));
      enableCC3000Patch();    
      firmwareUpdateCC3000();
      tests_to_run &= ~(1UL << CC3000_FIRMWARE_PATCH);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
      soft_reset();
    }
    else if(tests_to_run & (1UL << CC3000_TEST)){
      Serial.println(F("CC3000 Test: Will connect to network and ping the server"));
      enableTestCC3000(); 
      testCC3000();      
      tests_to_run &= ~(1UL << CC3000_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
      soft_reset();
    }
    else if(tests_to_run & (1UL << SDCARD_TEST)){
      enableTestSdCard();
      testSdCard();
      tests_to_run &= ~(1UL << SDCARD_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
    }
    else if(tests_to_run & (1UL << SPIFLASH_TEST)){
      enableTestSpiFlashQuick();
      testSpiFlash();
      tests_to_run &= ~(1UL << SPIFLASH_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom
    }
    else if(tests_to_run & (1UL << WATCHDOG_TEST)){
      tests_to_run &= ~(1UL << WATCHDOG_TEST);  // clear the bit in RAM        
      eeprom_write_dword((uint32_t *) 4, tests_to_run); // write it back to eeprom      
      initTinyWatchdog();
    }    
    else{
    
    }
  }
}
