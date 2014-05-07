#ifndef INCLUDED_ISR_INT0_H
#define INCLUDED_ISR_INT0_H
#include "mcu.h"
#include <intrins.h>
extern bit f_cpld_chanl0_using,f_cpld_chanl1_using,f_cpld_trans_ed,f_cpld_trans_num;
extern unsigned int g_in_write_p[8];
extern unsigned int g_out_read_p[8];  
extern unsigned int g_out_write_p[8]; 
extern int is_out_empty(int port);
extern unsigned char out_getchar(int port);
extern void in_putchar(int port,unsigned char ch);
extern unsigned char cpld_count,cpld_trans_bak;
#endif
