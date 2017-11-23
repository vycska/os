#include "active_buzzer.h"
#include "LPC1769.h"

void Active_Buzzer_Init(void) {
	PCONP |= (1<<15);
	PINSEL1 &= (~(3<<2)); //function GPIO for P0.17
	PINMODE1 = (PINMODE1&(~(3<<2))) | (2<<2); //neither pull-up, nor pull-down
	FIO0DIR |= (1<<17); //pin is output
	FIO0MASK &= (~(1<<17)); //pin is enabled for reading and writing
	FIO0CLR|=(1<<17);
}

void Active_Buzzer_Command(int k) {
	if(k) FIOSET|=(1<<17);
	else FIOCLR|=(1<<17);
}
