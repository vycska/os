#include "task_hd44780.h"
#include "fifos.h"
#include "hd44780.h"
#include "os.h"

extern int mtx_timer[4];

void Task_HD44780(void) {
   char *pString;
   int len = 0, line = 0, pos = 0;

   Fifo_Uart0_Put("Task_HD44780 has started", 0);

   OS_Blocking_Wait(&mtx_timer[3]);
   HD44780_Init();
   OS_Blocking_Signal(&mtx_timer[3]);

   while(1) {
      Fifo_HD44780_Get(&pString, &len, &line, &pos);
      OS_Blocking_Wait(&mtx_timer[3]);
      HD44780_DisplayString(pString, len, line, pos);
      OS_Blocking_Signal(&mtx_timer[3]);
   }
}
