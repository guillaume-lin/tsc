#ifndef INCLUDED_MAIN_H
#define INCLUDED_MAIN_H

#include <intrins.h>
#include "c8051f020.h"							// SFR declarations

//#define single_mode 1

#define ADDRESS_PC104_RD	0xda00				// 读 PC104 的地址
#define ADDRESS_PC104_RD1	0xdc00				// 读 PC104 的地址
#define ADDRESS_PC104_WR	0xde00				// 写 PC104 的地址

#define ADDRESS_82C55A		0xcaxx				// 82C55A 的地址(CS0)
#define ADDRESS_82C55A_A	0xca00				// 82C55A 的数据 A 口地址
#define ADDRESS_82C55A_B	0xca01				// 82C55A 的数据 B 口地址
#define ADDRESS_82C55A_C	0xca02				// 82C55A 的数据 C 口地址
#define ADDRESS_82C55A_D	0xca03				// 82C55A 的控制口地址

#define ADDRESS_82C55B		0xccxx				// 82C55B 的地址(CS1)
#define ADDRESS_82C55B_A	0xcc00				// 82C55B 的数据 A 口地址
#define ADDRESS_82C55B_B	0xcc01				// 82C55B 的数据 B 口地址
#define ADDRESS_82C55B_C	0xcc02				// 82C55B 的数据 C 口地址
#define ADDRESS_82C55B_D	0xcc03				// 82C55B 的控制口地址

#define ADDRESS_82C55C		0xcexx				// 82C55C 的地址(CS2)
#define ADDRESS_82C55C_A	0xce00				// 82C55C 的数据 A 口地址
#define ADDRESS_82C55C_B	0xce01				// 82C55C 的数据 B 口地址
#define ADDRESS_82C55C_C	0xce02				// 82C55C 的数据 C 口地址
#define ADDRESS_82C55C_D	0xce03				// 82C55C 的控制口地址

#define ADDRESS_82C55D		0xd0xx				// 82C55D 的地址(CS3)（备用）
#define ADDRESS_82C55D_A	0xd000				// 82C55D 的数据 A 口地址
#define ADDRESS_82C55D_B	0xd001				// 82C55D 的数据 B 口地址
#define ADDRESS_82C55D_C	0xd002				// 82C55D 的数据 C 口地址
#define ADDRESS_82C55D_D	0xd003				// 82C55D 的控制口地址

#define ADDRESS_keyboard	0xd200				// 键盘扫描线地址(CS4A)

#define ADDRESS_74HC573A	0xa000				// 74HC573A 的地址(OCS0)   灯控板 1 输出片选
#define ADDRESS_74HC573B	0xa200				// 74HC573B 的地址(OCS1)   灯控板 1 输出片选
#define ADDRESS_74HC573C	0xa400				// 74HC573C 的地址(OCS2)   灯控板 2 输出片选
#define ADDRESS_74HC573D	0xa600				// 74HC573D 的地址(OCS3)   灯控板 2 输出片选
#define ADDRESS_74HC573E	0xa800				// 74HC573E 的地址(OCS4)   灯控板 3 输出片选
#define ADDRESS_74HC573F	0xaa00				// 74HC573F 的地址(OCS5)   灯控板 3 输出片选
#define ADDRESS_74HC573G	0xac00				// 74HC573G 的地址(OCS6)   灯控板 4 输出片选
#define ADDRESS_74HC573H	0xae00				// 74HC573H 的地址(OCS7)   灯控板 4 输出片选
#define ADDRESS_74HC573I	0xb000				// 74HC573I 的地址(OCS8)   灯控板 5 输出片选
#define ADDRESS_74HC573J	0xb200				// 74HC573J 的地址(OCS9)   灯控板 5 输出片选
#define ADDRESS_74HC573K	0xb400				// 74HC573K 的地址(OCS10)  灯控板 6 输出片选
#define ADDRESS_74HC573L	0xb600				// 74HC573L 的地址(OCS11)  灯控板 6 输出片选
#define ADDRESS_74HC573M	0xb800				// 74HC573M 的地址(OCS12)  备用
#define ADDRESS_74HC573N	0xba00				// 74HC573N 的地址(OCS13)  备用
#define ADDRESS_74HC573O	0xbc00				// 74HC573O 的地址(OCS14)  备用
#define ADDRESS_74HC573P	0xbe00				// 74HC573P 的地址(OCS15)  备用

