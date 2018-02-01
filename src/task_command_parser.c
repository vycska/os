#include "task_command_parser.h"
#include "bme280.h"
#include "e2prom.h"
#include "fifos.h"
#include "fs.h"
#include "hd44780.h"
#include "i2c.h"
#include "iap.h"
#include "led.h"
#include "os.h"
#include "task_log.h"
#include "task_dsm501.h"
#include "utils.h"
#include "utils-asm.h"
#include "LPC1769.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

extern int mtx_filesystem;
extern volatile long long int millis;
extern struct BoardLed_Config boardled_config;
extern struct tcb *RunPt;
extern struct tcb tcbs[NUMTHREADS];
extern struct BME280_Config bme280_config;

extern volatile struct sDustSensor dust_sensor[300];

void params_fill(char *s, unsigned int *params) {
   char *p,                     //pointer
     l,                         //length
     d,                         //all digits
     k;                         //# params
   int i;                       //iterator

   for(p = s, d = 1, k = 0, l = strlen(s), i = 0; i <= l; i++) {
      if(s[i] == ':' || i == l) {
         s[i] = 0;
         params[k + 1] = d ? (params[0] |= (1 << (16 + k)), atoi(p)) : (unsigned int)p;
         k += 1;
         d = 1;
         p = &s[i + 1];
      }
      else {
         d = d && isdigit(s[i]);
      }
   }
   params[0] |= (k & 0xff);
}

//ar paremetras #k yra integer'is, jei ne -- jis yra pointeris i stringa
int params_integer(char k, unsigned int *params) {
   return ((params[0] >> 16) & (1 << k)) != 0;
}

//kiek parametru uzpildyta po params_fill ivykdymo
int params_count(unsigned int *params) {
   return params[0] & 0xff;
}

