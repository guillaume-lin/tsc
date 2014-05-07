#include "isr_smbus.h"
//-----------------------------------------------------------------------------------
//Global VARIABLES
//-----------------------------------------------------------------------------------
unsigned char data COMMAND;						// Holds the slave address + R/W bit for
												// use in the SMBus ISR.

unsigned char data WORD;						// Holds data to be transmitted by the SMBus
												// OR data that has just been received.

unsigned char data ADDR_NUMBER;					// Used by ISR to check what data has just been
												// sent - High address byte, Low byte, or data byte
unsigned char data wr_eeprom_count;

unsigned char data HIGH_ADD,LOW_ADD;			// High & Low byte for EEPROM memory address
unsigned char data SMB_WR_NUM_BAK;				// SMB 要发送的字节总数
unsigned char data SMB_WR_NUM_C;				// SMB 已发送的字节数
unsigned char data SMB_RD_NUM_BAK;				// SMB 要接收的字节总数
unsigned char data SMB_RD_NUM_C;				// SMB 已接收的字节数
unsigned char BUFF_SMB_TR[128];					// SMB 发送缓冲区
unsigned char BUFF_SMB_RD[128];					// SMB 接收缓冲区
unsigned char data smbus_count;					// 从 IIC 接口芯片读写数据计时器
unsigned char step_all_flash;					// 某相位表在 FLASH 中保存的总步数

bit f_lcd;	

unsigned char bdata flag0_smbus; 
sbit SM_BUSY					= flag0_smbus^0;	// This bit is set when a send or receive
													// is started. It is cleared by the
													// ISR when the operation is finished.
sbit f_rtc_err					= flag0_smbus^1;	// 读 RTC 芯片出错标志位
sbit f_rd_smbus_ok				= flag0_smbus^3;	// smbus 读缓冲区有数据等待读出
sbit f_epprom_err				= flag0_smbus^4;	// 读 EEPROM 芯片出错标志位
sbit f_step_all_ok				= flag0_smbus^5;	// 某相位表在 FLASH 中保存的总步数已读出标志
sbit f_wr_eeprom_now			= flag0_smbus^7;	// 向 EEPROM 里写数据延时标志位
				

unsigned char code Dis_table[] = {0x7e,0x0c,0xda,0x9e,0xac,0xb6,0xf6,0x0e,0xfe,0xbe,	//0,1,2,3,4,5,6,7,8,9
				0xee,0xf4,0x72,0xdc,0xf2,0xe2,0xbe,0xec,0x60,0xe0,						//A,b,C,d,E,F,g,H,i,k
				0x70,0xc4,0x7e,0xea,0xc0,0xb6,0xf0,0x54,0x00,0xd0,						//L,n,O,P,r,S,t,u, ,c
				0x7c,0x6e,0xd4,0x80,0x10,0x7f,0x90,0xea,0xbc,0xaa,						//U,N,o,-,_,0.,=,C.,y,O
				0xba,0xd6,0x92,0x8c,0xd7} ;												//o_,-o,=-,-1,-o.

