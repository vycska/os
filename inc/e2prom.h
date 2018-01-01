#ifndef BOARD_E2PROM_H
#define BOARD_E2PROM_H

#define E2PROM_SLAVE 0x50

int Board_E2PROM_ReadBytes(unsigned int, unsigned char *, unsigned short);
int Board_E2PROM_WriteByte(unsigned int, unsigned char);
int Board_E2PROM_WritePage(unsigned char, unsigned char, unsigned char *, unsigned char);

#endif