#define ADDRESS_74HC244A	0xa000				// 74HC244E 的地址(ICS4)   灯控板 1 输入片选		
#define ADDRESS_74HC244B	0xa200				// 74HC244F 的地址(ICS5)   灯控板 1 输入片选        
#define ADDRESS_74HC244C	0xa400				// 74HC244G 的地址(ICS6)   灯控板 2 输入片选        
#define ADDRESS_74HC244D	0xa600				// 74HC244H 的地址(ICS7)   灯控板 2 输入片选        
#define ADDRESS_74HC244E	0xa800				// 74HC244I 的地址(ICS8)   灯控板 3 输入片选		
#define ADDRESS_74HC244F	0xaa00				// 74HC244J 的地址(ICS9)   灯控板 3 输入片选        
#define ADDRESS_74HC244G	0xac00				// 74HC244K 的地址(ICS10)  灯控板 4 输入片选        
#define ADDRESS_74HC244H	0xae00				// 74HC244L 的地址(ICS11)  灯控板 4 输入片选        
#define ADDRESS_74HC244I	0xb000				// 74HC244M 的地址(ICS12)  灯控板 5 输入片选        
#define ADDRESS_74HC244J	0xb200				// 74HC244N 的地址(ICS13)  灯控板 5 输入片选        
#define ADDRESS_74HC244K	0xb400				// 74HC244O 的地址(ICS14)  灯控板 6 输入片选        
#define ADDRESS_74HC244L	0xb600				// 74HC244P 的地址(ICS15)  灯控板 6 输入片选        
#define ADDRESS_74HC244M	0xbe00				// 74HC244A 的地址(ICS0)   车辆检测板 1       		
#define ADDRESS_74HC244N	0xc000				// 74HC244B 的地址(ICS1)   车辆检测板 2         	
#define ADDRESS_74HC244O	0xc200				// 74HC244C 的地址(ICS2)   车辆检测板 3         	
#define ADDRESS_74HC244P	0xc400				// 74HC244D 的地址(ICS3)   车辆检测板 4 (备用)  	
#define ADDRESS_74HC244Q	0xb800				// 74HC573Q 的地址(ICS16)  备用
#define ADDRESS_74HC244R	0xba00				// 74HC573R 的地址(ICS17)  备用
#define ADDRESS_74HC244S	0xbc00				// 74HC573S 的地址(ICS18)  备用


#define check_chanl_0		0x01				// 每块车辆检测板第 0 路
#define check_chanl_1		0x02				// 每块车辆检测板第 1 路
#define check_chanl_2		0x04				// 每块车辆检测板第 2 路
#define check_chanl_3		0x08				// 每块车辆检测板第 3 路
#define check_chanl_4		0x10				// 每块车辆检测板第 4 路
#define check_chanl_5		0x20				// 每块车辆检测板第 5 路
#define check_chanl_6		0x40				// 每块车辆检测板第 6 路
#define check_chanl_7		0x80				// 每块车辆检测板第 7 路

#define MAX_LAMP_NUM		72					// 系统最大的信号灯数 
#define MAX_DETECTOR_NUM	32					// 系统最大的检测器数
#define UART_CHANL			P2


#define KEY0_MOD			0x10				// 按键 0 步进值
#define MANUAL 				0xe1				// 手动自动选择 225
#define MANUAL_MOD1			0xbb				// 手动模式 1 187
#define MANUAL_MOD1A		0xcf				// 手动模式 1 按键 207
#define MANUAL_MOD2			0x83				// 手动模式 2 131
#define MANUAL_MOD2A		0x97				// 手动模式 2 按键 151
#define MANUAL_MOD3			0x3b				// 手动模式 3 59
#define MANUAL_MOD3A		0x48				// 手动模式 3 按键 72
                                        		
#define S21					0xf0				// 强制黄闪 240
#define S22					0x9f				// 强制全红 159
#define S23					0x4f				// 预留 79

#define DELAY_TIME			60000

#define key_port			P1					// 键盘扫描输入口


//050628#define TimeEnable() SYSTime=0; if(SYS5ms){ SYSTime=1; SYS5ms=0;watchdog=~watchdog;}

                                        		
//4sbit watchdog 				= P3^0;				// 看门狗输出    
//4sbit chip_uart_rst			= P3^1;
//4sbit chip_uart_ms			= P3^7;
//4sbit state_pc104 			= P2^7;				// PC104 状态
//4sbit buzz		 			= P1^6;				// 蜂鸣器输出
                    		
