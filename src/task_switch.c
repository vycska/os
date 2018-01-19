#include "task_switch.h"
#include "active_buzzer.h"
#include "fifos.h"
#include "hd44780.h"
#include "led.h"
#include "main.h"
#include "os.h"
#include "passive_buzzer.h"
#include "pitches.h"
#include "switch.h"
#include "task_ds18b20.h"
#include "task_oled.h"
#include "timer.h"
#include "utils.h"
#include "LPC1769.h"

extern int ds18b20_selected;
extern struct BoardLed_Config boardled_config;
extern struct Task_DS18B20_Data task_ds18b20_data;
extern struct Task_Oled_Data task_oled_data;

int smphr_switch, status_switch;

struct Tone star_wars_song[] = {
   {NOTE_A4, 500}, {NOTE_A4, 500}, {NOTE_A4, 500}, {NOTE_F4, 350}, {NOTE_C5, 150}, {NOTE_A4, 500}, {NOTE_F4, 350}, {NOTE_C5, 150}, {NOTE_A4, 650},
   {NOTE_0, 500},
   {NOTE_E5, 500}, {NOTE_E5, 500}, {NOTE_E5, 500}, {NOTE_F5, 350}, {NOTE_C5, 150}, {NOTE_GS4, 500}, {NOTE_F4, 350}, {NOTE_C5, 150}, {NOTE_A4, 650},
   {NOTE_0, 500},
   {NOTE_A5, 500}, {NOTE_A4, 300}, {NOTE_A4, 150}, {NOTE_A5, 500}, {NOTE_GS5, 325}, {NOTE_G5, 175}, {NOTE_FS5, 125}, {NOTE_F5, 125}, {NOTE_FS5, 250},
   {NOTE_0, 325},
   {NOTE_AS4, 250}, {NOTE_DS5, 500}, {NOTE_D5, 325}, {NOTE_CS5, 175}, {NOTE_C5, 125}, {NOTE_AS4, 125}, {NOTE_C5, 250},
   {NOTE_0, 350}
};

struct Tone solfeggio_song[] = {
   {NOTE_C5, 500}, {NOTE_D5, 500}, {NOTE_E5, 500}, {NOTE_F5, 500}, {NOTE_G5, 500}, {NOTE_A5, 500}, {NOTE_B5, 500}, {NOTE_C6, 500},
   {NOTE_0, 500},
   {NOTE_C6, 500}, {NOTE_B5, 500}, {NOTE_A5, 500}, {NOTE_G5, 500}, {NOTE_F5, 500}, {NOTE_E5, 500}, {NOTE_D5, 500}, {NOTE_C5, 500}
};

struct Tone jingle_bells_song[] = {
   {NOTE_E5, 300}, {NOTE_E5, 300}, {NOTE_E5, 600}, {NOTE_0, 300},
   {NOTE_E5, 300}, {NOTE_E5, 300}, {NOTE_E5, 600}, {NOTE_0, 300},
   {NOTE_E5, 300}, {NOTE_G5, 300}, {NOTE_C5, 300}, {NOTE_D5, 300}, {NOTE_E5, 1200}, {NOTE_0, 300},
   {NOTE_F5, 300}, {NOTE_F5, 300}, {NOTE_F5, 300}, {NOTE_F5, 300}, {NOTE_F5, 300}, {NOTE_E5, 300}, {NOTE_E5, 300}, {NOTE_E5, 300}, {NOTE_E5, 300}, {NOTE_D5, 300}, {NOTE_D5, 300}, {NOTE_E5, 300}, {NOTE_D5, 600}, {NOTE_0, 300},
   {NOTE_G5, 600}, {NOTE_0, 1000},
};

struct Songs {
   struct Tone *song;
   int k_notes;
} songs[] = {
   {
   star_wars_song, sizeof(star_wars_song) / sizeof(struct Tone)}, {
   solfeggio_song, sizeof(solfeggio_song) / sizeof(struct Tone)}, {
   jingle_bells_song, sizeof(jingle_bells_song) / sizeof(struct Tone)}
};

void Task_Switch(void) {
   char s[24];
   int flag_playing = 0, current_song = 0, smphr_finished;
   double t;

   Fifo_Uart0_Put("Task_Switch has started", 0);

   Switch_Init();

   OS_InitSemaphore(&smphr_switch, 0);
   OS_InitSemaphore(&smphr_finished, 0);

   while(1) {
      OS_Blocking_Wait(&smphr_switch);
      if(status_switch == 2) {  //value 2 means rising
         Timer1_Start();

         if(!flag_playing) {
            flag_playing = 1;
            Passive_Buzzer_Start_Song(songs[current_song].song, songs[current_song].k_notes);
            current_song = (current_song + 1) % (sizeof(songs) / sizeof(struct Songs));
         }
         else {
            flag_playing = 0;
            Passive_Buzzer_Stop_Song();
         }

         Active_Buzzer_Command(1);
         boardled_config.active_color = (boardled_config.active_color + 1) % 8;
         task_ds18b20_data.selected = (task_ds18b20_data.selected + 1) % 3;
         task_oled_data.k = (task_oled_data.k+1)%6;
      }
      else if(status_switch == 1) {     // value 1 means falling
         t = (double)Timer1_Stop() / CLOCK / 1000000;
         Active_Buzzer_Command(0);
         mysprintf(s, "%f4", (char *)&t);
         //HD44780_ClearLine(3);
         Fifo_HD44780_Put("                    ",3,0);
         Fifo_HD44780_Put(s, 3, 0);
         mysprintf(s, "%u-%u-%u", YEAR & 0xfff, MONTH & 0xf, DOM & 0x1f);
         Fifo_HD44780_Put(s, 2, 10);
         mysprintf(s, "%u:%u:%u", HOUR & 0x1f, MIN & 0x3f, SEC & 0x3f);
         Fifo_HD44780_Put(s, 3, 10);
      }
      OS_Sleep(10);             //switch may bounce during this period

      //enable interrupts
      IO2IntClr |= (1 << 13);   //clear interrupt
      IO2IntEnR |= (1 << 13);
      IO2IntEnF |= (1 << 13);
   }
}
