#include "isr_adc0.h"

char bdata flag_adc0win;            	
sbit f_key0_int 	= flag_adc0win^0;		// 键盘 0 由按键按下标志位
sbit f_key1_int 	= flag_adc0win^1; 		// 键盘 1 由按键按下标志位
sbit f_key2_int 	= flag_adc0win^2; 		// 键盘 2 由按键按下标志位
sbit f_out0_int 	= flag_adc0win^3;		// 信号机柜门打开标志位 	
sbit f_out1_int 	= flag_adc0win^4; 		// 信号机柜震动检测标志位
sbit f_out2_int 	= flag_adc0win^5;		//  	
sbit f_out3_int 	= flag_adc0win^6; 		// 
sbit f_key3_int 	= flag_adc0win^7; 		// 键盘 3 由按键按下标志位


bit f_temperature;							// 获得一次温度转换值标志位  
bit f_adc0win; 								// ADC0 窗口比较
                                    	
unsigned char key0_adc0,key1_adc0,key2_adc0,key3_adc0;                    	
unsigned long result;							// ADC0 decimated value


/*********************************************************************************************
*	原型：void adc0_isr(void) interrupt 15 using 1
*	功能: ADC0 中断子程序
*	说明：温度传感器
*	参数：无
*	返回：无
*********************************************************************************************/
void adc0_isr(void) interrupt 15 using 3
{
static unsigned char int_dec = INT_DEC;	// integrate/decimate counter
										// we post a new result when
										// int_dec = 0
static unsigned long accumulator=0L;	// here's where we integrate the adc samples  

	AD0WINT = 0;
	if (AMX0SL == 0x08) {			
		accumulator += ADC0;			// read adc value and add to running total
		int_dec--;						// update decimation counter
		if (int_dec == 0) {				// if zero, then post result
			f_temperature = 1;       
			int_dec = INT_DEC;			// reset counter
			result = accumulator >> 5;
			accumulator = 0L;			// reset accumulator
			}
		}
	AD0INT = 0;							// clear adc interrupt flag
	}
	
/*********************************************************************************************
*	原型：void adc0win_isr(void) interrupt 8 using 1
*	功能: ADC0 窗口比较中断子程序
*	说明：以后按下的键为处理按键
*	参数：无
*	返回：无
*********************************************************************************************/
void adc0win_isr(void)interrupt 8 				// using 1
{
	switch (AMX0SL){
		case 0x00: 
			f_key0_int = 1;
			key0_adc0 = ADC0H;					// 保存键盘 0 按键 AD 值
			break;
		case 0x01: 
			f_key1_int = 1;
			key1_adc0 = ADC0H; 					// 保存键盘 1 按键 AD 值
			break;
		case 0x02: 								// 手动/自动选择
			f_key2_int = 1;
   			key2_adc0 = ADC0H; 					// 保存键盘 2 按键 AD 值
   			break;
		case 0x03: f_out0_int = 1;break;
		case 0x04: f_out1_int = 1;break;
		case 0x05: f_out2_int = 1;break;
		case 0x06: f_out3_int = 1;break;
		case 0x07: 								// 步进按键
			f_key3_int = 1;
			key3_adc0 = ADC0H; 					// 保存键盘 2 按键 AD 值
			break;
		default: f_adc0win = 0;break;
		}
	AD0WINT = 0;								// clear adc0win interrupt flag
	AD0INT = 0;
	}