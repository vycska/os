#include "adc.h"
#include "main.h"
#include "LPC1769.h"

void ADC_Init(void) {
	PCONP |= (1<<12); //power
	PCLKSEL0 = (PCLKSEL0&(~(0x3<<24))) | (1<<24); //pclk = cclk
	PINSEL1 = (PINSEL1&(~(0x3<<14))) | (1<<14); //P0.23 is AD0.0
	PINMODE1 = (PINMODE1&(~(0x3<<14))) | (2<<14); //neither pull-up, nor pull-down
	
	AD0CR = (AD0CR&(~((0xff<<0) | (0xff<<8) | (1<<16) | (1<<21) | (7<<24)))) | (1<<0) | ((CLOCK/13+1)<<8); //select AD0.0, set divider for clock, software controlled, converter is in power-down, no-start

	AD0CR |= (1<<21); //converter is operational [but not started]
}
