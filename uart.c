#include "uart.h"
#include "main.h"
#include "os.h"
#include "utils.h"
#include "utils-asm.h"
#include "LPC1769.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct sUART_In uart0_in;

//databits: 5/6/7/8
//stopbits: 1/2
//parity: 0 - no, 1 - odd, 2 - even
void UART0_Init(int baudrate,int databits,int stopbits,int parity) {
	int dl,div,mul;

	PCONP|=(1<<3); //power
	PCLKSEL0 = (PCLKSEL0&(~(3<<6))) | (1<<6); //pclk=CCLK

	U0LCR|=(1<<7); //enable divisor latches access

	UART_get_baudrate_config(CLOCK,baudrate,&dl,&div,&mul);
	U0DLL=(U0DLL&(~(0xff))) | (dl&0xff);
	U0DLM=(U0DLM&(~(0xff))) | ((dl>>8)&0xff);
	U0FDR=(U0FDR&(~(0xff))) | ((div&0xf)<<0) | ((mul&0xf)<<4);

	U0LCR&=(~(1<<7)); //disable divisor latches access

	U0LCR=(U0LCR&(~(0x7f))) | (((databits-5)&3)<<0) | (((stopbits-1)&1)<<2) | ((parity>0?1:0)<<3) | (((parity-1)&1)<<4); //break transmission disabled

	U0FCR=(U0FCR&(~(3<<6))) | ((1<<0) | (1<<1) | (1<<2)); //RX trigger 1 character, FIFO enable, FIFOs reset

	// pins P0.2 [TXD0], P0.3 [RXD0]
	PINSEL0=(PINSEL0&(~(0xf<<4))) | ((1<<4) | (1<<6));
	PINMODE0=(PINMODE0&(~(0xf<<4))) | ((2<<4)+(2<<6)); //neither pur nor pdr

	U0IER=(1<<0); //enable RDA interrupt

	IPR1=(IPR0&(~(0x1f<<11)))+(5<<11); //interrupt priority
	ISER0|=(1<<5); //interrupt enable

	U0TER|=(1<<7); //enable transmitter
}

void UART0_Transmit(char *s,int k) {
	int i;
	for(i=0;i<k+2;i++) { //+2 nes gale pridesim \r\n
		if(i%16==0) while((U0LSR&(1<<5))==0); //wait for FIFO to become empty
		U0THR=(i==k?'\r':(i==k+1?'\n':s[i]));
	}
 }

void UART0_IRQHandler(void) {
	unsigned char c;
	switch((U0IIR>>1)&7) { //interrupt identification
		case 3: //011; 1 - receive line status
			break;
		case 2: //010; 2a - receive data available
			c=U0RBR;
			if(isprint(c)) {
				uart0_in.s[uart0_in.i]=c;
				uart0_in.i=(uart0_in.i+1)%UART_IN_MAX;
			}
			else if(uart0_in.i!=0){
				//send received command to the FIFO buffer
				uart0_in.s[uart0_in.i]=0;
				Fifo_Command_Parser_Put(uart0_in.s);
				uart0_in.i=0;
			}
			break;
		case 6: //110; 2b - character time-out indicator
			break;
		case 1: //001; 3 - THRE
			break;
	}
}

void UART_get_baudrate_config(int pclk,int baudrate,int *dl,int *div,int *mul) {
	int i,j,flag;
	double dl2,minf;

	for(flag=1,minf=0,i=0;i<=14;i++)
		for(j=i+1;j<=15;j++) {
			dl2=1000000*pclk/16.0/baudrate*j/(j+i);
			if(flag || (dl2-(int)dl2)<minf) {
				flag=0;
				minf=dl2-(int)dl2;
				*div=i;
				*mul=j;
				*dl=dl2;
			}
		}
}
