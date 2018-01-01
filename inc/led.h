#ifndef BOARD_LED_H
#define BOARD_LED_H

struct BoardLed_Config {
   char active_color, dc;
   int period;
   volatile int counter;
};

void Board_LED_Init(void);
void Board_LED(unsigned char);

#endif
