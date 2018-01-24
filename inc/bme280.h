#ifndef __BME280_H__
#define __BME280_H__

#define BME280_SLAVE (0x76)

struct BME280_Config {
   unsigned short dig_T1;       //28044
   signed short dig_T2;         //25822
   signed short dig_T3;         //50
   unsigned short dig_P1;       //37193
   signed short dig_P2;         //-10879
   signed short dig_P3;         //3024
   signed short dig_P4;         //4201
   signed short dig_P5;         //-128
   signed short dig_P6;         //-7
   signed short dig_P7;         //9900
   signed short dig_P8;         //-10230
   signed short dig_P9;         //4285
   unsigned char dig_H1;        //75
   signed short dig_H2;         //365
   unsigned char dig_H3;        //0
   signed short dig_H4;         //312
   signed short dig_H5;         //0
   signed char dig_H6;          //30

   int t_fine;                  // t_fine carries fine temperature as global value

   unsigned char os_h, os_t, os_p;
   int adc_H, adc_P, adc_T, H, P, T;
};

int BME280_RegisterRead(unsigned int, unsigned char *, int);
int BME280_RegisterWrite(unsigned int, unsigned char *, int);
int BME280_GetID(unsigned char *);
void BME280_Init(void);
void BME280_StartForcedMeasurement(void);
void BME280_GetValue(double *, double *, double *);

int BME280_compensate_T_int32(int adc_T);
unsigned int BME280_compensate_P_int64(int adc_P);
unsigned int BME280_compensate_H_int32(int adc_H);

#endif
