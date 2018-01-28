#include "task_dsm501.h"
#include "fifos.h"
#include "main.h"
#include "os.h"
#include "utils.h"
#include "LPC1769.h"

//int dust_sensor_index;
//volatile struct sDustSensor dust_sensor[300];

volatile struct Task_DSM501_Data task_dsm501_data = {{0},0,30000};

void Task_DSM501(void) {
   char s[32];
   double v;
   Fifo_Uart0_Put("Task_DSM501 has started", 0);
   while(1) {
      v = (double)task_dsm501_data.lowsum / MT * 100;
      mysprintf(s,"%f2%%",(unsigned char*)&v);
      Fifo_HD44780_Put(s, 3, 13);
      OS_Sleep(2000);
   }
}
