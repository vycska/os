#include "i2c.h"
#include "main.h"
#include "os.h"
#include "utils.h"
#include "LPC1769.h"
#include <stdlib.h>

extern int mtx_i2c[3];

struct I2C i2c[3];

void I2C0_Init(void) {
	PCONP |= (1<<7); //power
	PCLKSEL0 = (PCLKSEL0&(~(3<<14))) | (1<<14); //peripheral clock = cclk

	PINSEL1 = (PINSEL1 & (~(3<<22))) | (1<<22); //P0.27 SDA0
	PINSEL1 = (PINSEL1 & (~(3<<24))) | (1<<24); //P0.28 SCL0
	
	I2CPADCFG=0;

	I2C0SCLL = CLOCK*10/2; //rate=100kHz [standard]
	I2C0SCLH = CLOCK*10/2;

	I2C0CONCLR = (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6); //clear AA, SI, STOP, STA
	I2C0CONSET = (1<<6); //set I2EN
	
	IPR2 = (IPR2&(~(0x1f<<19))) | (5<<19);
	ISER0|=(1<<10); //enable interrupt
}

void I2C1_Init(void) {
	PCONP |= (1<<19); //power
	PCLKSEL1 = (PCLKSEL1&(~(3<<6))) | (1<<6); //peripheral clock = cclk

	PINSEL1 |= (3<<6); //P0.19 SDA1
	PINSEL1 |= (3<<8); //P0.20 SCL1
	PINMODE1 = (PINMODE1&(~(3<<6))) | (2<<6);
	PINMODE1 = (PINMODE1&(~(3<<8))) | (2<<8);
	PINMODE_OD0 |= ((1<<19) | (1<<20));

	I2C1SCLL = CLOCK*10/2; //rate=100kHz [standard]
	I2C1SCLH = CLOCK*10/2;

	I2C1CONCLR = (1<<2) | (1<<3) | (1<<5); //clear AA, SI, STA
	I2C1CONSET = (1<<6); //set I2EN

	IPR2 = (IPR2&(~(0x1fLLU<<27))) | (5<<27);
	ISER0|=(1<<11); //enable interrupt
}

