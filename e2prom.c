#include "e2prom.h"
#include "i2c.h"
#include "os.h"
#include "LPC1769.h"

extern int mtx_i2c[3];
extern struct I2C i2c[3];

//address: 0-1fff [8192 bytes], l - length of bytes in buffer and count of bytes to be read
int Board_E2PROM_ReadBytes(unsigned int address,unsigned char *buffer_data,unsigned short l) {
	unsigned char buffer_address[2];
	OS_Spinning_Wait(&mtx_i2c[1]);
	i2c[1].slave=E2PROM_SLAVE;
	i2c[1].active_buffer=0;
	i2c[1].direction=2;
	i2c[1].current=0;
	i2c[1].status=0;
	buffer_address[0]=((address>>8)&0x1f);
	buffer_address[1]=(address&0xff);
	i2c[1].buffer[0]=buffer_address;
	i2c[1].length[0]=2;
	i2c[1].buffer[1]=buffer_data;
	i2c[1].length[1]=l;
	I2C_Start(1);
	while(i2c[1].status==0 || (I2C1CONSET&((1<<3) | (1<<4) | (1<<5)))!=0);
	OS_Spinning_Signal(&mtx_i2c[1]);
	return i2c[1].status;
}
int Board_E2PROM_WriteByte(unsigned int address,unsigned char data) {
	unsigned char buffer_address[2],buffer_data[1];
	OS_Spinning_Wait(&mtx_i2c[1]);
	i2c[1].slave=E2PROM_SLAVE;
	i2c[1].active_buffer=0;
	i2c[1].direction=0;
	i2c[1].current=0;
	i2c[1].status=0;
	buffer_address[0]=((address>>8)&0x1f);
	buffer_address[1]=(address&0xff);
	i2c[1].buffer[0]=buffer_address;
	i2c[1].length[0]=2;
	buffer_data[0]=data;
	i2c[1].buffer[1]=buffer_data;
	i2c[1].length[1]=1;
	I2C_Start(1);
	while(i2c[1].status==0 || (I2C1CONSET&((1<<3) | (1<<4) | (1<<5)))!=0);
	OS_Spinning_Signal(&mtx_i2c[1]);
	return i2c[1].status;
}
// p - page [0-255], i - index of start byte [0-31], l - length of data [0-(32-i)]
int Board_E2PROM_WritePage(unsigned char p,unsigned char i,unsigned char *data,unsigned char l) {
	unsigned char buffer_address[2];
	OS_Spinning_Wait(&mtx_i2c[1]);
	i2c[1].slave=E2PROM_SLAVE;
	i2c[1].active_buffer=0;
	i2c[1].direction=0; //W
	i2c[1].current=0;
	i2c[1].status=0;
	buffer_address[0]=(p>>3)&0x1f;
	buffer_address[1]=((p&0x7)<<5) | (i&0x1f);
	i2c[1].buffer[0]=buffer_address;
	i2c[1].length[0]=2;
	i2c[1].buffer[1]=data;
	i2c[1].length[1]=l;
	I2C_Start(1);
	while(i2c[1].status==0 || (I2C1CONSET&((1<<3) | (1<<4) | (1<<5)))!=0);
	OS_Spinning_Signal(&mtx_i2c[1]);
	return i2c[1].status;
}
	

