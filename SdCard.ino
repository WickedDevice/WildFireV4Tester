#include <SD.h>
File myFile;

void setupSdCard(void){
  Serial.print(F("SD Card Initialization..."));        
  if (!SD.begin(16)) {
    Serial.println(F("Failed!"));      
  }
  else{
    Serial.println(F("Complete."));     
  }
}

void testSdCard(void){
  if(testSdCard_enabled){
    Serial.println(F("\n\nTest SD"));    
    SD.remove("test.txt");
    
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open("test.txt", FILE_WRITE);
    
    // if the file opened okay, write to it:
    if (myFile) {
      Serial.print(F("Writing to test.txt..."));
      myFile.println(F("testing 1, 2, 3."));
  	// close the file:
      myFile.close();
      Serial.println(F("done."));
    } else {
      // if the file didn't open, print an error:
      Serial.println(F("error opening test.txt"));
      testSdCard_enabled = false;
      return;
    }
    
    // re-open the file for reading:
    myFile = SD.open("test.txt");
    if (myFile) {
      Serial.println("test.txt:");
      
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
      	Serial.write(myFile.read());
      }
      // close the file:
      myFile.close();
    } else {
    	// if the file didn't open, print an error:
      Serial.println(F("error opening test.txt"));
      testSdCard_enabled = false;
      return;
    }
    SD.remove("test.txt");
    Serial.println(F("SD test complete"));
    
    testSdCard_enabled = false;
  }
}
