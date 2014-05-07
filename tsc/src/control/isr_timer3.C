#include "isr_timer3.h"

/*********************************************************************************************
*	原型：void timer3_isr(void) interrupt 14
*	功能: 定时器 3 产生 0.5ms 的定时中断来启动 ADC0，同时产生 2.5ms、100ms 的系统时钟 
*	说明：无
*	参数：无
*	返回：无
*********************************************************************************************/
void timer3_isr(void) interrupt 14 
{
static unsigned char data SYStime_count,ii = 0x00;
unsigned char data i,j;
unsigned char xdata * data pp;
	EIE1 = EIE1 & 0xfb;					// 不允许ADC0 窗口比较中断
	EIE2 = EIE2 & 0xfd; 				// 不允许ADC0 转换结束产生的中断
	AD0INT = 0;							// clear adc0int interrupt flag
	AD0WINT = 0;						// clear adc0win interrupt flag
	SYStime_count++;
	if ((SYStime_count % 5) == 0x00) {
		SYS5ms = 1;
		AMX0SL++;
		switch (AMX0SL){
			case 0x01: {
				ADC0GT = ADC0GT_BAK1;
				ADC0LT = ADC0LT_BAK1;
				break;
				}
			case 0x02: {
				ADC0GT = ADC0GT_BAK2;
				ADC0LT = ADC0LT_BAK2;
				break;
				}
			case 0x03: {
				ADC0GT = ADC0GT_BAK3;
				ADC0LT = ADC0LT_BAK3;
				break;
				}
			case 0x04: {
				ADC0GT = ADC0GT_BAK4;
				ADC0LT = ADC0LT_BAK4;
				break;
				}
			case 0x05: {
				ADC0GT = ADC0GT_BAK5;
				ADC0LT = ADC0LT_BAK5;
				break;
				}
			case 0x06: {
				ADC0GT = ADC0GT_BAK6;
				ADC0LT = ADC0LT_BAK6;
				break;
				}
			case 0x07: {
				ADC0GT = ADC0GT_BAK7;
				ADC0LT = ADC0LT_BAK7;
				break;
				}
			default: {
				if (AMX0SL > 0x08) {
					AMX0SL = 0x00;							// 选择 Ain0 作为A/D输入
					ADC0CF = ADC0CF & 0xfe;					// 内部放大器增益为 1 （Page 55）
					ADC0CN = 0x85;							// A/D转换数据左对齐
					EIE2 = EIE2 & 0xfd; 					// 不允许ADC0 转换结束产生的中断
					ADC0GT = ADC0GT_BAK0;
					ADC0LT = ADC0LT_BAK0;
					
//1219					if (f_key2_int) f_manual_use = 1;		// 手动/自动选择开关闭合
//1219					else f_manual_use = 0;					// 手动/自动选择开关打开
//1219					f_key2_int = 0;
					
					}
				else {
					AMX0SL = 0x08;							// 选择温度传感器作为A/D输入
					ADC0CF = ADC0CF | 0x01;					// 内部放大器增益为 2 （Page 55）
					EIE1 = EIE1 & 0xfb;						// 不允许ADC0 窗口比较中断
					}                       				
				}                           				
			}                               				
		AD0INT = 0;											// clear adc interrupt flag
		AD0WINT = 0;
		}
	else {
		if ((SYStime_count % 5) == 0x01) {
			if (AMX0SL == 0x08) EIE2 = EIE2 | 0x02;			// 允许ADC0 转换结束产生的中断			
			else EIE1 = EIE1 | 0x04; 						// 允许ADC0 窗口比较中断
			}
		}
	
	if (((SYStime_count % 4) == 0x00) & !f_rd_lamp_input_now) {		// 定时2ms 读入灯状态（保存最近 10 次读入值）
		j = 0x00;
//		pp = ADDRESS_74HC244A;
		pp = ADDRESS_74HC244K;
		for (i=0x00;i<(MAX_LAMP_NUM - 1)/12 + 1;i++) {
			lamp_input_buffer[ii][j] = *pp;					// 
			j++;
			pp = pp + 0x200;
			lamp_input_buffer[ii][j] = *pp;					// 
			j++;
//			pp = pp + 0x200;
			pp = pp - 0x600;
			}
		ii++;
		if (ii == 10) ii = 0x00;
		}

	if ((SYStime_count % 50) == 0x00) SYS25ms = 1;
	
	if ((SYStime_count % 100) == 0x00) {
		SYS100ms = 1;										// 其实是 50 ms
		SYStime_count = 0x00;
		}
//	if (SYStime_count == 200) {
//		SYS200ms = 1;
//		SYStime_count = 0x00;
//		}
	TMR3CN = TMR3CN & 0x07;									// clear timer3 interrupt flag   
	}