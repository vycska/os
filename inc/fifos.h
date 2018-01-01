#ifndef FIFOS_H
#define FIFOS_H

#define FIFO_COMMAND_PARSER_ITEMS 4
#define FIFO_COMMAND_PARSER_ITEM_SIZE 48
#define FIFO_HD44780_ITEMS 4
#define FIFO_HD44780_ITEM_SIZE 21       //20 matomu simboliu ir gale \0
#define FIFO_UART0_OUTPUT_ITEMS 10
#define FIFO_OLED_ITEMS 6
#define FIFO_OLED_ITEM_SIZE 21

struct Fifo_Command_Parser {
   char buffer[FIFO_COMMAND_PARSER_ITEMS][FIFO_COMMAND_PARSER_ITEM_SIZE];
   int i_get, i_put, mtx_fifo_command_parser, smphr_count_items;
};

struct Fifo_HD44780 {           //no checking if there is free space, just overwriting
   char buffer[FIFO_HD44780_ITEMS][FIFO_HD44780_ITEM_SIZE];
   int i_get, i_put, length[FIFO_HD44780_ITEMS], line[FIFO_HD44780_ITEMS], pos[FIFO_HD44780_ITEMS], mtx_fifo_hd44780, smphr_count_items;
};

struct Fifo_Uart0_Output {
   char *buffer[FIFO_UART0_OUTPUT_ITEMS];
   int i_get, i_put, mtx_fifo_uart0_output, smphr_count_items, smphr_space_left, *smphr_finished[FIFO_UART0_OUTPUT_ITEMS];
};

struct Fifo_Oled {
   char buffer[FIFO_OLED_ITEMS][FIFO_OLED_ITEM_SIZE];
   int i_get,i_put,length[FIFO_OLED_ITEMS],line[FIFO_OLED_ITEMS],mtx_fifo_oled,smphr_count_items;
};

/* ********** */

void Fifo_Command_Parser_Init(void);
void Fifo_Command_Parser_Get(char **);
void Fifo_Command_Parser_Put(char *);

void Fifo_HD44780_Init(void);
void Fifo_HD44780_Get(char **, int *, int *, int *);
void Fifo_HD44780_Put(char *, int, int);

void Fifo_Uart0_Init(void);
void Fifo_Uart0_Get(char **, int **);
void Fifo_Uart0_Put(char *, int *);

void Fifo_Oled_Init(void);
void Fifo_Oled_Get(char **, int *, int *);
void Fifo_Oled_Put(char *, int);

#endif
