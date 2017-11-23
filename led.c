#include "led.h"

#include "LPC1769.h"

struct BoardLed_Config boardled_config={2,5,1000,0};

void Board_LED_Init(void) {
	PCONP |= (1<<15);
	
	//P0.22 [red led]
	PINSEL1&=(~(3<<12));
	PINMODE1=(PINMODE1&(~(3<<12))) | (1<<13);
	FIO0DIR|=(1<<22);
	FIO0MASK&=(~(1<<22));
	FIO0SET|=(1<<22);

	//P3.25 [green led]
	PINSEL7&=(~(3<<18));
	PINMODE7=(PINMODE7&(~(3<<18))) | (1<<19);
	FIO3DIR|=(1<<25);
	FIO3MASK&=(~(1<<25));
	FIO3SET|=(1<<25);

	//P3.26 [blue led]
	PINSEL7&=(~(3<<20));
	PINMODE7=(PINMODE7&(~(3<<20))) | (1<<21);
	FIO3DIR|=(1<<26);
	FIO3MASK&=(~(1<<26));
	FIO3SET|=(1<<26);
}

void Board_LED(unsigned char rgb) { //0th bit - r, 1st bit - g, 2nd bit - b
	if((rgb>>0)&1) FIO0CLR |= (1<<22);
	else FIO0SET |= (1<<22);

	if((rgb>>1)&1) FIO3CLR |= (1<<25);
	else FIO3SET |= (1<<25);

	if((rgb>>2)&1) FIO3CLR |= (1<<26);
	else FIO3SET |= (1<<26);

	//rgb = rgb^0x7; //led is negative logic, therefore we invert the bits
	//FIO0PIN = (FIO0PIN&(~(1<<22))) | (((rgb>>0)&1)<<22);
	//FIO3PIN = (FIO3PIN&(~(1<<25))) | (((rgb>>1)&1)<<25);
	//FIO3PIN = (FIO3PIN&(~(1<<26))) | (((rgb>>2)&1)<<26);
}
