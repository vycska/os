#ifndef TASK_LOG_H
#define TASK_LOG_H

#define LOG_FILE_NAME "log.txt"

struct Log_Record { //sizeof must be s.t. divide 1024 [e.g. now 16]
	int time;
	short bme_h,
				bme_p,
				bme_t,
				ds_t[3];
};

struct Task_Log_Data {
	char data_buf[256];
	int data_len;
	struct Log_Record log_record;
};

void Task_Log(void);
void log_record_fill(struct Log_Record*);

#endif
