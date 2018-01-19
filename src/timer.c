#include "timer.h"
#include "main.h"
#include "os.h"
#include "passive_buzzer.h"
#include "utils.h"
#include "LPC1769.h"

extern struct PassiveBuzzerConfig passive_buzzer_config;

/*
Registers:
* IR - interrupt register [clear interrupt, which interrupt source is pending]
* TCR - timer control register [control functions]
* TC - timer counter
* PR - prescaler register
* PC - prescaler counter
* MCR - match control register [control if interrupt is generated, what happens to TC on match]
* MR0-MR3 - match registers
* CTCR - count control register [select between Timer or Counter mode]
*/

void Timer0_Init(void) {        //naudojamas HD44780
   PCONP |= (1 << 1);           //power
   PCLKSEL0 = (PCLKSEL0 & (~(3 << 2))) | (1 << 2);      //clock=CCLK

   T0TCR = 0x02;                //TC and PC counters disabled and reset
   T0CTCR &= (~(3 << 0));       //timer mode
   T0PR = 0;                    //prescale value (TC is incremented every this value+1)
}

void Timer0_Delay(int ns) {
   T0TCR = 0x02;                //TC and PC counters disabled and reset
   T0MCR = (1 << 2);            //counters stopped with disable on MR0
   T0MR0 = MAX2(ns*CLOCK/1000,2)-1;      //match value
   T0TCR = 0x01;                //TC and PC counters and enabled and anti-reset
   while(T0TC < T0MR0);
}

void Timer1_Init(void) {        //naudojamas su switchu
   PCONP |= (1 << 2);           //power
   PCLKSEL0 = (PCLKSEL0 & (~(3 << 4))) | (1 << 4);      //clock=CCLK
   T1TCR = 0x02;                //TC and PC counters disabled and reset
   T1CTCR &= (~(3 << 0));       //timer mode
   T1PR = 0;                    //prescale value (TC is incremented every this value+1)
}

void Timer1_Start(void) {
   T1TCR = 0x02;                //TC and PC counters disabled and reset
   T1MCR = 0;                   //MR register not used
   T1TCR = 0x01;                //TC and PC counters and enabled and anti-reset
}

unsigned int Timer1_Stop(void) {        //grazina ms x10
   unsigned int tc = T1TC;

   T1TCR = 0x02;                //TC and PC counters disabled and reset
   return tc;
}

void Timer2_Init(void) {        //naudojamas muzikai su passive buzzeriu
   PCONP |= (1 << 22);          //power
   PCLKSEL1 = (PCLKSEL1 & (~(3 << 12))) | (1 << 12);    //pclk = cclk
   T2TCR = (T2TCR & (~((1 << 0) | (1 << 1)))) | (1 << 1);       //counters disabled and resetted
   T2CTCR &= (~(3 << 0));       //timer mode
   T2PR = 0;
   T2MCR = (T2MCR & (~((7 << 0) | (7 << 3)))) | (1 << 0) | (1 << 3) | (1 << 4); //interrupt on MR0, interrupt on MR1, reset on MR1

   IPR0 = (IPR0 & (~(0x1fu << 27))) | (2u << 27);
   ISER0 |= (1 << 3);
}

void TIMER2_IRQHandler(void) {
   if(T2IR & (1 << 0)) {        //MR0 interrupts
      Passive_Buzzer_Tone(0);
      T2IR |= (1 << 0);         //reset the interrupt
   }
   if(T2IR & (1 << 1)) {        //MR1 interrupt
      passive_buzzer_config.current_songtone = (passive_buzzer_config.current_songtone + 1) % passive_buzzer_config.count_songtones;
      T2MR0 = passive_buzzer_config.song[passive_buzzer_config.current_songtone].duration * CLOCK * 1000 - 1;
      T2MR1 = (passive_buzzer_config.song[passive_buzzer_config.current_songtone].duration + 50) * CLOCK * 1000 - 1;
      Passive_Buzzer_Tone(passive_buzzer_config.song[passive_buzzer_config.current_songtone].freq);
      T2IR |= (1 << 1);         //reset the interrupt
   }
}
