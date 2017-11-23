#include "task_log.h"
#include "fifos.h"
#include "fs.h"
#include "os.h"
#include "task_bme280.h"
#include "task_ds18b20.h"
#include "utils.h"
#include "LPC1769.h"
#include <string.h>

extern int mtx_files[DIRECTORY_ENTRIES];
extern struct Task_BME280_Data task_bme280_data;
extern struct Task_DS18B20_Data task_ds18b20_data;

struct Task_Log_Data task_log_data;

void Task_Log(void) {
	int f;
	
	Fifo_Uart0_Put("Task_Log has started",0);
	
	while(1) {
		log_record_fill(&task_log_data.log_record);
		memcpy(task_log_data.data_buf+task_log_data.data_len,&task_log_data.log_record,sizeof(struct Log_Record));
		task_log_data.data_len+=sizeof(struct Log_Record);
		if(task_log_data.data_len>=256) {
			task_log_data.data_len=0;
			if((f=fs_filesearch(LOG_FILE_NAME))==STATUS_ERROR)
				f=fs_filenew(LOG_FILE_NAME,0,sizeof(struct Log_Record));
			if(f!=STATUS_ERROR) {
				OS_Blocking_Wait(&mtx_files[f]);
				fs_fileappend(f,task_log_data.data_buf,256);
				fs_flush();
				OS_Blocking_Signal(&mtx_files[f]);
			}
		}
		OS_Sleep(60000); //1 min
	}
}

void log_record_fill(struct Log_Record *lr) {
	int i;
	lr->time=utime();
	lr->bme_h=task_bme280_data.h;
	lr->bme_p=task_bme280_data.p;
	lr->bme_t=task_bme280_data.t;
	for(i=0;i<3;i++)
		lr->ds_t[i]=task_ds18b20_data.t[i];
}
