#include "pll.h"
#include "main.h"
#include "utils.h"

#include "LPC1769.h"
#include <math.h>

void PLL0_Init() {
	int g,d,m,n,fcco;
	
	//calculate M and N values
	fcco=CLOCK * (int)ceil(275.0/CLOCK); //FCCO is a close multiple of CCLK
	m=fcco;
	n=2*XTAL;
	g=gcd(m,n);
	m/=g;
	n/=g;
	d=fcco/CLOCK;

	PLL0CON = (PLL0CON & (~(1<<1))) | (0<<1); //disconnect PLL0 with feed sequence
	PLL0_Feed();

	PLL0CON = (PLL0CON & (~(1<<0))) | (0<<0); //disable PLL0 with feed sequence
	PLL0_Feed();

	SCS = (SCS & (~(1<<4))) | (0<<4); //select main oscillator range [1-20MHz]
	SCS = (SCS & (~(1<<5))) | (1<<5); //enable main oscillator
	while((SCS&(1<<6))==0); //wait for positive main oscillator status

	CLKSRCSEL = (CLKSRCSEL & (~(3<<0))) | (1<<0); //select clock source for the PLL0 [main oscillator]

	//write M and N values with feed sequence
	PLL0CFG = (PLL0CFG & (~(0x7fff<<0))) | ((m-1)<<0);
	PLL0CFG = (PLL0CFG & (~(0xff<<16))) | ((n-1)<<16);
	PLL0_Feed();

	PLL0CON = (PLL0CON & (~(1<<0))) | (1<<0); //enable PLL0 with feed sequence
	PLL0_Feed();

	CCLKCFG = (CCLKCFG & (~(0xff<<0))) | ((d-1)&0xff); //divide PLL0 output to obtain CCLK
	USBCLKCFG = (USBCLKCFG & (~(0xf<<0))) | ((fcco/48)&0xf);

	while((PLL0STAT&(1<<26))==0); //wait for the lock of PLL0

	PLL0CON = (PLL0CON & (~(1<<1))) | (1<<1); //connect PLL0 with feed sequence
	PLL0_Feed();
}

void PLL0_Feed(void) {
	PLL0FEED = (PLL0FEED & (~(0xff<<0))) | (0xaa<<0);
	PLL0FEED = (PLL0FEED & (~(0xff<<0))) | (0x55<<0);
}
