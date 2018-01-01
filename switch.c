#include "switch.h"
#include "os.h"

#include "LPC1769.h"

extern int smphr_switch, status_switch;

void Switch_Init(void) {
   PCONP |= (1 << 15);
   PINSEL4 &= (~(3 << 26));     //function GPIO for P2.13 [switch]
   PINMODE4 |= (3 << 26);       //pull-down resistor
   FIO2DIR &= (~(1 << 13));     //pin is input
   FIO2MASK &= (~(1 << 13));    //pin is enabled for reading and writing
   IO2IntEnR |= (1 << 13);      //enable rising edge interrupt
   IO2IntEnF |= (1 << 13);      //enable falling edge interrupt
   IO2IntClr |= (1 << 13);      //clear interrupt
   IPR5 = (IPR5 & (~(0x1f << 11))) | (2 << 11);
   ISER0 |= (1 << 21);          //enable external interrupt 3
}

void EINT3_IRQHandler(void) {
   status_switch = 0;
   if(((IO2IntStatF & (1 << 13)) ? (status_switch = 1) : 0) || ((IO2IntStatR & (1 << 13)) ? (status_switch = 2) : 0)) { //rising or falling edge on P2.13
      IO2IntEnF &= (~(1 << 13));        //disable falling edge interrupt
      IO2IntEnR &= (~(1 << 13));        //disable rising edge interrupt
      OS_Blocking_Signal(&smphr_switch);
      OS_Suspend();
   }
   IO2IntClr |= (1 << 13);      //clear interrupt
}
