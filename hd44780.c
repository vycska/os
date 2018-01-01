#include "hd44780.h"
#include "timer.h"
#include "LPC1769.h"

//address: 0x00-0x07 [the same are 0x08-0x0f]
//pattern: as the font is 5x8, 40 bits are used [0-39], LSB is upper line
void HD44780_NewChar(unsigned char address, unsigned long long int pattern) {
   char i;

   for(i = 0; i <= 7; i++) {    //i yra eilute [nuo virsaus]
      HD44780_SetCGRAM((address & 0x07) << 3 | i);
      HD44780_DataWrite((pattern >> (i * 5)) & 0x1f);
   }
}

void HD44780_InitPins(void) {
   //P0.5 - RS
   PINSEL0 &= (~(3 << 10));
   PINMODE0 = (PINMODE0 & (~(3 << 10))) | (1 << 11);
   FIO0DIR |= (1 << 5);
   FIO0MASK &= (~(1 << 5));
   FIO0CLR |= (1 << 5);
   //P0.10 - RW
   PINSEL0 &= (~(3 << 20));
   PINMODE0 = (PINMODE0 & (~(3 << 20))) | (1 << 21);
   FIO0DIR |= (1 << 10);
   FIO0MASK &= (~(1 << 10));
   FIO0CLR |= (1 << 10);
   //P0.11 - E
   PINSEL0 &= (~(3 << 22));
   PINMODE0 = (PINMODE0 & (~(3 << 22))) | (1 << 23);
   FIO0DIR |= (1 << 11);
   FIO0MASK &= (~(1 << 11));
   FIO0CLR |= (1 << 11);
   //P2.0-P2.7 - D0-D7
   PINSEL4 &= (~(0xffff << 0));
   PINMODE4 = (PINMODE4 & (~(0xffff << 0))) | (0xaaaa << 0);
   FIO2DIR |= (0xff << 0);
   FIO2MASK &= (~(0xff << 0));
   FIO2CLR |= (0xff << 0);
}

void HD44780_Init(void) {
   HD44780_InitPins();

   //wake up
   Timer0_Delay_NoInt(50*1000*1000);   //50ms
   HD44780_CommandWrite(0x30, 5000);
   HD44780_CommandWrite(0x30, 200);
   HD44780_CommandWrite(0x30, 50);

   //initialize
   HD44780_FunctionSet(0, 1, 1);        //5x8 font, 2 lines, 8 data bits
   HD44780_DisplayControl(0, 0, 1);     //no blink, no cursor, display on
   HD44780_ClearDisplay();      //display clear
   HD44780_EntryModeSet(0, 1);  //no shift, increment

}

void HD44780_FunctionSet(char font, char lines, char data_length) {
   unsigned char d = 1 << 5;

   d |= ((font & 1) << 2);
   d |= ((lines & 1) << 3);
   d |= ((data_length & 1) << 4);
   HD44780_CommandWrite(d, 50);
}

void HD44780_ClearDisplay(void) {
   unsigned char d = 1 << 0;

   HD44780_CommandWrite(d, 2000);
}

void HD44780_ReturnHome(void) {
   unsigned char d = 1 << 1;

   HD44780_CommandWrite(d, 2000);
}

void HD44780_EntryModeSet(char shift, char increment) {
   unsigned char d = 1 << 2;

   d |= ((shift & 1) << 0);
   d |= ((increment & 1) << 1);
   HD44780_CommandWrite(d, 50);
}

void HD44780_DisplayControl(char blink, char cursor, char display) {
   unsigned char d = 1 << 3;

   d |= ((blink & 1) << 0);
   d |= ((cursor & 1) << 1);
   d |= ((display & 1) << 2);
   HD44780_CommandWrite(d, 50);
}

void HD44780_Shift(char dir_display, char dir_cursor) {
   unsigned char d = 1 << 4;

   d |= ((dir_display & 1) << 2);
   d |= ((dir_cursor & 1) << 3);
   HD44780_CommandWrite(d, 50);
}

void HD44780_SetCGRAM(unsigned char address) {
   unsigned char d = 1 << 6;

   d |= ((address & 0x3f) << 0);
   HD44780_CommandWrite(d, 50);
}

void HD44780_SetDDRAM(unsigned char address) {
   unsigned char d = 1 << 7;

   d |= ((address & 0x7f) << 0);
   HD44780_CommandWrite(d, 50);
}
void HD44780_CommandWrite(unsigned char d,int  delay) {
   FIO0CLR |= (1 << 5);         //RS low - access CR
   FIO0CLR |= (1 << 10);        //RW low - write
   FIO2PIN = (FIO2PIN & (~0xff)) | d;
   FIO0SET |= (1 << 11);        //E high - start
   Timer0_Delay_NoInt(10*1000);
   FIO0CLR |= (1 << 11);        //E low - finish transaction
   Timer0_Delay_NoInt(delay*1000);
}

void HD44780_DataWrite(unsigned char d) {
   FIO0SET |= (1 << 5);         //RS high -- access DR
   FIO0CLR |= (1 << 10);        //RW low - write
   FIO2PIN = (FIO2PIN & (~0xff)) | d;
   FIO0SET |= (1 << 11);        //E high - start
   Timer0_Delay_NoInt(10*1000);
   FIO0CLR |= (1 << 11);        //E low - finish transaction
   Timer0_Delay_NoInt(50*1000);
}

void HD44780_DisplayString(char *s, int l, int line, int pos) { //lines: 0 - 0x00, 1 - 0x40, 2 - 0x14, 3 - 0x54
   int i;

   HD44780_SetDDRAM((line & 1 ? 1 : 0) * 0x40 + (line & 2 ? 1 : 0) * 0x14 + pos);
   for(i = 0; i < l; i++)
      HD44780_DataWrite(s[i]);
}

void HD44780_ClearLine(int line) {
   HD44780_DisplayString("                    ", 20, line, 0);
}
