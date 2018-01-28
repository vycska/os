#ifndef __TASK_DSM501_H__
#define __TASK_DSM501_H__

#define MT 30000 //measuring time in ms

struct sDustSensor {
   long long int millis;
   unsigned int value;
};

struct Task_DSM501_Data {
   unsigned char d[MT/8];
   int i,lowsum;
};

void Task_DSM501(void);

#endif