void Task_Command_Parser(void) {
   char *pString, buf[128];
   int i, smphrFinished;
   unsigned int params[16];

   Fifo_Uart0_Put("Task_Command_Parser has started", 0);
   OS_InitSemaphore(&smphrFinished, 0);
   while(1) {
      Fifo_Command_Parser_Get(&pString);
      mysprintf(buf, "<< %s >>", pString);
      Fifo_Uart0_Put(buf, &smphrFinished);
      OS_Blocking_Wait(&smphrFinished);

      memset(params, 0, 16 * sizeof(unsigned int));
      params_fill(pString, params);

      switch (crc16((unsigned char *)params[1], strlen((char *)params[1]))) {
         case 0xdc2e:{         //test
            Fifo_Uart0_Put("Vyksta testavimas", 0);
            break;
         }
         case 0x178a:{         //system_reset
            SystemReset();
            break;
         }
         case 0x57e6:{         //millis
            mysprintf(buf, "%l", (char *)&millis);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xd89c:{         //live_time
            mysprintf(buf, "%d,%d:%d:%d", (int)(millis / 1000 / 60 / 60 / 24), (int)(millis / 1000 / 60 / 60 % 24), (int)(millis / 1000 / 60 % 60), (int)(millis / 1000 % 60));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0x669b:{
            mysprintf(buf, "active_color: %d, dc: %d, period: %d, counter: %d", (int)boardled_config.active_color, (int)boardled_config.dc, (int)boardled_config.period, (int)boardled_config.counter);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xd480:{         //led_active_color:x
            boardled_config.active_color = params[2];
            break;
         }
         case 0xb400:{         //led_dc
            boardled_config.counter = 0;
            boardled_config.dc = params[2];
            break;
         }
         case 0x4d2c:{         //led_period:x
            boardled_config.counter = 0;
            boardled_config.period = params[2];
            break;
         }
         case 0xb38d:{         //hd44780_test:x:y:s
            Fifo_HD44780_Put((char *)params[4], params[2], params[3]);
            break;
         }
         case 0xca53:{         //task_info
            for(i = 0; i < NUMTHREADS; i++) {
               mysprintf(buf, "%d %s \t\t\t [%d]: sp: %x, \t\t sleep: %d, \t\t block: %x", (int)tcbs[i].id, tcbs[i].name, (int)tcbs[i].priority, tcbs[i].sp, tcbs[i].sleep, tcbs[i].block);
               Fifo_Uart0_Put(buf, &smphrFinished);
               OS_Blocking_Wait(&smphrFinished);
            }
            break;
         }
         case 0xa577:{         //bme280_config
            mysprintf(buf, "dig_T1: %d", (int)bme280_config.dig_T1);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_T2: %d", (int)bme280_config.dig_T2);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_T3: %d", (int)bme280_config.dig_T3);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P1: %d", (int)bme280_config.dig_P1);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P2: %d", (int)bme280_config.dig_P2);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P3: %d", (int)bme280_config.dig_P3);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P4: %d", (int)bme280_config.dig_P4);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P5: %d", (int)bme280_config.dig_P5);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P6: %d", (int)bme280_config.dig_P6);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P7: %d", (int)bme280_config.dig_P7);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P8: %d", (int)bme280_config.dig_P8);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_P9: %d", (int)bme280_config.dig_P9);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H1: %d", (int)bme280_config.dig_H1);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H2: %d", (int)bme280_config.dig_H2);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H3: %d", (int)bme280_config.dig_H3);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H4: %d", (int)bme280_config.dig_H4);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H5: %d", (int)bme280_config.dig_H5);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "dig_H6: %d", (int)bme280_config.dig_H6);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "t_fine: %d", (int)bme280_config.t_fine);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "os_h: %d", (int)bme280_config.os_h);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "os_p: %d", (int)bme280_config.os_p);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "os_t: %d", (int)bme280_config.os_t);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "adc_H: %d", (int)bme280_config.adc_H);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "adc_P: %d", (int)bme280_config.adc_P);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "adc_T: %d", (int)bme280_config.adc_T);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "H: %d", (int)bme280_config.H);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "P: %d", (int)bme280_config.P);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);

            mysprintf(buf, "T: %d", (int)bme280_config.T);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0x7f7e:{         //iap_info
            int len;
            unsigned int result;

            result = iap_read_part_id();
            len = mysprintf(&buf[0], "Part id: 0x%x\r\n", result);
            result = iap_read_boot_code_version();
            len += mysprintf(&buf[len], "Boot code version: %d.%d\r\n", (result >> 8) & 0xff, result & 0xff);
            result = (unsigned int)iap_read_device_serial_number();
            len += mysprintf(&buf[len], "%u %u %u %u", *((unsigned int *)result + 0), *((unsigned int *)result + 1), *((unsigned int *)result + 2), *((unsigned int *)result + 3));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xec26:{         //i2c_test
            mysprintf(buf, "I2C[%d] %d: %d", (int)params[2], (int)params[3], (int)I2C_TestSlave((int)params[2], (unsigned char)params[3]));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0x496b:{         //e2prom_readall
            unsigned char page[32];
            int p, t;

            for(p = 0; p < 8192 / 32; p++) {
               t = Board_E2PROM_ReadBytes(p * 32, page, 32);    //nuskaitau 32 baitus [viena page'a]
               if(t == 1) {
                  mysprintf(buf, "Page %d:", p);
                  Fifo_Uart0_Put(buf, &smphrFinished);
                  OS_Blocking_Wait(&smphrFinished);
                  for(t = i = 0; i < 32; i++)
                     t += mysprintf(&buf[t], "%x ", (unsigned int)page[i]);
                  Fifo_Uart0_Put(buf, &smphrFinished);
                  OS_Blocking_Wait(&smphrFinished);
               }
            }
            break;
         }
         case 0xed5:{          //psr
            mysprintf(buf, "0x%x", GetPSR());
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xe89e:{         //crc16
            mysprintf(buf, "0x%x", (unsigned int)crc16((unsigned char *)params[2], strlen((char *)params[2])));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xf2b6:{         //rtc
            mysprintf(buf, "%u-%u-%u [%u, %u] %d:%d:%d", YEAR & 0xfff, MONTH & 0xf, DOM & 0x1f, DOW & 0x7, DOY & 0x1ff, HOUR & 0x1f, MIN & 0x3f, SEC & 0x3f);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0x34f:{          //rtc_set
            CCR &= (~(1 << 0)); //disable counters
            YEAR = params[2] & 0xfff;
            MONTH = params[3] & 0xf;
            DOM = params[4] & 0x1f;
            DOW = params[5] & 0x7;
            DOY = params[6] & 0x1fff;
            HOUR = params[7] & 0x1f;
            MIN = params[8] & 0x3f;
            SEC = params[9] & 0x3f;
            CCR |= (1 << 0);    //enable counters
            break;
         }
         case 0xa9c1:{         //gpreg
            mysprintf(buf, "GPREG0: 0x%x\r\nGPREG1: 0x%x\r\nGPREG2: 0x%x\r\nGPREG3: 0x%x\r\nGPREG4: 0x%x", GPREG0, GPREG1, GPREG2, GPREG3, GPREG4);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0x6425:{         //gpreg_set
            GPREG0 = params[2];
            GPREG1 = params[3];
            GPREG2 = params[4];
            GPREG3 = params[5];
            GPREG4 = params[6];
            break;
         }
         case 0xc595:{         //fs_info
            mysprintf(buf, "fs size: %d, sector size: %d", FS_TOTAL_SIZE, SECTOR_SIZE);
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            mysprintf(buf, "fs_copy: %d, fs_errors: %d", fs_copy(), fs_errors());
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            for(i = 0; i < DIRECTORY_ENTRIES; i++) {
               if(!fs_direntryempty(i)) {
                  mysprintf(buf, "%d: %s %d", i, fs_filename(i), fs_filesize(i));
                  Fifo_Uart0_Put(buf, &smphrFinished);
                  OS_Blocking_Wait(&smphrFinished);
               }
            }
            break;
         }
         case 0x9c8e: {         //utime
            mysprintf(buf, "%u", utime());
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         case 0xd566: {         //log_file
            char b[16];
            int f, rfs, fs;
            struct tm *ptm;

            f = fs_filesearch(LOG_FILE_NAME);
            if(f != STATUS_ERROR) {
               OS_Blocking_Wait(&mtx_filesystem);
               for(rfs = 0, fs = fs_filesize(f); rfs < fs; rfs += 16) {
                  fs_fileread_seq(f, b, 16);
                  ptm = localtime((time_t *) ((unsigned int *)&b[0]));
                  mysprintf(buf, "%d-%d-%d %d:%d:%d : %d %d %d, %d %d %d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)(*((short *)&b[4])), (int)(*((short *)&b[6])), (int)(*((short *)&b[8])), (int)(*((short *)&b[10])),
                     (int)(*((short *)&b[12])), (int)(*((short *)&b[14])));
                  Fifo_Uart0_Put(buf, &smphrFinished);
                  OS_Blocking_Wait(&smphrFinished);
               }
               OS_Blocking_Signal(&mtx_filesystem);
            }
            break;
         }
         case 0xbf26: {         //temp
            mysprintf(buf, "sizeof(struct Log_Record): %d", (int)sizeof(struct Log_Record));
            Fifo_Uart0_Put(buf, &smphrFinished);
            OS_Blocking_Wait(&smphrFinished);
            break;
         }
         /*
         case 0xf74a: { //adc
            for(i=0;i<300;i++) {
               mysprintf(buf,"%d : %l %u\n",i,(char*)&dust_sensor[i].millis,dust_sensor[i].value);
               Fifo_Uart0_Put(buf, &smphrFinished);
               OS_Blocking_Wait(&smphrFinished);
            }
            break;
         }
         */
         default:{
            Fifo_Uart0_Put("Unknown command", 0);
         }
      }
   }
}
