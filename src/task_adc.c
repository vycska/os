#include "task_adc.h"
#include "adc.h"
#include "fifos.h"
#include "os.h"
#include "utils.h"
#include "LPC1769.h"

extern volatile unsigned int adc[8];

void Task_ADC(void) {
   char s[24];
   double value;

   Fifo_Uart0_Put("Task_ADC has started", 0);

   while(1) {
      value = 3.3 * adc[0] / 4095.0;
      Fifo_HD44780_Put("          ", 0, 10);
      mysprintf(s, "%u %f2V", adc[0], (char *)&value);
      Fifo_HD44780_Put(s, 0, 10);
      OS_Sleep(2000);
   }
}
