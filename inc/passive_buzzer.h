#ifndef PASSIVE_BUZZER_H
#define PASSIVE_BUZZER_H

struct PassiveBuzzerConfig {
	struct Tone *song;
	int current_songtone,count_songtones;
};

struct Tone {
	int freq, //Hz
			duration; //ms
};

void Passive_Buzzer_Init(void);
void Passive_Buzzer_Tone(int);

void Passive_Buzzer_Start_Song(struct Tone *song,int count_songtones);
void Passive_Buzzer_Stop_Song(void);


#endif
