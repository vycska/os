#ifndef I2C_H_
#define I2C_H_

struct I2C {
    unsigned char *buffer[2],
		slave, //slave address
		active_buffer, //0 or 1 [metadata or data]
		direction; //0 - write, 1 - read, 2 - write and then read
    int current,length[2],status;
};

void I2C0_Init(void);
void I2C1_Init(void);
void I2C_Start(int);
int I2C_TestSlave(int,unsigned char);

#endif