void I2C_Handler(int k,struct I2C *i2c) {
	switch(k==0?(I2C0STAT&0xf8):(k==1?(I2C1STAT&0xf8):(I2C2STAT&0xf8))) {
			case 0xf8: {
				break;
			}
			/* ***** general master states ***** */
			case 0x08: { //START has been transmitted
				if(k==0) {
					I2C0DAT=(unsigned char)((i2c->slave<<1) | ((i2c->direction==1?1:0)<<0)); //write slave address and direction [0 - write, 1 - read, 2 - write+read]
					I2C0CONCLR = (1<<5); //clear START
				}
				else if(k==1) {
					I2C1DAT=(unsigned char)((i2c->slave<<1) | ((i2c->direction==1?1:0)<<0));
					I2C1CONCLR = (1<<5); //clear START
				}
				else {
					I2C2DAT=(unsigned char)((i2c->slave<<1) | ((i2c->direction==1?1:0)<<0));
					I2C2CONCLR = (1<<5); //clear START
				}
				break;
			}
			case 0x10: { //repeated START has been transmitted
				if(k==0) {
					I2C0DAT = (i2c->slave<<1) | (1<<0); //write slave address and direction 1 [read]
					I2C0CONCLR = (1<<5); //clear START
				}
				else if(k==1) {
					I2C1DAT = (i2c->slave<<1) | (1<<0);
					I2C1CONCLR = (1<<5); //clear START
				}
				else {
					I2C2DAT = (i2c->slave<<1) | (1<<0);
					I2C2CONCLR = (1<<5); //clear START
				}
				break;
			}
			/* ***** master transmitter states ***** */
			case 0x18: { //SLA+W has been transmitted and ACK received
				if(i2c->current < i2c->length[i2c->active_buffer]) {
					if(k==0)
						I2C0DAT = i2c->buffer[i2c->active_buffer][i2c->current];
					else if(k==1)
						I2C1DAT = i2c->buffer[i2c->active_buffer][i2c->current];
					else
						I2C2DAT = i2c->buffer[i2c->active_buffer][i2c->current];
					i2c->current+=1;
				}
				else {
					if(k==0)
						I2C0CONSET = (1<<4); //set STOP
					else if(k==1)
						I2C1CONSET = (1<<4);
					else
						I2C2CONSET = (1<<4);
					i2c->status = 1; //i received ACK -- everything ok
				}
				break;
			}
			case 0x20: { //slave address+write transmitted, NACK received
				if(k==0)
					I2C0CONSET = (1<<4); //set STOP
				else if(k==1)
					I2C1CONSET = (1<<4);
				else
					I2C2CONSET = (1<<4);
				i2c->status=-1; //not what i expected
				break;
			}
			case 0x28: { //data byte transmitted and ACK received
				if(i2c->current < i2c->length[i2c->active_buffer]) { //dabartiniame buferyje yra neissiustu duomenu
					if(k==0)
						I2C0DAT = i2c->buffer[i2c->active_buffer][i2c->current];
					else if(k==1)
						I2C1DAT = i2c->buffer[i2c->active_buffer][i2c->current];
					else
						I2C2DAT = i2c->buffer[i2c->active_buffer][i2c->current];
					i2c->current += 1;
				}
				else if(++i2c->active_buffer==1 && i2c->length[1]>0) { //dabartinis buferis isnaudotas, ziuriu sekanti
					if(i2c->direction==2) { //write baigtas, toliau seks read
						if(k==0)
							I2C0CONSET = (1<<5); //set START
						else if(k==1)
							I2C1CONSET = (1<<5);
						else
							I2C2CONSET = (1<<5);
						i2c->current = 0;
					}
					else { //issiunciamas sekancio buferio pirmas baitas
						if(k==0)
							I2C0DAT = i2c->buffer[1][0];
						else if(k==1)
							I2C1DAT = i2c->buffer[1][0];
						else
							I2C2DAT = i2c->buffer[1][0];
						i2c->current=1;
					}
				}
				else { //sekancio buferio nera arba jis tuscias
					if(k==0)
						I2C0CONSET = (1<<4); //set STOP
					else if(k==1)
						I2C1CONSET = (1<<4);
					else if(k==2)
						I2C2CONSET = (1<<4);
					i2c->status=1; //viska issiunciau ka norejau
				}
				break;
			}
			case 0x30: { //data transmitted, NACK received
				if(k==0)
					I2C0CONSET = (1<<4); //set STOP
				else if(k==1)
					I2C1CONSET = (1<<4);
				else
					I2C2CONSET = (1<<4);
				i2c->status = (i2c->current==i2c->length[i2c->active_buffer]?1:-1); //ziurau ar visus baitus issiunciau
				break;
			}
			/* ***** master receiver states ***** */
			case 0x40: { //SLA+R has been transmitted and ACK received
				if(i2c->length[i2c->active_buffer]==0) { //neprasoma jokiu duomenu gavimo
					if(k==0)
						I2C0CONSET = (1<<4); //set STOP
					else if(k==1)
						I2C1CONSET = (1<<4);
					else
						I2C2CONSET = (1<<4);
					i2c->status=1;
				}
				else if(i2c->length[i2c->active_buffer]==1) { //sekanti karta bus gautas vienintelis ir paskutinis baitas
					if(k==0)
						I2C0CONCLR = (1<<2); //clear AA
					else if(k==1)
						I2C1CONCLR = (1<<2);
					else
						I2C2CONCLR = (1<<2);
				}
				else { //bus gauta daugiau nei vienas baitas, todel is pradziu siusime AA
					if(k==0)
						I2C0CONSET = (1<<2); //set AA
					else if(k==1)
						I2C1CONSET = (1<<2);
					else
						I2C2CONSET = (1<<2);
				}
				break;
			}
			case 0x48: { //slave address+read transmitted, NACK received
				if(k==0)
					I2C0CONSET = (1<<4); //set STOP
				else if(k==1)
					I2C1CONSET = (1<<4);
				else
					I2C2CONSET = (1<<4);
				i2c->status=-1; //gal as ir nenorejau nieko nuskaityti, bet kadangi gavau NACK, laikau kad tai neigiamas dalykas
				break;
			}
			case 0x50: { //data received, ACK returned
				i2c->buffer[i2c->active_buffer][i2c->current] = (k==0?I2C0DAT:(k==1?I2C1DAT:I2C2DAT));
				i2c->current+=1;
				if(i2c->current == i2c->length[i2c->active_buffer]-1) {//sekanti karta bus gautas paskutinis baitas
					if(k==0)
						I2C0CONCLR = (1<<2); //clear AA
					else if(k==1)
						I2C1CONCLR = (1<<2);
					else
						I2C2CONCLR = (1<<2);
				}
				else {
					if(k==0)
						I2C0CONSET = (1<<2); //set AA
					else if(k==1)
						I2C1CONSET = (1<<2);
					else
						I2C2CONSET = (1<<2);
				}
				break;
			}
			case 0x58: { //data received, NACK returned [by me]
				i2c->buffer[i2c->active_buffer][i2c->current] = (k==0?I2C0DAT:(k==1?I2C1DAT:I2C2DAT));
				if(k==0)
					I2C0CONSET = (1<<4); //set STOP
				else if(k==1)
					I2C1CONSET = (1<<4);
				else
					I2C2CONSET = (1<<4);
				i2c->status=1;
				break;
			}
			default: {
				if(k==0) {
					I2C0CONCLR = (1<<2); //clear AA
					I2C0CONSET = (1<<4); //set STOP
				}
				else if(k==1) {
					I2C1CONCLR = (1<<2);
					I2C1CONSET = (1<<4);
				}
				else {
					I2C2CONCLR = (1<<2);
					I2C2CONSET = (1<<4);
				}
				i2c->status=-2;
				break;
			}
	}
	if(k==0)
		I2C0CONCLR = (1<<3); //clear SI
	else if(k==1)
		I2C1CONCLR = (1<<3);
	else
		I2C2CONCLR = (1<<3);
}

