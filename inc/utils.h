#ifndef UTILS_H
#define UTILS_H

#define MIN2(a,b) (((a)<(b))?(a):(b))
#define MIN3(a,b,c) (((a)<(b))?(((c)<(a))?(c):(a)):(((c)<(b))?(c):(b)))
#define MAX2(a,b) (((a)>(b))?(a):(b))

int gcd(int,int);
unsigned short crc16(unsigned char*,unsigned char);
void streverse(char*,int);
int dec2hex2str(unsigned long long int,char*);
int dec2str(long long int,char*);
int dbl2str(double,int,char*);
int mysprintf(char*,const char*,...);
long long int power(int,int);
unsigned int utime(void);
unsigned char reflect_byte(unsigned char);

#endif
