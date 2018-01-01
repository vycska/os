#include "task_adc.h"
#include "adc.h"
#include "fifos.h"
#include "os.h"
#include "utils.h"
#include "LPC1769.h"

void Task_ADC(void) {
   char s[24];
   unsigned int value;
   double volt;

   Fifo_Uart0_Put("Task_ADC has started", 0);

   ADC_Init();
   while(1) {
      AD0CR |= (1 << 24);       //start conversion now
      while((AD0GDR & (1u << 31)) == 0);        //wait for conversion to finish
      value = (AD0GDR >> 4) & 0xfff;
      volt = 3.3 * value / 4095.0;
      Fifo_HD44780_Put("          ", 0, 10);
      mysprintf(s, "%u %f2V", value, (char *)&volt);
      Fifo_HD44780_Put(s, 0, 10);
      AD0CR &= (~(7 << 24));    //clear start conversion now
      OS_Sleep(1000);
   }
}
