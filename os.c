#include "os.h"
#include "main.h"
#include "uart.h"
#include "utils-asm.h"
#include "LPC1769.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

struct tcb tcbs[NUMTHREADS],*RunPt;
int Stacks[NUMTHREADS][STACKSIZE];

void OS_Init(int k,...) {
	int i;
	va_list va;
	
	va_start(va,k);
	for(i=0;i<k;i++) {
		tcbs[i].id = i;
		tcbs[i].name = va_arg(va,char*);
		tcbs[i].priority = va_arg(va,int);
		tcbs[i].sleep = 0;																						// thread initially not sleeeping
		tcbs[i].block = 0;																						// thread initially not blocked
		Stacks[i][STACKSIZE-1] = 0x01000000;													// PSR [thumb bit]
		Stacks[i][STACKSIZE-2] = (int)va_arg(va,void(*)(void));				// PC
		Stacks[i][STACKSIZE-3] = 0x14141414;													// R14
		Stacks[i][STACKSIZE-4] = 0x12121212;													// R12
		Stacks[i][STACKSIZE-5] = 0x03030303;													// R3
		Stacks[i][STACKSIZE-6] = 0x02020202;													// R2
		Stacks[i][STACKSIZE-7] = 0x01010101;													// R1
		Stacks[i][STACKSIZE-8] = 0x00000000;													// R0
		Stacks[i][STACKSIZE-9] = 0x11111111;													// R11
		Stacks[i][STACKSIZE-10] = 0x10101010;													// R10
		Stacks[i][STACKSIZE-11] = 0x09090909;													// R9
		Stacks[i][STACKSIZE-12] = 0x08080808;													// R8
		Stacks[i][STACKSIZE-13] = 0x07070707;													// R7
		Stacks[i][STACKSIZE-14] = 0x06060606;													// R6
		Stacks[i][STACKSIZE-15] = 0x05050505;													// R5
		Stacks[i][STACKSIZE-16] = 0x04040404;													// R4
		tcbs[i].sp = &Stacks[i][STACKSIZE-16];												// thread stack pointer
		tcbs[i].next = &tcbs[i+1!=k?(i+1):0];													// 0 points to 1, 1 points to 2, and so on
	}
	va_end(va);
	
  RunPt = &tcbs[0];
}

void Scheduler(void) {
	int i,best_priority;
	struct tcb *cursor,*best_task;

	for(best_task=RunPt,best_priority=128,cursor=RunPt->next,i=0;i<NUMTHREADS;cursor=cursor->next,i++)
		if(cursor->priority<best_priority && cursor->block==0 && cursor->sleep==0) {
			best_priority=cursor->priority;
			best_task=cursor;
		}
	RunPt=best_task;
}

void OS_InitSemaphore(int *s,int v) {
	*s=v;
}

void OS_Spinning_Signal(int *s) {
	DisableInterrupts();
	*s+=1;
	EnableInterrupts();
}

void OS_Spinning_Wait(int *s) { //spinlock semaphore wait
	DisableInterrupts();
	while(*s<=0) {
		EnableInterrupts();
		DisableInterrupts();
	}
	*s-=1;
	EnableInterrupts();
}

void OS_Suspend_Wait(int *s) {
	DisableInterrupts();
	while(*s<=0) {
		EnableInterrupts();
		OS_Suspend();
		DisableInterrupts();
	}
	*s-=1;
	EnableInterrupts();
}

void OS_Blocking_Wait(int *s) { //blocking semaphore wait
	DisableInterrupts();
	*s-=1;
	if(*s<0) { //no pass available, thus block
		RunPt->block=s;
		EnableInterrupts();
		OS_Suspend();
	}
	EnableInterrupts();
}

void OS_Blocking_Signal(int *s) {
	int i;
	struct tcb *cursor;
	DisableInterrupts();
	*s+=1;
	if(*s<=0) { //wake up one thread
		for(cursor=RunPt->next,i=0;i<NUMTHREADS && cursor->block!=s;i+=1,cursor=cursor->next);
		if(i<NUMTHREADS) cursor->block=0;
	}
	EnableInterrupts();
}

void SystemReset(void) {
	_DSB();
	AIRCR = (AIRCR & (~(0xffffU<<16) | (1<<2) | (1<<1) | (1<<0))) | (0x5faU<<16) | (1<<2);
	_DSB();
	while(1);
}

void OS_Suspend(void) {
	STCURR=0;
	ICSR |= (1<<26); // SysTick exception set pending
}

void OS_Sleep(int ms) {
	RunPt->sleep=ms;
	OS_Suspend();
}
