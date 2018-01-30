#include "rit.h"
#include "led.h"
#include "main.h"
#include "os.h"
#include "LPC1769.h"

extern volatile unsigned short adc[8];
extern long long int millis;
extern struct tcb *RunPt;
extern struct BoardLed_Config boardled_config;

void RIT_Init(void) {           //1ms
   PCONP |= (1 << 16);
   PCLKSEL1 = (PCLKSEL1 & (~(3 << 26))) | (1 << 26);    //PCLK = CCLK
   RICTRL = (RICTRL & (~((1 << 2) | (1 << 3)))) | (1 << 0) | (1 << 1);  //disable for debug; disable timer; clear interrupt flag; enable clear on interrupt
   RICOMPVAL = CLOCK * 1000 * 1 - 1;    //*1 is *ms
   RIMASK = 0;
   RICOUNTER = 0;

   IPR7 = (IPR7 & (~(0x1f << 11))) | (1 << 11); //set interrupt priority
   ISER0 |= (1 << 29);          //enable interrupt

   RICTRL |= (1 << 3);          //enable timer
}

void RIT_IRQHandler(void) {     // process sleeping threads
   struct tcb *cursor;

   //increment milliseconds timer
   millis += 1;

   //process sleeping threads
   for(cursor = RunPt->next; cursor != RunPt; cursor = cursor->next)
      if(cursor->sleep > 0)
         cursor->sleep -= 1;

   //process board led blinking
   boardled_config.counter += 1;
   if(boardled_config.counter == boardled_config.dc * boardled_config.period / 100) {
      Board_LED(0);
   }
   else if(boardled_config.counter >= boardled_config.period) {
      boardled_config.counter = 0;
      boardled_config.active_color = (boardled_config.active_color + 1) % 8;
      if(adc[0] > 500) Board_LED(boardled_config.active_color);
   }

   AD0CR |= (1<<16); //start burst
   RICTRL |= (1 << 0);          //clear interrupt flag
}
