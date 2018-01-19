#include "iap.h"
#include "main.h"
#include "utils-asm.h"
#include "LPC1769.h"

static unsigned int command[5] = { 0 }, result[5] = {
0};

//Sectors in LPC176x: 0-15 -- size 4kB, 16-29 -- size 32kB; s0 -- 0x0001.0000-0x0001.7fff, s29: 0x0007.8000-0x0007.ffff
int sector(unsigned int adr) {
   return (adr >>= 12) >= 16 ? 14 + (adr >> 3) : adr;
}

int iap_prepare_sectors(unsigned int start_sector, unsigned int end_sector) {
   command[0] = IAP_CMD_PREPARE_SECTORS;
   command[1] = start_sector;
   command[2] = end_sector;
   IAP_EXECUTE_CMD(command, result);
   return result[0];
}

// "dest_address" must be 256 bytes boundary; "source address" must be word boundary; "len" must be 256 | 512 | 1024 | 4096
int iap_copy_ram_to_flash(unsigned int dest_address, unsigned char *source_address, unsigned int len) {
   _DSB();
   DisableInterrupts();
   iap_prepare_sectors(sector(dest_address), sector(dest_address + len - 1));
   command[0] = IAP_CMD_COPY_RAM_TO_FLASH;
   command[1] = dest_address;
   command[2] = (unsigned int)source_address;
   command[3] = len;
   command[4] = CLOCK * 1000;
   IAP_EXECUTE_CMD(command, result);
   EnableInterrupts();
   return result[0];
}

int iap_erase_sectors(int start_sector, int end_sector) {
   _DSB();
   DisableInterrupts();
   iap_prepare_sectors(start_sector, end_sector);
   command[0] = IAP_CMD_ERASE_SECTORS;
   command[1] = start_sector;
   command[2] = end_sector;
   command[3] = CLOCK * 1000;
   IAP_EXECUTE_CMD(command, result);
   EnableInterrupts();
   return result[0];
}

int iap_check_sectors_blank(unsigned int start_sector, unsigned int end_sector) {
   command[0] = IAP_CMD_BLANK_CHECK_SECTORS;
   command[1] = start_sector;
   command[2] = end_sector;
   IAP_EXECUTE_CMD(command, result);
   return result[0];
}

unsigned int iap_read_part_id(void) {
   command[0] = IAP_CMD_READ_PART_ID;
   IAP_EXECUTE_CMD(command, result);
   return result[1];
}

unsigned short iap_read_boot_code_version(void) {
   command[0] = IAP_CMD_READ_BOOT_ROM_VERSION;
   IAP_EXECUTE_CMD(command, result);
   return (unsigned short)result[1];
}

unsigned int *iap_read_device_serial_number(void) {
   command[0] = IAP_CMD_READ_SERIAL_NUMBER;
   IAP_EXECUTE_CMD(command, result);
   return &result[1];
}

// addresses must be at word boundary; len must be multiple of 4
int iap_compare_memory_contents(unsigned int address1, unsigned int address2, int len) {
   command[0] = IAP_CMD_COMPARE;
   command[1] = address1;
   command[2] = address2;
   command[3] = len;
   IAP_EXECUTE_CMD(command, result);
   return result[0];
}

void iap_reinvoke_isp(void) {
   command[0] = IAP_CMD_REINVOKE_ISP;
   IAP_EXECUTE_CMD(command, result);
}
