#include "systick.h"
#include "main.h"
#include "LPC1769.h"

void SysTick_Init(int ms) {
   STCTRL = STCTRL & (~((1 << 0) | (1 << 1)));  // disable SysTick during setup
   SHPR3 = (SHPR3 & (~(0x1fU << 27))) | (31U << 27);    // set the lowest priority [31]
   STRELOAD = ms * 1000 * CLOCK - 1;    // reload value
   STCURR = 0;                  // any write to current clears it
   STCTRL |= ((1 << 0) | (1 << 1) | (1 << 2));  // enable, core clock and interrupt arm
}
