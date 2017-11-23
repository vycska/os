#ifndef TIMER_H_
#define TIMER_H_

void Timer0_Init(void);
void Timer0_Delay_Int(int,int*);
void Timer0_Delay_NoInt(int);
void TIMER0_IRQHandler(void);

void Timer1_Init(void);
void Timer1_Start(void);
unsigned int Timer1_Stop(void);

void Timer2_Init(void);
void TIMER2_IRQHandler(void);

void Timer3_Init(void);
void Timer3_Delay(int);

#endif
