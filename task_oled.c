#include "task_oled.h"
#include "fifos.h"
#include "os.h"
#include "task_bme280.h"
#include "task_ds18b20.h"
#include "timer.h"
#include "u8g2.h"
#include "utils.h"
#include "LPC1769.h"

uint8_t u8x8_gpio_and_delay(u8x8_t*,uint8_t,uint8_t,void*);

extern int mtx_timer[4];
extern struct Task_BME280_Data task_bme280_data;
extern struct Task_DS18B20_Data task_ds18b20_data;

struct Task_Oled_Data task_oled_data;
u8g2_t u8g2; //a structure which contains all the data for one display

void Task_Oled(void) {
   char d,buf[64];
   double v;

   Fifo_Uart0_Put("Task_Oled has started",0);
   OS_Blocking_Wait(&mtx_timer[0]);
   u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R1,u8x8_byte_sw_i2c,u8x8_gpio_and_delay); //init u8g2 structure
   u8g2_InitDisplay(&u8g2); //send init sequence to the display, display is in sleep mode
   u8g2_SetPowerSave(&u8g2,0); //wake up display
   OS_Blocking_Signal(&mtx_timer[0]);
   while(1) {
      OS_Blocking_Wait(&mtx_timer[0]);

      u8g2_ClearBuffer(&u8g2);

      u8g2_DrawLine(&u8g2,0,0,63,0);
      u8g2_DrawLine(&u8g2,0,127,63,127);
      u8g2_DrawLine(&u8g2,0,0,0,127);
      u8g2_DrawLine(&u8g2,63,0,63,127);
      
      u8g2_DrawCircle(&u8g2,32,96,31,U8G2_DRAW_ALL);

      u8g2_SetFont(&u8g2,u8g2_font_6x10_tf);
      mysprintf(buf,"%u-%u-%u",YEAR,MONTH,DOM);
      u8g2_DrawStr(&u8g2,4,14,buf);
      mysprintf(buf,"%u:%u:%u",HOUR,MIN,SEC);
      u8g2_DrawStr(&u8g2,4,26,buf);

      u8g2_DrawLine(&u8g2,0,30,63,30);

      u8g2_SetFont(&u8g2,u8g2_font_8x13B_tf);
      switch(task_oled_data.k) {
         case 0:
            v=task_bme280_data.t/10.0;
            d=0;
            break;
         case 1:
            v=task_bme280_data.h/10.0;
            d=1;
            break;
         case 2:
            v=task_bme280_data.p/10.0;
            d=2;
            break;
         case 3:
            v=task_ds18b20_data.t[0]/10.0;
            d=0;
            break;
         case 4:
            v=task_ds18b20_data.t[1]/10.0;
            d=0;
            break;
         case 5:
            v=task_ds18b20_data.t[2]/10.0;
            d=0;
            break;
         default:
            v=d=0;
      }
      if(v>-50)
         mysprintf(buf,"%f1%s",(char*)&v,d==0?"\xb0""C":(d==1?" %":" mm"));
      else mysprintf(buf,"--.-");
      u8g2_DrawStr(&u8g2,8,101,buf);

      u8g2_SendBuffer(&u8g2);

      OS_Blocking_Signal(&mtx_timer[0]);
      OS_Sleep(1000);
   }
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8,uint8_t msg,uint8_t arg_int,void *arg_ptr) {
   switch(msg) {
      case U8X8_MSG_GPIO_AND_DELAY_INIT: //called once during init phase of u8g2; can be used to setup pins
         PCONP |= (1<<15); //GPIO power
         PINSEL3 &= (~(3u<<28)); //P1.30 GPIO (SDA)
         PINSEL3 &= (~(3u<<30)); //P1.31 GPIO (SCL)
         PINMODE3 = (PINMODE3&(~(3u<<28))) | (2<<28); //neither pull-up, not pull-down
         PINMODE3 = (PINMODE3&(~(3u<<30))) | (2<<30); //neither pull-up, not pull-down
         PINMODE_OD1 |= (1u<<30); //open drain mode
         PINMODE_OD1 |= (1u<<31); //open drain mode
         FIO1DIR |= (1u<<30); //pin is an output
         FIO1DIR |= (1u<<31); //pin is an output
         FIO1MASK &= (~(1u<<30)); //allow access
         FIO1MASK &= (~(1u<<31)); //allow access
         FIO1SET |= (1u<<30); //at start line is released
         FIO1SET |= (1u<<31); //at start line is released
         break;
      case U8X8_MSG_DELAY_NANO: //delay arg_int*1 nano second
         Timer0_Delay_NoInt(arg_int);
         break;
      case U8X8_MSG_DELAY_100NANO: //delay arg_int*100 nano seconds
         Timer0_Delay_NoInt(arg_int*100);
         break;
      case U8X8_MSG_DELAY_10MICRO: //delay arg_int * 10 micro seconds
         Timer0_Delay_NoInt(arg_int*10*1000);
         break;
      case U8X8_MSG_DELAY_MILLI: //delay arg_int*1 milli second
         Timer0_Delay_NoInt(arg_int*1000*1000);
         break;
      case U8X8_MSG_DELAY_I2C: //arg_int is the I2C speed in 100 kHz, e.g. 4=400 Hz; arg_int=1: delay by 5 us, arg_int=4: delay by 1.25 us
         Timer0_Delay_NoInt(5.0/arg_int*1000);
         break;
      case U8X8_MSG_GPIO_I2C_CLOCK: //arg_int=0: output low at I2C clock pin; arg_int=1: input dir with pullup high for I2C clock pin
         if(arg_int==0) FIO1CLR |= (1u<<31);
         else FIO1SET |= (1u<<31);
         break;
      case U8X8_MSG_GPIO_I2C_DATA: //arg_int=0: output low at I2C data pin; arg_int=1: input dir with pullup high for I2C data pin
         if(arg_int==0) FIO1CLR |= (1u<<30);
         else FIO1SET |= (1u<<30);
         break;
      default:
         u8x8_SetGPIOResult(u8x8,1); //default return value
         break;
   }
   return 1;
}