sfr16 DP       				= 0x82;				// data pointer
sfr16 TMR3RL   				= 0x92;				// Timer3 reload value
sfr16 TMR3     				= 0x94;				// Timer3 counter
sfr16 ADC0     				= 0xbe;				// ADC0 data
sfr16 ADC0GT   				= 0xc4;				// ADC0 greater than window
sfr16 ADC0LT   				= 0xc6;				// ADC0 less than window
sfr16 RCAP2    				= 0xca;				// Timer2 capture/reload
sfr16 T2       				= 0xcc;				// Timer2
sfr16 DAC0     				= 0xd2;				// DAC0 data
sfr16 DAC1     				= 0xd5;				// DAC1 data
sfr16 RCAP4    				= 0xe4;				// Timer4 capture/reload
sfr16 T4       				= 0xf4;				// Timer4

char bdata flag_key0,flag_key1,flag_key2;
sbit f_key0_ok 				= flag_key0^0;		// 键盘 0 "确认"
sbit f_key0_menu 			= flag_key0^1;		// 键盘 0 "菜单"
sbit f_key0_right 			= flag_key0^2;		// 键盘 0 "右移"
sbit f_key0_left 			= flag_key0^3;		// 键盘 0 "左移"
sbit f_key0_down 			= flag_key0^4;		// 键盘 0 "下翻"
sbit f_key0_up 				= flag_key0^5;		// 键盘 0 "上翻"
sbit f_key0_err 			= flag_key0^6;		// 出现中间值，有可能出错(如组合键)	
                    		
sbit f_key1 				= flag_key1^0;		// 手动模式中有步进控制键按下
sbit f_manual 				= flag_key1^1;		// 手动模式（该标志位不参与控制，只为以后显示用）
sbit f_key1_ok 				=  flag_key1^2;		// 手动模式中步进控制键放开
sbit f_manual_mod1 			= flag_key1^3;		// 手动模式 1
sbit f_manual_mod2 			= flag_key1^4;		// 手动模式 2
sbit f_manual_mod3 			= flag_key1^5;		// 手动模式 3
sbit f_key1_stabile 		= flag_key1^6;		// 
                    		
sbit f_key2_s21 			= flag_key2^0;		//
sbit f_key2_s22 			= flag_key2^1;		//
sbit f_key2_s23 			= flag_key2^2;		//
sbit f_key2_err 			= flag_key2^3;		// 出现中间值，有可能出错(如组合键)	

bit f_key,f_key_now,f_buzz,flick_mark;
bit SYS5ms,SYS100ms,SYS200ms,SYSTime,f_frame_ack1,bit_test,f_time,f_uart_rst,f_rd_lamp_input_now;


unsigned char lamp_status[(MAX_LAMP_NUM-1)/8 + 1];					// 转换后的灯状态
//unsigned char flick_count;
unsigned char lamp_out[(MAX_LAMP_NUM-1)/4 + 1];
//unsigned char lamp_output_buffer[2*((MAX_LAMP_NUM - 1)/12 + 1)];	// 交通灯输出缓冲 
//unsigned char lamp_flick_buffer[2*((MAX_LAMP_NUM - 1)/12 + 1)];	// 交通灯闪光输出缓冲 
//unsigned char lamp_input_buffer[8][2*((MAX_LAMP_NUM - 1)/12 + 1)];		// 交通灯状态输入缓冲（第0、1、2行保存最近三次读入值），第3行存放
		// 各灯的实际状态（由于实行的是半波(10ms)检测, 8ms 一次），通过该行的数据与送显的数据比较，来判断灯的好坏，并进行绿冲突检测

unsigned char vehicle_info[(MAX_DETECTOR_NUM-1)/8 + 1];
unsigned char vehicle_count[MAX_DETECTOR_NUM];

unsigned char contt,trans_count,chartemp2,key0_time,key1_val,key1_time,key1_adc0_old,key2_time,key2_adc0_old;

unsigned char pc104_test,pc104_test1,chartemp3,test_w,test_w1,test_w2,test_w3;

//unsigned char code key0_table[]={0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40};

//unsigned char code scan_table[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
//unsigned char code key_table[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x00,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};

void initialize_mcu(void);

#endif
