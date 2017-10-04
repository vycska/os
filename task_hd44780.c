#include "task_hd44780.h"
#include "fifos.h"
#include "hd44780.h"

void Task_HD44780(void) {
	char *pString;
	int len=0,line=0,pos=0;
	
	Fifo_Uart0_Put("Task_HD44780 has started",0);
	
	HD44780_Init();

  while(1) {
		Fifo_HD44780_Get(&pString,&len,&line,&pos);
		HD44780_DisplayString(pString,len,line,pos);
  }
}
