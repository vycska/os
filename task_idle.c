#include "task_idle.h"
#include "fifos.h"
#include "utils-asm.h"

void Task_Idle(void) {
	Fifo_Uart0_Put("Task_Idle has started",0);
	while(1) {
		WaitForInterrupt();
	}
}
