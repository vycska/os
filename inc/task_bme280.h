#ifndef TASK_BME280_H
#define TASK_BME280_H

struct Task_BME280_Data {
   short h, p, t;
};

void Task_BME280(void);

#endif
