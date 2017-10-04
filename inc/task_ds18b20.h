#ifndef TASK_DS18B20_H
#define TASK_DS18B20_H

struct Task_DS18B20_Data {
	int selected;
	short t[3];
};

void Task_DS18B20(void);

#endif