void I2C_Start(int k) {
	if(k==0) {
		ICPR0 |= (1<<10); //clear pending for I2C0
		I2C0CONCLR = (1<<2) | (1<<3) | (1<<4) | (1<<5); //clear AA, SI, STOP, STA
		I2C0CONSET = (1<<5); //set START
	}
	else if(k==1) {
		ICPR0 |= (1<<11); //clear pending for I2C1
		I2C1CONCLR = (1<<2) | (1<<3) | (1<<4) | (1<<5); //clear AA, SI, STOP, STA
		I2C1CONSET = (1<<5); //set START
	}
	else {
		ICPR0 |= (1<<12); //clear pending for I2C2
		I2C2CONCLR = (1<<2) | (1<<3) | (1<<4) | (1<<5); //clear AA, SI, STOP, STA
		I2C2CONSET = (1<<5); //set START
	}
}

int I2C_TestSlave(int k,unsigned char slave) {
	if(k<0 || k>2) k=0;
	OS_Spinning_Wait(&mtx_i2c[k]);
	i2c[k].slave=slave;
	i2c[k].active_buffer=0;
	i2c[k].direction=1;
	i2c[k].current=0;
	i2c[k].status=0;
	i2c[k].buffer[0]=0;
	i2c[k].length[0]=0;
	i2c[k].buffer[1]=0;
	i2c[k].length[1]=0;
	I2C_Start(k);
	while(i2c[k].status==0 || ((k==0?I2C0CONSET:(k==1?I2C1CONSET:I2C2CONSET))&((1<<3) | (1<<4) | (1<<5)))!=0);
	OS_Spinning_Signal(&mtx_i2c[k]);
	return i2c[k].status;	
}

void I2C0_IRQHandler(void) {
	I2C_Handler(0,&i2c[0]);
}

void I2C1_IRQHandler(void) {
	I2C_Handler(1,&i2c[1]);
}
