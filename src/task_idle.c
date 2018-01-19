#include "task_idle.h"
#include "fifos.h"
#include "os.h"
#include "utils.h"
#include "utils-asm.h"

extern char _intvecs_size, _text_size, _rodata_size, _data_size, _bss_size, _stack_size, _heap_size;

void Task_Idle(void) {
   char s[32];
   int smphrFinished;

   Fifo_Uart0_Put("Task_Idle has started", 0);

   OS_InitSemaphore(&smphrFinished, 0);

   mysprintf(s, "_intvecs_size: %u", (unsigned int)&_intvecs_size);
   Fifo_Uart0_Put(s, &smphrFinished);
   OS_Blocking_Wait(&smphrFinished);

   mysprintf(s, "_text_size: %u", (unsigned int)&_text_size);
   Fifo_Uart0_Put(s, &smphrFinished);
   OS_Blocking_Wait(&smphrFinished);

   mysprintf(s, "_rodata_size: %u", (unsigned int)&_rodata_size);
   Fifo_Uart0_Put(s, &smphrFinished);
   OS_Blocking_Wait(&smphrFinished);

   mysprintf(s, "_data_size: %u", (unsigned int)&_data_size);
   Fifo_Uart0_Put(s, &smphrFinished);
   OS_Blocking_Wait(&smphrFinished);

   mysprintf(s, "_bss_size: %u", (unsigned int)&_bss_size);
   Fifo_Uart0_Put(s, &smphrFinished);
   OS_Blocking_Wait(&smphrFinished);

   mysprintf(s, "_stack_size: %u", (unsigned int)&_stack_size);
   Fifo_Uart0_Put(s, &smphrFinished);
   OS_Blocking_Wait(&smphrFinished);

   mysprintf(s, "_heap_size: %u", (unsigned int)&_heap_size);
   Fifo_Uart0_Put(s, &smphrFinished);
   OS_Blocking_Wait(&smphrFinished);

   while(1) {
      WaitForInterrupt();
   }
}
