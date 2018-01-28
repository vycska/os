#include "main.h"
#include "active_buzzer.h"
#include "adc.h"
#include "fifos.h"
#include "fs.h"
#include "i2c.h"
#include "led.h"
#include "os.h"
#include "os-asm.h"
#include "passive_buzzer.h"
#include "pll.h"
#include "rtc.h"
#include "systick.h"
#include "switch.h"
#include "task_adc.h"
#include "task_bme280.h"
#include "task_command_parser.h"
#include "task_ds18b20.h"
#include "task_dsm501.h"
#include "task_hd44780.h"
#include "task_idle.h"
#include "task_log.h"
#include "task_oled.h"
#include "task_switch.h"
#include "task_uart0_output.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "utils-asm.h"
#include "LPC1769.h"

extern char _data_start_lma, _data_start, _data_end, _bss_start, _bss_end;
int mtx_filesystem, mtx_i2c[3], mtx_timer[4];
long long int millis;

int main(void) {
   int i;

   DisableInterrupts(); //interrupts will be enabled in OS_Start()
   PCONP = 0;                   //gate off clock for peripherals
   PLL0_Init();
   PCLKSEL0 = 0;                //select clock for peripherals to be CCLK/4
   PCLKSEL1 = 0;

   FLASHCFG = (FLASHCFG & (~(0xf << 12))) | (5 << 12);  //flash accesses use 6 cpu clocks ["safe" setting]

   UART0_Init(115200, 8, 1, 0);
   Board_LED_Init();
   I2C0_Init();
   I2C1_Init();

   Timer1_Init();
   Timer2_Init();
   Timer3_Init();

   ADC_Init();
   RTC_Init();

   Active_Buzzer_Init();
   Passive_Buzzer_Init();
   
   Fifo_HD44780_Init();
   Fifo_Uart0_Init();
   Fifo_Command_Parser_Init();

   OS_InitSemaphore(&mtx_filesystem, 1);
   for(i = 0; i < 3; i++)
      OS_InitSemaphore(&mtx_i2c[i], 1);
   for(i = 0; i < 4; i++)
      OS_InitSemaphore(&mtx_timer[i], 1);

   fs_mount();
   if(fs_checkdisk() == STATUS_ERROR)
      fs_format();

   OS_Init(NUMTHREADS,
         "switch", 3, &Task_Switch,
         "uart0_output", 5, &Task_Uart0_Output,
         "hd44780", 6, &Task_HD44780,
         "command_parser", 7, &Task_Command_Parser,
         "bme280", 10, &Task_BME280,
         "adc", 15, &Task_ADC,
         "dsm501", 15, &Task_DSM501,
         "log", 11, &Task_Log,
         "ds18b20", 8, &Task_DS18B20,
         "oled", 12, &Task_Oled,
         "idle", 31, &Task_Idle);
   OS_Start();
   while(1);
}

void init(void) {
   char *dst, *src;

   //copy data to ram
   for(src = &_data_start_lma, dst = &_data_start; dst < &_data_end; src++, dst++)
      *dst = *src;
   //zero bss
   for(dst = &_bss_start; dst < &_bss_end; dst++)
      *dst = 0;
}
