#include "task_dsm501.h"
#include "main.h"
#include "os.h"
#include "LPC1769.h"

void ADC2_Init(void) {
   PCONP |= (1 << 12);          //power
   PCLKSEL0 = (PCLKSEL0 & (~(0x3 << 24))) | (1 << 24);  //pclk = cclk
   PINSEL1 = (PINSEL1 & (~(0x3 << 18))) | (1 << 18);    //P0.25 is AD0.2
   PINMODE1 = (PINMODE1 & (~(0x3 << 18))) | (2 << 18);  //neither pull-up, nor pull-down

   AD0CR = (1<<2) | ((CLOCK/13)<<8) | (0<<16) | (0<<21) | (5<<24) | (0<<27); //select AD0.2, clock divider, no burst, power-down, convert on MAT0.3, on rising

   AD0CR |= (1 << 21);          //converter is operational
}

void Task_DSM501(void) {
   while(1) {
      OS_Sleep(3000);
   }
}
