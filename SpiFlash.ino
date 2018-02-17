#include "WildFire_SPIFlash.h"
WildFire_SPIFlash flash;

uint16_t num_pages_to_test = 1024;

void setupSpiFlash(void){
  Serial.print(F("SPI Flash Initialization..."));
  if (flash.initialize()){
    Serial.println(F("Complete."));    
  }
  else{
    Serial.println(F("Failed!"));  
  }
}

void testSpiFlash(void){
  if(testSpiFlash_enabled){
    setupSpiFlash();
    
    uint8_t page[256] = {0};
    unsigned long seed = micros();
    long num_error_found = 0;
    
    setupSpiFlash();
    
    Serial.print(F("Erasing Flash chip ... "));
    flash.chipErase();
    while(flash.busy());
    Serial.println(F("DONE"));  
    
    randomSeed(seed);   
    // write a random sequence to flash
    // one page at a time
    // There are 1024 256-byte Pages in the 4M-bit flash
    for(uint16_t jj = 0; jj < num_pages_to_test; jj++){
      for(uint16_t ii = 0; ii < 256; ii++){
        page[ii] = random(256);
      }
      
      if(jj == 0){
        printFlashBlock(page); 
      }      
      
      uint32_t page_address = (uint32_t) jj * 256;
      flash.writeBytes(page_address, page, 256); // write the page
      while(flash.busy()){;}
      
      if((jj % 10) == 0) Serial.print(F("w"));
    }
    Serial.println();
    
    randomSeed(seed); // reset the random sequence generator
    //read back the memory from flash one block at a time
    //and compare the actual contents to expectation
    for(uint16_t jj = 0; jj < num_pages_to_test; jj++){
      uint32_t page_address = (uint32_t) jj * 256;
      flash.readBytes(page_address, page, 256);
      
      if(jj == 0){
        printFlashBlock(page); 
      }          
      
      for(uint16_t ii = 0; ii < 256; ii++){
        uint8_t expected_value = random(256);
        if(page[ii] != expected_value){
          num_error_found++;        
          Serial.println();
          Serial.print(F("Error: @ "));
          Serial.print(page_address + ii);
          Serial.print(F(" Expected ["));
          Serial.print(expected_value);
          Serial.print(F("] got ["));
          Serial.println((uint8_t) page[ii]);         
        }        
      }
      if((jj % 10) == 0) Serial.print(F("r"));
    }    
    Serial.println();
    
    Serial.print(F("SPI Flash Test Complete - ")); 
    Serial.print(num_error_found);
    Serial.println(F(" errors found"));
  
    testSpiFlash_enabled = false;
  }
}

void printFlashBlock(uint8_t * page){
  const uint8_t row_size = 32; // equally divdes 256
  for(uint16_t ii = 0; ii < 256/row_size; ii++){
    uint32_t row_address = (uint32_t) ii * row_size;
    Serial.print(F("@0x"));
    if(row_address < 0x10) Serial.print(F("0")); 
    Serial.print(row_address, HEX);    
    Serial.print(F(": "));
 
    for(uint8_t jj = 0; jj < row_size; jj++){
       if(page[row_address+jj] < 0x10){
         Serial.print(F("0")); 
       }
       Serial.print(page[row_address+jj], HEX);
       Serial.print(" ");
    }
    Serial.println();
  } 
}
