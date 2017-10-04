#ifndef OS_H
#define OS_H

#include <stdint.h>

#define NUMTHREADS	9			// number of threads
#define STACKSIZE		150		// number of 32-bit words in stack

struct tcb {
	int		*sp,
				*block,
				sleep;
	char	id,
				priority,
				*name;
	struct tcb *next;
};

void OS_Init(int,...);

void Scheduler(void);

void OS_Suspend(void);
void OS_Sleep(int);

void OS_InitSemaphore(int*,int);

void OS_Suspend_Wait(int*);
void OS_Spinning_Wait(int*);
void OS_Spinning_Signal(int*);

void OS_Blocking_Wait(int*);
void OS_Blocking_Signal(int*);

void SystemReset(void);

#endif