/*********************************************************************************************
*	原型：void smbus_isr(void) interrupt 7
*	功能: SMBUS(IIC) 中断子程序
*	说明：I2C 接口处理，MCU 始终都是作为主器件，从器件包括 EEPROM、RTC 及调试用的 LCD
*	参数：无
*	返回：无
*********************************************************************************************/
void smbus_isr(void) interrupt 7
{
unsigned char data i,j;
	i = SMB0STA;
	if (i == SMB_START) {                	
		SMB0DAT = (COMMAND & 0xFE);						// Load address of the slave to be accessed.
		STA = 0;										// Manually clear START bit
		}                      	
	else {
		if (i == SMB_RP_START) {             	
			SMB0DAT = COMMAND;							// COMMAND should hold slave address + R.
			STA = 0;
			}
		else {
			if (i == SMB_MTADDACK) SMB0DAT = HIGH_ADD;	// Load high byte of memory address to be written.
			else {
				if (i == SMB_MTADDNACK) { 
					STO = 1;
					STA = 1;
					}
				else {
					if (i == SMB_MTDBACK) { 
						switch (ADDR_NUMBER) {
							case 1:						// If ADDR_NUMBER=1, LOW_ADD was just sent.
								if (COMMAND & 0x01){	// If R/W=READ, sent repeated START.
									STO = 0;
									STA = 1;
									}
								else { 					// If R/W=WRITE, load byte to write.
									j = BUFF_SMB_TR[SMB_WR_NUM_C];
									if (f_lcd) j = Dis_table[j];
									SMB0DAT = j;
									SMB_WR_NUM_C++;
									if (SMB_WR_NUM_C == SMB_WR_NUM_BAK) ADDR_NUMBER--;
									}
								break;
							case 2:						// If ADDR_NUMBER=2, only HIGH_ADD
								SMB0DAT = LOW_ADD;		// has been sent.
								ADDR_NUMBER--;			// Decrement for next time around.
								break;
							default: 					// If ADDR_NUMBER=0, transfer is finished.
								STO = 1;
								SM_BUSY = 0;			// Free SMBus
								f_epprom_err = 0; 
								if (COMMAND == CHIP_EEPROM) {		// 若向 EEPROM 里写数据则要延时
									f_wr_eeprom_now = 1; 
									wr_eeprom_count = 0x00;
								}
							}
						}
					else {	
						if (i == SMB_MTDBNACK) { 
							STO = 1;
							STA = 1;
							}
						else {
							if (i == SMB_MTARBLOST) { 
								STO = 1;
								STA = 1;
								}
							else {
								if (i == SMB_MRADDACK) { 
									if (f_rd_eeprom_now) {
										if (rdt_num_bak == 0x01) AA = 0;
										else AA = 1;
										}
									else {
									
										if (SMB_RD_NUM_BAK == 0x01) {	//只准备接收一个字节，清 AA 位，收到该字节后发送NACK 等待接收数据
											AA = 0;						// NACK sent on acknowledge cycle.
											}
										else AA = 1;					//准备接收多个字节，置位 AA 位，收到该字节后发送ACK 等待接收数据
										}
									}
								else {
									if (i == SMB_MRADDNACK) {
										STO = 0;
										STA = 1;
										}
									else {
										if (i == SMB_MRDBACK) { 
											if (f_rd_eeprom_now) {
												*start_addr_ram = SMB0DAT;
												rdt_num++;
												start_addr_ram++;
												if ((rdt_num + 1) == rdt_num_bak) {
													AA = 0;
													}
												else AA = 1;
												}
											else {
											
												BUFF_SMB_RD[SMB_RD_NUM_C] = SMB0DAT;
												SMB_RD_NUM_C++;
												if ((SMB_RD_NUM_C + 1) == SMB_RD_NUM_BAK) {	//准备接收最后一个字节，清 AA 位，收到该字节后发送NACK 等待接收数据
													AA = 0;						// NACK sent on acknowledge cycle.
													}
												else AA = 1;
												}
											}
										else {
											if (i == SMB_MRDBNACK) {			// 接收最后一个字节 
												if (f_rd_eeprom_now) {
													f_eeprom_rd = 0;
													f_rd_eeprom_now = 0;
													if (f_step_all) {
														f_step_all = 0;
														f_step_all_ok = 1;
														step_all_flash = SMB0DAT;
														}
													else *start_addr_ram = SMB0DAT;
													}
												else {
													
													BUFF_SMB_RD[SMB_RD_NUM_C] = SMB0DAT;
													COMMAND = COMMAND & 0xfe;
													f_rd_smbus_ok = 1; 				// smbus 读缓冲区有数据等待读出
													}
												STO = 1;
												SM_BUSY = 0;				// Free SMBus
												}
											else { 
												STO = 1;					// Reset communication.
												SM_BUSY = 0;
												}
											}
										}
									}	
								}		
							}			
						}
					}
				}
			}
		}
	if (STO == 1) {
		f_lcd = 0;
		}
	SI=0;																	// clear SMBUS interrupt flag
	}

