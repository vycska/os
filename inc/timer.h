#ifndef __TIMER_H__
#define __TIMER_H__

void Timer0_Init();

void Timer1_Init(void);
void Timer1_Start(void);
unsigned int Timer1_Stop(void);

void Timer2_Init(void);
void TIMER2_IRQHandler(void);

void Timer3_Init(void);
void Timer3_Delay(int);

#endif
