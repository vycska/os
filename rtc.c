#include "rtc.h"
#include "LPC1769.h"

void RTC_Init(void) {
	PCONP |= (1<<9); //power
	
	CCR = (CCR&(~((1<<0) | (1<<1) | (3<<2) | (1<<4)))) | (1<<4); //time counters disabled, calibration counter disabled
	ILR = (ILR&(~((1<<0) | (1<<1)))) | (1<<0); //CIIR interrupts enabled, AMR not
	CIIR = (CIIR&(~(0xff<<0))) | (1<<0); //seconds counter interrupt
	
	IPR4 = (IPR4&(~(0x1f<<11))) | (10<11);
	ISER0 |= (1<<17); //interrupt enable
	
	CCR |= (1<<0); //time counters enabled
}

void RTC_IRQHandler(void) {
	if(ILR&(1<<0)) { //CIIR interrupt
		ILR |= (1<<0); //clear interrupt
		GPREG0+=1;
	}
	if(ILR&(1<<1)) { //AMR interrupt
		ILR |= (1<<1); //clear interrupt
	}
}
