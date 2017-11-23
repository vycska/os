#ifndef HD44780_H_
#define HD44780_H_

void HD44780_InitPins(void);
void HD44780_Init(void);
void HD44780_ClearDisplay(void);
void HD44780_ReturnHome(void);
void HD44780_EntryModeSet(char,char);
void HD44780_DisplayControl(char,char,char);
void HD44780_Shift(char,char);
void HD44780_FunctionSet(char,char,char);
void HD44780_SetCGRAM(unsigned char);
void HD44780_SetDDRAM(unsigned char);
void HD44780_CommandWrite(unsigned char,unsigned short);
void HD44780_DataWrite(unsigned char);
void HD44780_DisplayString(char*,int,int,int);
void HD44780_ClearLine(int);

#endif
