#include "bme280.h"
#include "fifos.h"
#include "i2c.h"
#include "os.h"
#include "utils.h"
#include "LPC1769.h"

extern int mtx_i2c[3];
extern struct I2C i2c[3];

struct BME280_Config bme280_config;

int BME280_RegisterRead(unsigned int r, unsigned char *d, int l) {
   unsigned char reg[1];

   OS_Spinning_Wait(&mtx_i2c[0]);
   i2c[0].slave = BME280_SLAVE;
   i2c[0].active_buffer = 0;
   i2c[0].direction = 2;        //write then read
   i2c[0].current = 0;
   i2c[0].status = 0;
   reg[0] = r;
   i2c[0].buffer[0] = reg;
   i2c[0].length[0] = 1;
   i2c[0].buffer[1] = d;
   i2c[0].length[1] = l;
   I2C_Start(0);
   while(i2c[0].status == 0 || (I2C0CONSET & ((1 << 3) | (1 << 4) | (1 << 5))) != 0);
   OS_Spinning_Signal(&mtx_i2c[0]);
   return i2c[0].status;
}

int BME280_RegisterWrite(unsigned int r, unsigned char *d, int l) {
   unsigned char reg[1];

   OS_Spinning_Wait(&mtx_i2c[0]);
   i2c[0].slave = BME280_SLAVE;
   i2c[0].active_buffer = 0;
   i2c[0].direction = 0;        //write
   i2c[0].current = 0;
   i2c[0].status = 0;
   reg[0] = r;
   i2c[0].buffer[0] = reg;
   i2c[0].length[0] = 1;
   i2c[0].buffer[1] = d;
   i2c[0].length[1] = l;
   I2C_Start(0);
   while(i2c[0].status == 0 || (I2C0CONSET & ((1 << 3) | (1 << 4) | (1 << 5))) != 0);
   OS_Spinning_Signal(&mtx_i2c[0]);
   return i2c[0].status;
}

void BME280_Init(void) {
   unsigned char data[2] = { 0 };

   bme280_config.os_h = bme280_config.os_p = bme280_config.os_t = 3;    // oversampling is x4 [but note value is 3]

   data[0] = bme280_config.os_h;
   BME280_RegisterWrite(0xf2, data, 1);

   data[0] = (bme280_config.os_t << 5) | (bme280_config.os_p << 2);
   BME280_RegisterWrite(0xf4, data, 1);

   BME280_RegisterRead(0x88, data, 2);
   bme280_config.dig_T1 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x8a, data, 2);
   bme280_config.dig_T2 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x8c, data, 2);
   bme280_config.dig_T3 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x8e, data, 2);
   bme280_config.dig_P1 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x90, data, 2);
   bme280_config.dig_P2 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x92, data, 2);
   bme280_config.dig_P3 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x94, data, 2);
   bme280_config.dig_P4 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x96, data, 2);
   bme280_config.dig_P5 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x98, data, 2);
   bme280_config.dig_P6 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x9a, data, 2);
   bme280_config.dig_P7 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x9c, data, 2);
   bme280_config.dig_P8 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0x9e, data, 2);
   bme280_config.dig_P9 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0xa1, data, 1);
   bme280_config.dig_H1 = data[0];

   BME280_RegisterRead(0xe1, data, 2);
   bme280_config.dig_H2 = (data[1] << 8) | data[0];

   BME280_RegisterRead(0xe3, data, 1);
   bme280_config.dig_H3 = data[0];

   BME280_RegisterRead(0xe4, data, 2);
   bme280_config.dig_H4 = (data[0] << 4) | (data[1] & 0xf);

   BME280_RegisterRead(0xe5, data, 2);
   bme280_config.dig_H5 = (data[1] << 4) | ((data[0] >> 4) & 0xf);

   BME280_RegisterRead(0xe7, data, 1);
   bme280_config.dig_H6 = data[0];
}

void BME280_StartForcedMeasurement(void) {
   unsigned char data[1];

   data[0] = (bme280_config.os_t << 5) | (bme280_config.os_p << 2) | 1;
   BME280_RegisterWrite(0xf4, data, 1);
}

