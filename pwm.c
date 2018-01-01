#include "pwm.h"
#include "LPC1769.h"

void PWM_Init(void) {
   PCONP |= (1 << 6);           //PWM1 power/clock
   PCLKSEL0 = (PCLKSEL0 & (~(3 << 12))) | (1 << 12);    //peripheral clock for PWM1 is CCLK
   PINSEL3 = (PINSEL3 & (~(3 << 4))) | (2 << 4);        //select PWM1.1 function for P1.18
   PINMODE3 = (PINMODE3 & (~(3 << 4))) | (2 << 4);      //neither pull-up, nor pull-down

   PWM1TCR = (PWM1TCR & (~((1 << 0) | (1 << 3)))) | (1 << 1);   //counters disabled, counters resetted, PWM mode not enabled
   PWM1CTCR &= (~(3 << 0));     //timer mode
   PWM1MCR = (PWM1MCR & (~((7 << 0) | (7 << 3)))) | (1 << 1);   //MR0: interrupts disabled, timer resets, do not stop
   PWM1PR = 0;
   PWM1PCR |= (1 << 9);         //PWM1.1 is always only single edge controlled; enable PWM1 output
}
