#ifndef MAIN_H
#define MAIN_H

#define XTAL 12                 //MHz
#define CLOCK 100               //MHz

void Fifo_Command_Parser_Init(void);
void Fifo_Command_Parser_Put(char *);
void Fifo_Command_Parser_Get(char **);

#endif
