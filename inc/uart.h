#ifndef UART_H
#define UART_H

#define UART_IN_MAX 48

void UART0_Init(int,int,int,int);
void UART0_Transmit(char*,int);
void UART0_IRQHandler(void);
void UART_get_baudrate_config(int,int,int*,int*,int*);

struct sUART_In {
	char s[UART_IN_MAX];
	int i;
};

#endif
