//#define single_mode 1
#ifndef INCLUDED_ISR_UART_H
#define INCLUDED_ISR_UART_H

#include "mcu.h"
extern bit f_uart1_using;
extern unsigned char out_getchar(int port);
extern int is_out_full(int port);
extern int in_putchar(int port, unsigned char ch);
extern int is_in_empty(int port);
extern int is_out_empty(int port);

extern unsigned int g_in_write_p[8];
extern unsigned int g_out_read_p[8];  
extern unsigned int g_out_write_p[8]; 

#endif