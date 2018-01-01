#include "fifos.h"
#include "os.h"
#include <string.h>

struct Fifo_Command_Parser fifo_command_parser;
struct Fifo_HD44780 fifo_hd44780;
struct Fifo_Uart0_Output fifo_uart0_output;

void Fifo_Command_Parser_Init(void) {
   fifo_command_parser.i_get = fifo_command_parser.i_put = 0;
   OS_InitSemaphore(&fifo_command_parser.mtx_fifo_command_parser, 1);
   OS_InitSemaphore(&fifo_command_parser.smphr_count_items, 0);
}

void Fifo_Command_Parser_Get(char **pString) {
   OS_Blocking_Wait(&fifo_command_parser.smphr_count_items);
   OS_Blocking_Wait(&fifo_command_parser.mtx_fifo_command_parser);

   *pString = fifo_command_parser.buffer[fifo_command_parser.i_get];
   fifo_command_parser.i_get = (fifo_command_parser.i_get + 1) % FIFO_COMMAND_PARSER_ITEMS;

   OS_Blocking_Signal(&fifo_command_parser.mtx_fifo_command_parser);
}

void Fifo_Command_Parser_Put(char *pString) {
   OS_Blocking_Wait(&fifo_command_parser.mtx_fifo_command_parser);

   strcpy(fifo_command_parser.buffer[fifo_command_parser.i_put], pString);
   fifo_command_parser.i_put = (fifo_command_parser.i_put + 1) % FIFO_COMMAND_PARSER_ITEMS;

   OS_Blocking_Signal(&fifo_command_parser.mtx_fifo_command_parser);
   OS_Blocking_Signal(&fifo_command_parser.smphr_count_items);
}

/* ********** */

void Fifo_HD44780_Init(void) {
   fifo_hd44780.i_get = fifo_hd44780.i_put = 0;
   OS_InitSemaphore(&fifo_hd44780.mtx_fifo_hd44780, 1);
   OS_InitSemaphore(&fifo_hd44780.smphr_count_items, 0);
}

void Fifo_HD44780_Get(char **pString, int *len, int *line, int *pos) {
   OS_Blocking_Wait(&fifo_hd44780.smphr_count_items);
   OS_Blocking_Wait(&fifo_hd44780.mtx_fifo_hd44780);

   *pString = fifo_hd44780.buffer[fifo_hd44780.i_get];
   *len = fifo_hd44780.length[fifo_hd44780.i_get];
   *line = fifo_hd44780.line[fifo_hd44780.i_get];
   *pos = fifo_hd44780.pos[fifo_hd44780.i_get];
   fifo_hd44780.i_get = (fifo_hd44780.i_get + 1) % FIFO_HD44780_ITEMS;

   OS_Blocking_Signal(&fifo_hd44780.mtx_fifo_hd44780);
}

void Fifo_HD44780_Put(char *pString, int line, int pos) {
   int i;

   OS_Blocking_Wait(&fifo_hd44780.mtx_fifo_hd44780);

   for(i = 0; i < FIFO_HD44780_ITEM_SIZE - 1 && pString[i] != 0; i++)
      fifo_hd44780.buffer[fifo_hd44780.i_put][i] = pString[i];
   fifo_hd44780.buffer[fifo_hd44780.i_put][i] = 0;
   fifo_hd44780.length[fifo_hd44780.i_put] = i;
   fifo_hd44780.line[fifo_hd44780.i_put] = line;
   fifo_hd44780.pos[fifo_hd44780.i_put] = pos;
   fifo_hd44780.i_put = (fifo_hd44780.i_put + 1) % FIFO_HD44780_ITEMS;

   OS_Blocking_Signal(&fifo_hd44780.mtx_fifo_hd44780);
   OS_Blocking_Signal(&fifo_hd44780.smphr_count_items);
}

/* ********** */

void Fifo_Uart0_Init(void) {
   fifo_uart0_output.i_get = fifo_uart0_output.i_put = 0;
   OS_InitSemaphore(&fifo_uart0_output.mtx_fifo_uart0_output, 1);
   OS_InitSemaphore(&fifo_uart0_output.smphr_count_items, 0);
   OS_InitSemaphore(&fifo_uart0_output.smphr_space_left, FIFO_UART0_OUTPUT_ITEMS);
}

void Fifo_Uart0_Get(char **pString, int **smphrFinished) {
   OS_Blocking_Wait(&fifo_uart0_output.smphr_count_items);
   OS_Blocking_Wait(&fifo_uart0_output.mtx_fifo_uart0_output);

   *pString = fifo_uart0_output.buffer[fifo_uart0_output.i_get];
   *smphrFinished = fifo_uart0_output.smphr_finished[fifo_uart0_output.i_get];
   fifo_uart0_output.i_get = (fifo_uart0_output.i_get + 1) % FIFO_UART0_OUTPUT_ITEMS;

   OS_Blocking_Signal(&fifo_uart0_output.mtx_fifo_uart0_output);
   OS_Blocking_Signal(&fifo_uart0_output.smphr_space_left);
}

void Fifo_Uart0_Put(char *pString, int *smphrFinished) {
   OS_Blocking_Wait(&fifo_uart0_output.smphr_space_left);
   OS_Blocking_Wait(&fifo_uart0_output.mtx_fifo_uart0_output);

   fifo_uart0_output.buffer[fifo_uart0_output.i_put] = pString;
   fifo_uart0_output.smphr_finished[fifo_uart0_output.i_put] = smphrFinished;
   fifo_uart0_output.i_put = (fifo_uart0_output.i_put + 1) % FIFO_UART0_OUTPUT_ITEMS;

   OS_Blocking_Signal(&fifo_uart0_output.mtx_fifo_uart0_output);
   OS_Blocking_Signal(&fifo_uart0_output.smphr_count_items);
}

/*
void Fifo_Oled_Init(void) {
   fifo_oled.i_get=fifo_oled.i_put=0;
   OS_InitSemaphore(&fifo_oled.mtx_fifo_oled,1);
   OS_InitSemaphore(&fifo_oled.smphr_count_items,0);
}

void Fifo_Oled_Get(char **pString,int *len,int *line) {
   OS_Blocking_Wait(&fifo_oled.smphr_count_items);
   OS_Blocking_Wait(&fifo_oled.mtx_fifo_oled);

   *pString=fifo_oled.buffer[fifo_oled.i_get];
   *len=fifo_oled.length[fifo_oled.i_get];
   *line=fifo_oled.line[fifo_oled.i_get];
   fifo_oled.i_get = (fifo_oled.i_get+1) % FIFO_OLED_ITEMS;

   OS_Blocking_Signal(&fifo_oled.mtx_fifo_oled);
}

void Fifo_Oled_Put(char *pString,int line) {
   int i;

   OS_Blocking_Wait(&fifo_oled.mtx_fifo_oled);
   for(i=0;i<FIFO_OLED_ITEM_SIZE-1 && pString[i]!=0;i++)
      fifo_oled.buffer[fifo_oled.i_put][i] = pString[i];
   fifo_oled.buffer[fifo_oled.i_put][i] = 0;
   fifo_oled.length[fifo_oled.i_put] = i;
   fifo_oled.line[fifo_oled.i_put] = line;
   fifo_oled.i_put = (fifo_oled.i_put+1) % FIFO_OLED_ITEMS;

   OS_Blocking_Signal(&fifo_oled.mtx_fifo_oled);
   OS_Blocking_Signal(&fifo_oled.smphr_count_items);
}
*/
