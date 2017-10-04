#include "passive_buzzer.h"
#include "main.h"
#include "pwm.h"
#include "LPC1769.h"

struct PassiveBuzzerConfig passive_buzzer_config;

void Passive_Buzzer_Init(void) {
	PWM_Init();
}

void Passive_Buzzer_Start_Song(struct Tone *song,int count_songtones) {
	passive_buzzer_config.song=song;
	passive_buzzer_config.count_songtones=count_songtones;
	passive_buzzer_config.current_songtone=0;
	T2MR0 = song[0].duration*CLOCK*1000-1;
	T2MR1 = (song[0].duration+50)*CLOCK*1000-1;
	Passive_Buzzer_Tone(song[0].freq);
	T2TCR = (T2TCR&(~((1<<0) | (1<<1)))) | (1<<0); //counters anti-resetted and enabled
}

void Passive_Buzzer_Stop_Song(void) {
	Passive_Buzzer_Tone(0);
	T2TCR = (T2TCR&(~((1<<0) | (1<<1))) )| (1<<1); //counters disabled and resetted
}

void Passive_Buzzer_Tone(int f) { //dc is always 50% [for now]
	if(f<=0) {
		PWM1TCR&=(~(1<<3)); //disable pwm
		PWM1MCR |= ((1<<4) | (1<<5)); //reset and stop counter
	}
	else {
		PWM1MCR &= (~((1<<4) | (1<<5))); //clear reset and stop on MR1
		PWM1MR0 = 1000000*CLOCK/f;
		PWM1MR1 = 1000000*CLOCK/f/2;
		PWM1LER |= (1<<0) | (1<<1); //enable MR0 and MR1 latch
		PWM1TCR = (PWM1TCR&(~(1<<1))) | (1<<0) | (1<<3); //counters enabled, counter reset cleared, PWM enabled
	}
}
