#ifndef INCLUDED_MCU_H
#define INCLUDED_MCU_H
#include <intrins.h>
#include "c8051f020.h"							// SFR declarations
#include "datastruct.h"

//#define ADDRESS_PC104_RD	0xda00				// 读 PC104 的地址
//#define ADDRESS_PC104_RD1	0xdc00				// 读 PC104 的地址
//#define ADDRESS_PC104_WR	0xde00				// 写 PC104 的地址

#define ADDRESS_PC_WR0		0xc600				// 写 PC104 的地址
#define ADDRESS_PC_WR1		0xc800				// 写 PC104 的地址
#define ADDRESS_PC_WR2		0xd400	
#define ADDRESS_PC_RD0A		0xd600				// 读 PC104 的地址
#define ADDRESS_PC_RD0B		0xd800				// 读 PC104 的地址
#define ADDRESS_PC_RD1A		0xda00				// 读 PC104 的地址
#define ADDRESS_PC_RD1B		0xdc00				// 读 PC104 的地址
#define ADDRESS_PC_STATUS	0xde00				// 读 PC104 状态的地址

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
//#define ADDRESS_74HC573M	0xb800				// 74HC573M 的地址(OCS12)  备用
//#define ADDRESS_74HC573N	0xba00				// 74HC573N 的地址(OCS13)  备用
//#define ADDRESS_74HC573O	0xbc00				// 74HC573O 的地址(OCS14)  备用
//#define ADDRESS_74HC573P	0xbe00				// 74HC573P 的地址(OCS15)  备用
#define ADDRESS_FRONT_PANEL 0xba00				// 74HC573P 的地址(OCS16)  前面板控制模式显示

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
//#define ADDRESS_74HC244Q	0xb800				// 74HC573Q 的地址(ICS16)  备用
//#define ADDRESS_74HC244R	0xba00				// 74HC573R 的地址(ICS17)  备用
//#define ADDRESS_74HC244S	0xbc00				// 74HC573S 的地址(ICS18)  备用
#define ADDRESS_TELECONTROL	0xbc00				// 74HC573T 的地址(ICS19)  遥控器输入片选

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

#define key_port			P1					// 键盘扫描输入口
#define UART_CHANL			P2

#define KEY0_MOD			0x37				// 按键 0 步进值
#define KEY1_MOD			0x37				// 按键 1 步进值
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

#define	WRITE				0x00				// SMBus WRITE command
#define	READ				0x01				// SMBus READ command


#define	CHIP_RTC			0xd0				// Slave address for chip RTC
#define	CHIP_EEPROM			0xa2				// Slave address for chip EEPROM
#define	SLAVE_LCD			0x70				// Slave address for chip LCD
#define DEVICE_LCD			0xe0				// Device Address for chip LCD	  
#define MODE_LCD			0xc8				// Mode of LCD


#define uart0_trans_max 	350					//	200
#define uart0_received_max 	350					//	200

sbit power		 			= P1^3;				// 电源控制
sbit buzz		 			= P1^6;				// 蜂鸣器输出
sbit s_fy					= P2^7; 			// 黄闪控制端（高电平有效）
sbit led_bg	 				= P2^6;				// led 背光控制端（高电平有效） 
sbit watchdog 				= P3^0;				// 看门狗输出
sbit chip_uart_rst			= P3^1;
sbit pw_dc					= P3^4;
sbit pw_ac					= P3^5;
sbit chip_uart_ms			= P3^7;


extern bit f_rd_smbus_ok;						// smbus 读缓冲区有数据等待读出
extern bit SYS5ms,SYS100ms;
extern bit f_key0_int,f_key1_int,f_key2_int,f_key3_int;
extern bit SM_BUSY,f_step_all_ok,f_lcd,f_trans_aa,f_out0_int;
extern bit f_wr_eeprom_now;
extern bit f_temperature;

extern unsigned char key0_adc0,key1_adc0,key2_adc0,key3_adc0;

extern unsigned char data COMMAND;				// Holds the slave address + R/W bit for
												// use in the SMBus ISR.

extern unsigned char data WORD;					// Holds data to be transmitted by the SMBus
												// OR data that has just been received.

extern unsigned char data ADDR_NUMBER;			// Used by ISR to check what data has just been
												// sent - High address byte, Low byte, or data byte
extern unsigned char data wr_eeprom_count;

//extern unsigned char buffer_pc104[2][131];

extern unsigned char data HIGH_ADD,LOW_ADD;		// High & Low byte for EEPROM memory address
extern unsigned char data SMB_WR_NUM_BAK;		// SMB 要发送的字节总数
extern unsigned char data SMB_WR_NUM_C;			// SMB 已发送的字节数
extern unsigned char data SMB_RD_NUM_BAK;		// SMB 要接收的字节总数
extern unsigned char data SMB_RD_NUM_C;			// SMB 已接收的字节数
extern unsigned char BUFF_SMB_TR[128];			// SMB 发送缓冲区
extern unsigned char BUFF_SMB_RD[128];			// SMB 接收缓冲区
extern unsigned char data smbus_count;			// 从 IIC 接口芯片读写数据计时器

extern bit f_uart0_transed_now0; 
extern bit f_uart0_transed_now1;
extern bit f_uart0_transed_now2;
extern bit f_uart0_transed_now3;
extern bit f_uart0_transed_now4;

extern unsigned char ReceiveBuff0;

extern unsigned char trans_count;
extern unsigned char uart0_chanl_sel;										// 选择串口 0 相应的子通道
extern unsigned char xdata uart0_trans_buffer[8][uart0_trans_max];			// 串口 0 各子通道发送数据缓冲区
extern unsigned char xdata uart0_receive_buffer[8][uart0_received_max];		// 串口 0 各子通道接受数据缓冲区
extern unsigned char bdata flag0_uart0,flag1_uart0;

extern unsigned char phase_plan_no;
extern char bdata flag_eeprom0;
extern unsigned long result;							// ADC0 decimated value

extern void fun_smbus(void);
extern int load_config_data();
extern int save_control_mode_selection();
extern int get_detector_counter(detector_counter_t* counter);
#endif