void BME280_GetValue(double *h, double *p, double *t) {
   unsigned char data[8] = { 0 };

   BME280_RegisterRead(0xf7, data, 8);
   bme280_config.adc_P = (((unsigned int)data[0]) << 12) | (((unsigned int)data[1]) << 4) | (((unsigned int)data[2]) >> 4);
   bme280_config.adc_T = (((unsigned int)data[3]) << 12) | (((unsigned int)data[4]) << 4) | (((unsigned int)data[5]) >> 4);
   bme280_config.adc_H = (((unsigned int)data[6]) << 8) | ((unsigned int)data[7]);
   bme280_config.T = BME280_compensate_T_int32(bme280_config.adc_T);
   bme280_config.P = BME280_compensate_P_int64(bme280_config.adc_P);
   bme280_config.H = BME280_compensate_H_int32(bme280_config.adc_H);
   *h = (double)bme280_config.H / 1024.0;
   *p = (double)bme280_config.P / 256.0 * 0.0075006;    //1 atm [standard atmosphere] = 760 torr = 101325 Pa = 1.01325 bar; 1 mmHg = 133.322387415 Pa --> 1 Pa = 0.0075006 mmHg
   *t = (double)bme280_config.T / 100.0;
}

int BME280_GetID(unsigned char *id) {
   return BME280_RegisterRead(0xd0, id, 1);
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of 5123 equals 51.23 DegC.
int BME280_compensate_T_int32(int adc_T) {
   int var1, var2, T;

   var1 = ((((adc_T >> 3) - ((int)bme280_config.dig_T1 << 1))) * ((int)bme280_config.dig_T2)) >> 11;
   var2 = (((((adc_T >> 4) - ((int)bme280_config.dig_T1)) * ((adc_T >> 4) - ((int)bme280_config.dig_T1))) >> 12) * ((int)bme280_config.dig_T3)) >> 14;
   bme280_config.t_fine = var1 + var2;
   T = (bme280_config.t_fine * 5 + 128) >> 8;
   return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of 24674867 represents 24674867/256 = 96386.2 Pa = 963.862 hPa
unsigned int BME280_compensate_P_int64(int adc_P) {
   long long int var1, var2, p;

   var1 = ((long long int)bme280_config.t_fine) - 128000;
   var2 = var1 * var1 * (long long int)bme280_config.dig_P6;
   var2 = var2 + ((var1 * (long long int)bme280_config.dig_P5) << 17);
   var2 = var2 + (((long long int)bme280_config.dig_P4) << 35);
   var1 = ((var1 * var1 * (long long int)bme280_config.dig_P3) >> 8) + ((var1 * (long long int)bme280_config.dig_P2) << 12);
   var1 = (((((long long int)1) << 47) + var1)) * ((long long int)bme280_config.dig_P1) >> 33;

   if(var1 == 0) {
      return 0;                 // avoid exception caused by division by zero
   }

   p = 1048576 - adc_P;
   p = (((p << 31) - var2) * 3125) / var1;
   var1 = (((long long int)bme280_config.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
   var2 = (((long long int)bme280_config.dig_P8) * p) >> 19;
   p = ((p + var1 + var2) >> 8) + (((long long int)bme280_config.dig_P7) << 4);
   return (unsigned int)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of 47445 represents 47445/1024 = 46.333 %RH
unsigned int BME280_compensate_H_int32(int adc_H) {
   int v_x1_u32r;

   v_x1_u32r = (bme280_config.t_fine - ((int)76800));
   v_x1_u32r =
      (((((adc_H << 14) - (((int)bme280_config.dig_H4) << 20) - (((int)bme280_config.dig_H5) * v_x1_u32r)) + ((int)16384)) >> 15) * (((((((v_x1_u32r * ((int)bme280_config.dig_H6)) >> 10) * (((v_x1_u32r * ((int)bme280_config.dig_H3)) >> 11) + ((int)32768))) >> 10) +
               ((int)2097152)) * ((int)bme280_config.dig_H2) + 8192) >> 14));
   v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int)bme280_config.dig_H1)) >> 4));
   v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
   v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
   return (unsigned int)(v_x1_u32r >> 12);
}
