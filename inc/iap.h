#ifndef IAP_H
#define IAP_H

#define IAP_ROM_LOCATION								(0x1fff1ff1)
#define IAP_EXECUTE_CMD(a,b)						((void(*)(unsigned int*,unsigned int*))(IAP_ROM_LOCATION))(a,b)

#define IAP_CMD_PREPARE_SECTORS					50
#define IAP_CMD_COPY_RAM_TO_FLASH				51
#define IAP_CMD_ERASE_SECTORS						52
#define IAP_CMD_BLANK_CHECK_SECTORS			53
#define IAP_CMD_READ_PART_ID						54
#define IAP_CMD_READ_BOOT_ROM_VERSION		55
#define IAP_CMD_COMPARE									56
#define IAP_CMD_REINVOKE_ISP						57
#define IAP_CMD_READ_SERIAL_NUMBER			58

#define IAP_STA_CMD_SUCCESS							0
#define IAP_STA_INVALID_COMMAND					1
#define IAP_STA_SRC_ADDR_ERROR					2
#define IAP_STA_DST_ADDR_ERROR					3
#define IAP_STA_SRC_ADDR_NOT_MAPPED			4
#define IAP_STA_DST_ADDR_NOT_MAPPED			5
#define IAP_STA_COUNT_ERROR							6
#define IAP_STA_INVALID_SECTOR					7
#define IAP_STA_SECTOR_NOT_BLANK				8
#define IAP_ST_SECTOR_NOT_PREPARED			9
#define IAP_STA_COMPARE_ERROR						10
#define IAP_STA_BUSY										11

int sector(unsigned int);
int iap_prepare_sectors(unsigned int, unsigned int);
int iap_copy_ram_to_flash(unsigned int, unsigned char *, unsigned int);
int iap_erase_sectors(int, int);
int iap_check_sectors_blank(unsigned int, unsigned int);
unsigned int iap_read_part_id(void);
unsigned short iap_read_boot_code_version(void);
unsigned int *iap_read_device_serial_number(void);
int iap_compare_memory_contents(unsigned int, unsigned int, int);
void iap_reinvoke_isp(void);

#endif
