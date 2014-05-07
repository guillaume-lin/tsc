#include "isr_int0.h"

/*********************************************************************************************
*	原型：void int0_isr(void) interrupt 0 using 1
*	功能: 外部中断 0 子程序，接收由 PC104 发送过来的数据
*	说明：无
*	参数：无
*	返回：无
*********************************************************************************************/
void int0_isr(void) interrupt 0 using 1
{
bit f_receiveed;
unsigned char buffer,i;
unsigned char xdata * data pchar;
	pchar = ADDRESS_PC_STATUS;
	buffer = *pchar;										
	f_receiveed = 0;
	if ((buffer & 0x04) != 0x00) {							// CPLD 端口 0
		pchar = ADDRESS_PC_RD0A;
		uart0_receive_buffer[6][g_in_write_p[6]++] = *pchar;
		if(g_in_write_p[6] == uart0_trans_max)	g_in_write_p[6] = 0;
		
		pchar = ADDRESS_PC_RD1A;
		i = *pchar;
		f_receiveed = 1;
		}
	if ((buffer & 0x08) != 0x00) {							// CPLD 端口 1
		pchar = ADDRESS_PC_RD0B;
		uart0_receive_buffer[7][g_in_write_p[7]++] = *pchar;;
		if(g_in_write_p[7] == uart0_trans_max)	g_in_write_p[7] = 0;

		pchar = ADDRESS_PC_RD1B;
		i = *pchar;
		f_receiveed = 1;
		}
	if (!f_receiveed) {
		pchar = ADDRESS_PC_RD0A;
		i = *pchar;
		pchar = ADDRESS_PC_RD1A;
		i = *pchar;
		
		pchar = ADDRESS_PC_RD0B;
		i = *pchar;
		pchar = ADDRESS_PC_RD1B;
		i = *pchar;
		}
	}
/*********************************************************************************************
*	原型：void int6_isr(void) interrupt 18 using 1
*	功能: 外部中断 6 子程序，向 PC104 发送数据
*	说明：无
*	参数：无
*	返回：无
*********************************************************************************************/
void int6_isr(void) interrupt 18 using 2
{

unsigned char ctemp;
unsigned char xdata * data pchar;  
	P3IF = P3IF & 0xbf;
	pchar = ADDRESS_PC_STATUS;
	ctemp = *pchar;
	f_cpld_trans_ed = 0;
	if ((ctemp & 0x01) == 0x01) {
		if(g_out_read_p[6] != g_out_write_p[6]) {
			pchar = ADDRESS_PC_WR0;
			cpld_trans_bak = uart0_trans_buffer[6][g_out_read_p[6]++];
			*pchar = cpld_trans_bak;
			if(g_out_read_p[6] == uart0_trans_max)	g_out_read_p[6] = 0;
			f_cpld_trans_ed = 1;
			f_cpld_trans_num = 1;

			}		
		else f_cpld_chanl0_using = 0;
		cpld_count = 0x00;
//		return;
		}
	if (!f_cpld_trans_ed && ((ctemp & 0x02) == 0x02)) {
		if(g_out_read_p[7] != g_out_write_p[7]) {
			pchar = ADDRESS_PC_WR1;
			f_cpld_trans_num = 0;
			cpld_trans_bak = uart0_trans_buffer[7][g_out_read_p[7]++];
			*pchar = cpld_trans_bak;
			if(g_out_read_p[7] == uart0_trans_max)	g_out_read_p[7] = 0;
			}		
		else f_cpld_chanl1_using = 0;
		cpld_count = 0x00;
		}
	}