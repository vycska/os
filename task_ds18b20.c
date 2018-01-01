#include "task_ds18b20.h"
#include "ds18b20.h"
#include "fifos.h"
#include "os.h"
#include "utils.h"
#include "utils-asm.h"
#include "LPC1769.h"
#include <string.h>

extern int mtx_timer[4];

struct Task_DS18B20_Data task_ds18b20_data={0,{-1000,-1000,-1000}};

void Task_DS18B20(void) {
   char s[64];

   unsigned char devices[2][8] = {
      {0x28, 0xff, 0xfd, 0x39, 0x80, 0x16, 0x03, 0x1f}, //water-proof'inis
      {0x28, 0xff, 0x10, 0x90, 0xc1, 0x16, 0x04, 0x0a}  //to-92
   }, data[9];
   int i, j, l, smphrFinished;
   double t;

   Fifo_Uart0_Put("Task_DS18B20 has started", 0);

   DS18B20_Init();

   OS_InitSemaphore(&smphrFinished, 0);

   OS_Blocking_Wait(&mtx_timer[3]);
   j = DS18B20_ReadROM(data);
   OS_Blocking_Signal(&mtx_timer[3]);

   if(j == DS18B20_OK) {
      l = mysprintf(s, "one-wire device: ");
      for(i = 0; i < 8; i++)
         l += mysprintf(&s[l], "0x%x%s", (unsigned int)data[i], i == 7 ? "." : " ");
      Fifo_Uart0_Put(s, &smphrFinished);
      OS_Blocking_Wait(&smphrFinished);
   }

   while(1) {
      OS_Blocking_Wait(&mtx_timer[3]);
      DS18B20_ConvertTAll();
      OS_Blocking_Signal(&mtx_timer[3]);

      OS_Sleep(750);            //conversion time

      for(i = 0; i <= 2; i++) { //gali buti prijungti keletas device'u
         OS_Blocking_Wait(&mtx_timer[3]);
         j = DS18B20_ReadScratchpad((i == 2 ? 0 : devices[i]), data);
         OS_Blocking_Signal(&mtx_timer[3]);

         if(j == DS18B20_OK) {
            t = DS18B20_GetTemperature(data);
            task_ds18b20_data.t[i] = t * 10;    //atkreipk demesi cia (log recordas ir kiek devicu prijungta) !!!
            if(i == task_ds18b20_data.selected) {
               /*
                  l=mysprintf(s,"%d: ",i);
                  for(j=0;j<9;j++)
                  l+=mysprintf(&s[l],"0x%x%s",(unsigned int)data[j],j==8?".":" ");
                  Fifo_Uart0_Put(s,&smphrFinished);
                  OS_Blocking_Wait(&smphrFinished);
                */
               mysprintf(s, "T%d: %f1C", i, (char *)&t);
               Fifo_HD44780_Put(s, 3, 10);
               //Fifo_Uart0_Put(s,&smphrFinished);
               //OS_Blocking_Wait(&smphrFinished);
            }
         }
         else
            task_ds18b20_data.t[i] = -1000;
      }
      OS_Sleep(1000);
   }
}
