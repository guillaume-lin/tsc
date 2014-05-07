#ifndef INCLUDED_ISR_TIMER3_H
#define INCLUDED_ISR_TIMER3_H

#include "mcu.h"
#define ADC0GT_BAK0		0x2000			// ADC0 Ain0.0 窗口比较下限值 
#define ADC0LT_BAK0		0xff80			// ADC0 Ain0.0 窗口比较上限值
#define ADC0GT_BAK1		0x2000			// ADC0 Ain0.1 窗口比较下限值
#define ADC0LT_BAK1		0xff80			// ADC0 Ain0.1 窗口比较上限值
#define ADC0GT_BAK2		0x8000			// ADC0 Ain0.2 窗口比较下限值
#define ADC0LT_BAK2		0xff00			// ADC0 Ain0.2 窗口比较上限值
#define ADC0GT_BAK3		0x8000			// ADC0 Ain0.3 窗口比较下限值
#define ADC0LT_BAK3		0xfff0			// ADC0 Ain0.3 窗口比较上限值
#define ADC0GT_BAK4		0x8000			// ADC0 Ain0.4 窗口比较下限值
#define ADC0LT_BAK4		0xfff0			// ADC0 Ain0.4 窗口比较上限值
#define ADC0GT_BAK5		0x8000			// ADC0 Ain0.5 窗口比较下限值
#define ADC0LT_BAK5		0xff00			// ADC0 Ain0.5 窗口比较上限值
#define ADC0GT_BAK6		0x8000			// ADC0 Ain0.6 窗口比较下限值
#define ADC0LT_BAK6		0xfff0			// ADC0 Ain0.6 窗口比较上限值
#define ADC0GT_BAK7		0x6000			// ADC0 Ain0.7 窗口比较下限值
#define ADC0LT_BAK7		0xfff0			// ADC0 Ain0.7 窗口比较上限值

extern bit SYS5ms,SYS25ms,SYS100ms,SYS200ms,f_rd_lamp_input_now,f_key2_int;
//1219	extern bit f_manual_use;

extern unsigned char data g_in_write_p[8];
extern unsigned char data g_out_read_p[8];  
extern unsigned char data g_out_write_p[8];

extern unsigned char lamp_input_buffer[11][2*((MAX_LAMP_NUM - 1)/12 + 1)];		// 交通灯状态输入缓冲（第0、1、2行保存最近三次读入值），第3行存放
		// 各灯的实际状态（由于实行的是半波(10ms)检测, 8ms 一次），通过该行的数据与送显的数据比较，来判断灯的好坏，并进行绿冲突检测
#endif