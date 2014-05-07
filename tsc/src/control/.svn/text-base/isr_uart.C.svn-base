#include "isr_uart.h"

unsigned char uart0_chanl_sel;											// 选择串口 0 相应的子通道
unsigned char xdata uart0_trans_buffer[8][uart0_trans_max];				// 串口 0 各子通道发送数据缓冲区
unsigned char xdata uart0_receive_buffer[8][uart0_received_max];		// 串口 0 各子通道接受数据缓冲区

unsigned char ReceiveBuff0;
unsigned char bdata flag0_uart0,flag1_uart0;
sbit f_uart0_transed_now0		= flag0_uart0^0;						// 串口 0 子通道 1 有数据发送
sbit f_uart0_transed_now1 		= flag0_uart0^1; 						// 串口 0 子通道 2 有数据发送
sbit f_uart0_transed_now2 		= flag0_uart0^2; 						// 串口 0 子通道 3 有数据发送
sbit f_uart0_transed_now3 		= flag0_uart0^3; 						// 串口 0 子通道 4 有数据发送
sbit f_uart0_transed_now4 		= flag0_uart0^4; 						// 串口 0 子通道 5 有数据发送
sbit f_uart0_received_over0		= flag0_uart0^5;						// 串口 0 子通道 1 接收的数据溢出
sbit f_uart0_received_over1 	= flag0_uart0^6; 						// 串口 0 子通道 2 接收的数据溢出
sbit f_uart0_received_over2 	= flag0_uart0^7; 						// 串口 0 子通道 3 接收的数据溢出

sbit f_uart0_received_now0	 	= flag1_uart0^0; 						// 串口 0 子通道 1 正在接收数据
sbit f_uart0_received_now1	 	= flag1_uart0^1; 						// 串口 0 子通道 2 正在接收数据
sbit f_uart0_received_now2	 	= flag1_uart0^2; 						// 串口 0 子通道 3 正在接收数据
sbit f_uart0_received_now3	 	= flag1_uart0^3; 						// 串口 0 子通道 4 正在接收数据
sbit f_uart0_received_now4	 	= flag1_uart0^4; 						// 串口 0 子通道 5 正在接收数据
sbit f_uart0_received_over3 	= flag1_uart0^5; 						// 串口 0 子通道 4 接收的数据溢出
sbit f_uart0_received_over4 	= flag1_uart0^6; 						// 串口 0 子通道 5 接收的数据溢出
sbit f_chanl_no 				= flag1_uart0^7; 						// 该通道无数据发送

/*********************************************************************************************
*	原型：void uart0_isr(void) interrupt 4
*	功能: 串口 0 中断子程序 
*	说明：串口 0 可设定为单通道模式或多通道模式工作，多通道模式下可带 5 个子串口且各子串口波特率
*		  相同
*	参数：无
*	返回：无
*********************************************************************************************/
void uart0_isr(void) interrupt 4
{
unsigned char i,oo,ctemp1,ctemp2;
	if(TI0) {											// 发送完成标志位
#ifdef single_mode										// 串口扩展芯片在单通道模式下工作，使用子通道 1 占用的资源
		if(!is_out_empty(1)) {
			UART_CHANL = 0xc7 | uart0_chanl_sel;
			UART_CHANL = (UART_CHANL & 0xc7) | uart0_chanl_sel;
			SBUF0 = out_getchar(1);
			}

#else 													// 串口扩展芯片在多通道模式下工作
		if ((flag0_uart0 & 0x1f) != 0x00) {
			uart0_chanl_sel = uart0_chanl_sel + 0x08;                               
			if (uart0_chanl_sel > 0x30) uart0_chanl_sel = 0x08;

			ctemp1 = uart0_chanl_sel >> 3;
			ctemp2 = 0x01;
			for (i=0x01;i<ctemp1;i++) ctemp2 = ctemp2 << 1;
			if ((flag0_uart0 & ctemp2) != 0x00) {
				if (!is_out_empty(ctemp1)){
					UART_CHANL = 0xc7 | uart0_chanl_sel;
					UART_CHANL = (UART_CHANL & 0xc7) | uart0_chanl_sel;
					SBUF0 = out_getchar(ctemp1);
					}
				else {
					f_chanl_no = 1;
					flag0_uart0 = flag0_uart0 & !ctemp2;
					}
				}
			else f_chanl_no = 1;
			if (f_chanl_no) {
				UART_CHANL = UART_CHANL & 0xc7;
				SBUF0 = 0x00; 
				}
			}
		f_chanl_no = 0;
#endif
		TI0 = 0;
		}
	if(RI0){							// 接收完成标志位
#ifdef single_mode1						// 串口扩展芯片在单通道模式下工作，使用子通道 1 占用的资源
		in_putchar(1,SBUF0);
#else 									// 串口扩展芯片在多通道模式下工作
		oo = P4;
		oo = oo&0x07;
		if ((oo<=5) & (oo>=1)) in_putchar(oo,SBUF0);
#endif
		RI0 = 0;
		}
	}
	
/*********************************************************************************************
*	原型：void uart1_isr(void) interrupt 20
*	功能: 串口 1 中断子程序 
*	说明：用作 485 通讯
*	参数：无
*	返回：无
*********************************************************************************************/
void uart1_isr(void) interrupt 20
{
	if (SCON1 & 0x02) {						// TI1 = 1
		if(g_out_read_p[5] != g_out_write_p[5]) {
			SBUF1 = uart0_trans_buffer[5][g_out_read_p[5]++];
			if(g_out_read_p[5] == uart0_trans_max) g_out_read_p[5] = 0;		
			}
		else f_uart1_using = 0;
		SCON1 = SCON1 & 0xfd;				// TI1 = 0
		}
	if (SCON1 & 0x01) {						// RI1 = 1
		uart0_receive_buffer[5][g_in_write_p[5]++] = SBUF1;
		if(g_in_write_p[5] == uart0_trans_max) g_in_write_p[5] = 0;
		SCON1 = SCON1 & 0xfe;				// RI1 = 0
		}
	}
