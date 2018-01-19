#include "task_bme280.h"
#include "bme280.h"
#include "fifos.h"
#include "fs.h"
#include "hd44780.h"
#include "os.h"
#include "utils.h"
#include <string.h>

extern struct BME280_Config bme280_config;

struct Task_BME280_Data task_bme280_data;

void Task_BME280(void) {
   char s[24];
   double h, p, t;

   Fifo_Uart0_Put("Task_BME280 has started", 0);

   OS_Sleep(2);                 //max time for first communication

   BME280_Init();

   while(1) {
      BME280_StartForcedMeasurement();
      OS_Sleep(1.25 + 2.5 * power(2, bme280_config.os_t - 1) + 2.3 * power(2, bme280_config.os_p - 1) + 0.575 + 2.3 * power(2, bme280_config.os_h - 1) + 0.575);
      BME280_GetValue(&h, &p, &t);

      //HD44780_ClearLine(0);
      //HD44780_ClearLine(1);
      //HD44780_ClearLine(2);

      mysprintf(s, "T: %f1 C", (char *)&t);
      Fifo_HD44780_Put(s, 0, 0);
      mysprintf(s, "P: %f1 mmHg", (char *)&p);
      Fifo_HD44780_Put(s, 1, 0);
      mysprintf(s, "H: %f1 %%", (char *)&h);
      Fifo_HD44780_Put(s, 2, 0);

      task_bme280_data.h = h * 10;
      task_bme280_data.p = p * 10;
      task_bme280_data.t = t * 10;

      OS_Sleep(2000);
   }
}
