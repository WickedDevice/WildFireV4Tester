#include <WildFire_CC3000.h>
#include <WildFire.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include "utility/nvmem.h"
#include "driverpatchinc_1_14.h"

extern boolean restart_required_buildtest;
boolean restart_required_fwpatch = false;

void firmwareUpdateCC3000(void){
  wf.begin();
  
  static uint8_t ucStatus_Dr, return_status = 0xFF;
  static uint8_t counter = 0;
  
  // array to store RM parameters from EEPROM
  static unsigned char cRMParamsFromEeprom[128];
    
  // 2 dim array to store address and length of new FAT
  static uint16_t aFATEntries[2][NVMEM_RM_FILEID + 1] =
  /*  address   */ {{0x50,   0x1f0,  0x1390, 0x0390, 0x2390, 0x4390, 0x6390, 0x63a0, 0x63b0, 0x63f0, 0x6430, 0x6830},
  /*  length  */    {0x1a0,  0x1a0,  0x1000, 0x1000, 0x2000, 0x2000, 0x10,   0x10,   0x40,   0x40,   0x400,  0x200 }};
  /* 0. NVS */
  /* 1. NVS Shadow */
  /* 2. Wireless Conf */
  /* 3. Wireless Conf Shadow */
  /* 4. BT (WLAN driver) Patches */
  /* 5. WiLink (Firmware) Patches */
  /* 6. MAC addr */
  /* 7. Frontend Vars */
  /* 8. IP config */
  /* 9. IP config Shadow */
  /* 10. Bootloader Patches */
  /* 11. Radio Module params */
  /* 12. AES128 for smart config */
  /* 13. user file */
  /* 14. user file */
  /* 15. user file */  
  
  if(firmwareUpdateCC3000_enabled){
    if(restart_required_buildtest || restart_required_fwpatch){
      Serial.println(F("A restart is required before executing this test...")); 
      Serial.println(F("No action taken."));
      firmwareUpdateCC3000_enabled = false;
      return;
    }
    else{
      Serial.println(F("CC3000 Firmware Patch Starting ... please DO NOT RESTART part way."));
    }
    
    restart_required_fwpatch = true;
    
    displayDriverMode();
    displayFreeRam();
  
    /* Initialise the module */
    Serial.println(F("\nInitialising the CC3000 ..."));
    if (!cc3000.begin(2)) // init with NO patches!
    {
      Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
      while(1);
    }
  
    displayFirmwareVersion();
    displayMACAddress();
  
    return_status = 1;
    uint8_t index;
    uint8_t *pRMParams;
  
    while ((return_status) && (counter < 3)) {
      // read RM parameters
      // read in 16 parts to work with tiny driver
  
      return_status = 0;
      pRMParams = cRMParamsFromEeprom;
  
      for (index = 0; index < 16; index++) {
        return_status |= nvmem_read(NVMEM_RM_FILEID, 8, 8*index, pRMParams);
        Serial.print(F("\n\rRead NVRAM $")); Serial.print(8*index); Serial.print("\t");
        for(uint8_t x=0; x<8; x++) {
           Serial.print("0x"); Serial.print(pRMParams[x], HEX); Serial.print(", ");
        }
        pRMParams += 8;
      }
      counter++;
    }
    // if RM file is not valid, load the default one
    if (counter == 3) {
      Serial.println(F("\n\rLoad default params"));
      pRMParams = (uint8_t *)cRMdefaultParams;
    }
  
    return_status = 1;
  
    while (return_status) {
      // write new FAT
      return_status = fat_write_content(aFATEntries[0], aFATEntries[1]);
      Serial.print(F("Wrote FAT entries: ")); Serial.println(return_status, DEC);
    }
  
    return_status = 1;
  
    Serial.println(F("Write params"));
  
    while (return_status) {
      // write RM parameters
      // write in 4 parts to work with tiny driver
  
      return_status = 0;
  
      for (index = 0; index < 4; index++) {
        return_status |= nvmem_write(NVMEM_RM_FILEID, 32, 32*index, (pRMParams + 32*index));
        Serial.println(F("Wrote 32 bytes to NVRAM"));
      }
    }
    Serial.println(F("Wrote params"));
  
    return_status = 1;
  
    // write back the MAC address, only if exist
    if (MACvalid) {
      // zero out MCAST bit if set
      cMacFromEeprom[0] &= 0xfe;
      while (return_status) {
        return_status = nvmem_set_mac_address(cMacFromEeprom);
      }
    }
  
    ucStatus_Dr = 1;
    Serial.println(F("Writing driver patch"));
  
    while (ucStatus_Dr) {
      //writing driver patch to EEPRROM - PORTABLE CODE
      // Note that the array itself is changing between the different Service Packs
      ucStatus_Dr = nvmem_write_patch(NVMEM_WLAN_DRIVER_SP_FILEID, drv_length, wlan_drv_patch);
    }
  
  
    Serial.println(F("Wrote driver patch"));
  
    Serial.println(F("Writing firmware"));
  
    unsigned char ucStatus_FW = 1;
  
    while (ucStatus_FW) {
      //writing FW patch to EAPRROM  - PORTABLE CODE
      //Note that the array itself is changing between the different Service Packs
      ucStatus_FW = nvmem_write_patch(NVMEM_WLAN_FW_SP_FILEID, fw_length, fw_patch);
    }
  
    Serial.println(F("Starting w/patches"));
  
    cc3000.reboot();
  
    Serial.println(F("Patched!"));
    displayFirmwareVersion();
    displayMACAddress();   
   
    Serial.println(F("CC3000 Firmware Update Complete. [Restart Required]"));     
    firmwareUpdateCC3000_enabled = false;
      
  }
}

//*****************************************************************************
//
//! fat_write_content
//!
//! \param[in] file_address  array of file address in FAT table:\n
//!            this is the absolute address of the file in the EEPROM.
//! \param[in] file_length  array of file length in FAT table:\n
//!            this is the upper limit of the file size in the EEPROM.
//!
//! \return on succes 0, error otherwise
//!
//! \brief  parse the FAT table from eeprom
//
//*****************************************************************************
uint8_t fat_write_content(uint16_t *file_address, uint16_t *file_length)
{
  uint16_t  index = 0;
  uint8_t   ucStatus;
  uint8_t   fatTable[48];
  uint8_t*  fatTablePtr = fatTable;
  uint8_t LS[3]  = "LS";

  // first, write the magic number
  ucStatus = nvmem_write(16, 2, 0, LS);

  for (; index <= NVMEM_RM_FILEID; index++)
  {
    // write address low char and mark as allocated
    *fatTablePtr++ = (uint8_t)(file_address[index] & 0xff) | _BV(0);

    // write address high char
    *fatTablePtr++ = (uint8_t)((file_address[index]>>8) & 0xff);

    // write length low char
    *fatTablePtr++ = (uint8_t)(file_length[index] & 0xff);

    // write length high char
    *fatTablePtr++ = (uint8_t)((file_length[index]>>8) & 0xff);
  }

  // second, write the FAT
  // write in two parts to work with tiny driver
  ucStatus = nvmem_write(16, 24, 4, fatTable);
  ucStatus = nvmem_write(16, 24, 24+4, &fatTable[24]);

  // third, we want to erase any user files
  memset(fatTable, 0, sizeof(fatTable));
  ucStatus = nvmem_write(16, 16, 52, fatTable);
  
  return ucStatus;
}

