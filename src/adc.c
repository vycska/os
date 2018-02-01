#include "adc.h"
#include "main.h"
#include "task_dsm501.h"
#include "LPC1769.h"

extern volatile long long int millis;
//extern int dust_sensor_index;
//extern volatile struct sDustSensor dust_sensor[300];
extern volatile struct Task_DSM501_Data task_dsm501_data;

volatile unsigned short adc[8];

void ADC_Init(void) {
   PCONP |= (1 << 12);                                  //power
   PCLKSEL0 = (PCLKSEL0 & (~(0x3 << 24))) | (1 << 24);  //pclk = cclk

   PINSEL1 = (PINSEL1 & (~(0x3 << 14))) | (1 << 14);    //P0.23 is AD0.0
   PINMODE1 = (PINMODE1 & (~(0x3 << 14))) | (2 << 14);  //neither pull-up, nor pull-down
   PINSEL1 = (PINSEL1 & (~(0x3 << 18))) | (1 << 18);    //P0.25 is AD0.2
   PINMODE1 = (PINMODE1 & (~(0x3 << 18))) | (2 << 18);  //neither pull-up, nor pull-down

   AD0CR = (1<<0) | (1<<2) | ((CLOCK/13)<<8) | (0<<16) | (0<<21) | (0<<24); //select AD0, select AD2, currently no burst, currently ADC powered down, no start

   AD0INTEN = (1<<2);           //conversion on channel 2 [the last] will generate an interrupt
   IPR5 = (IPR5&(~(0x1f<<19))) | (2<<19);
   ISER0 |= (1<<22);            //ADC interrupt enable

   AD0CR |= (1 << 21);          //converter is operational [but not started]
}


void ADC_IRQHandler(void) {
   int B,b,v;
   AD0CR &= (~(1<<16)); //clear burst

   adc[0] = (AD0DR0>>4)&0xfff;
   adc[2] = (AD0DR2>>4)&0xfff;

   /*
   if(millis>60000 && dust_sensor_index<300) {
      dust_sensor[dust_sensor_index].millis = millis;
      dust_sensor[dust_sensor_index].value = adc[2];
      dust_sensor_index+=1;
   }
   */

   B = task_dsm501_data.i/8;
   b = task_dsm501_data.i%8;
   v = (task_dsm501_data.d[B] & (1<<b)) != 0;
   if(adc[2]<1000) {
      if(v) {
         task_dsm501_data.lowsum += 1;
         task_dsm501_data.d[B] &= (~(1<<b));
      }
   }
   else {
      if(!v) {
         task_dsm501_data.lowsum -= 1;
         task_dsm501_data.d[B] |= (1<<b);
      }
   }
   task_dsm501_data.i += 1;
   if(task_dsm501_data.i==MT)
      task_dsm501_data.i = 0;

   ICPR0 |= (1<<22); //clear pending interrupt [?]
}
