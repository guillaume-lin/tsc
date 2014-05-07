#ifndef INCLUDED_INITIALIZE_MCU_C
#define INCLUDED_INITIALIZE_MCU_C

//#include "c8051f020.h"
//#include "io.h"


unsigned char i;
unsigned char xdata * data pp;

//振荡器初始化
	OSCXCN = 0x67;					//晶体振荡器方式，振荡器频率 f > 6.7MHz （Page 121）
	while ((OSCXCN & 0x80) != 0x80);//直到晶体振荡器正在运行并且工作稳定

	OSCICN = 0x88;					//选择外部振荡器作为系统时钟，内部振荡器禁止，允许时钟丢失检测器（检测到时钟丢失时间大于100 微秒时将触发复位）

//复位源	
//	RSTSRC

	WDTCN = 0xde;					// disable watchdog timer
	WDTCN = 0xad;           		
                            		
//电压基准初始化            		
	REF0CN = 0x07;					// ADC0 电压基准取自VREF0 引脚，内部温度传感器、偏压发生器、电压基准缓冲器工作（Page 82）
         
                            		
//端口初始化                		    
	XBR0 = 0x0d;					// 分配串口0(2个输出)、IIC(2个输出)及PCA0(1个输出)到端口引脚（Page 152） 
	XBR1 = 0x14;					// 分配中断0及中断1到端口引脚（Page 153）
	XBR2 = 0x84;					// 分配串口1到端口引脚，弱上拉全局禁止（Page 154） 
	P1MDIN = 0xff;					// 端口P1引脚被配置为数字输入输出方式（Page 156）
	XBR2 = XBR2 | 0x40;				// 使能交叉开关
	                        			
	P0MDOUT = 0x51;					// 将UART0 的TX 引脚TX0 P0.0、UART1 的TX 引脚TX1 P0.4、PCA0 的输出引脚CEX0 P0.6，设置为推挽方式（Page 155）
	P0 = P0 | 0xae;					
	P1MDOUT = 0xc0;					// 将P1.6、P1.7 设置为推挽方式，其他设置为 OC 输出方式（Page 157）
//	P1 = P1 | 0x3f;         		// 将 P1.5 -- P1.0 设置为数字输入
	P1 = 0x3f;
	P2MDOUT = 0x7f;					// 将P2.6、P2.5、P2.4、P2.3、P2.2、P2.1、P2.0 设置为推挽方式（Page 157）
	P2 = P2 | 0x80;					// 将 P2.7 设置为数字输入         			
	P3MDOUT = 0xcf;					// 将P3.7、P3.6、P3.3、P3.2、P3.1、P3.0 设置为推挽方式（Page 158）
//	P3 = P3 | 0x30;         	
	P3 = 0x30;
	
//	chip_uart_rst = 0;				// 给串口扩展芯片送复位信号 
//	chip_uart_rst = 1;                        	
	
	P74OUT = 0x3f;   
	P7 = 0xff;        
//	P74OUT = 0xff;					// 将P4、P5、P6、P7 设置为推挽方式（Page 161）
	
//外部存储器接口配置初始化  		
//	EMI0CN = 0x??					//XRAM 页选择EMI0CN
	EMI0CF = 0x3c;			//0x3c;		//0x38;					//存储器接口配置在 P4-P7，工作在非复用、带块选择的分片方式（Page 130）

	EA = 0;							//禁止所有中断
//	FLSCL = FLSCL | 0X01;			//允许由用户软件写/擦除FLASH
//	PSCTL = PSCTL | 0x02;			//允许FLASH 扇区擦除（Page 123）
//	PSCTL = PSCTL | 0x01;			//允许FLASH 写（Page 123）
//	                            	
//                                	
//	PSCTL = PSCTL & 0xfd;			//禁止FLASH 扇区擦除（Page 123）
//	PSCTL = PSCTL & 0xfe;			//禁止FLASH 写（Page 123）
//	FLSCL = FLSCL & 0xfe;			//禁止由用户软件写/擦除FLASH（Page 123）
//	
//	PSCTL = PSCTL | 0x04;			//从用户软件访问FLASH 时，将访问128 字节的临时存储器扇区（EEPROM）（Page 123）
	EMI0TC = 0x1c;					// 0x7d;	//3c;	//0c;	//外部存储器接口的时序参数（Page 135）

//可编程计数器阵列（PCA0）初始化（仅分配一个输出 CEX0 到引脚 P0.4）
	PCA0CN = PCA0CN | 0x40;			//允许PCA0 计数器定时器（Page 233）
	PCA0MD = 0x80;					//当系统控制器处于等待方式时，PCA0 停止工作；PCA0 计数器的时基为系统时钟的 12 分频；禁止CF 中断（Page 234）
	PCA0CPM0 = 0x46;				//允许频率输出方式，禁止 CCF0 中断（Page 235）
	PCA0CPL0 = 0x5c;				//PCA0 捕捉模块高字节（Page 236）
	PCA0CPH0 = 0x5c;				//PCA0 捕捉模块高字节（Page 236）
	
//ADC0 初始化，按键 1 由 Ain0.0 输入,按键 2 由 Ain0.1 输入
	ADC0CF = 0x79;					//CLKSAR0 为 2.21184MHz，内部放大器增益为 2 （Page 55）
	ADC0CN = 0x85;					//ADC0 允许一直跟踪方式、定时器 3 溢出启动ADC0 转换、ADC0H:ADC0L 寄存器数据左对齐
//	ADC0CN = 0xc4;					//ADC0 允许、定义跟踪方式、定时器 3 溢出启动ADC0 转换、ADC0H:ADC0L 寄存器数据右对齐
	AMX0CF = 0x00;					//全部输入（Ain0.0 - Ain0.7）工作在单端方式（Page 53）
	                        		
	AMX0SL = 0x00;					//AMUX 选择 Ain0.0（键盘 0 输入端）（Page 54）
	AMX0SL = 0x01;					//AMUX 选择 Ain0.1（键盘 1 输入端）（Page 54）
	AMX0SL = 0x02;					//AMUX 选择 Ain0.2（键盘 2 输入端）（Page 54）
	AMX0SL = 0x08;					//AMUX 选择 温度传感器（Page 54）
	                        		
	ADC0LTH = 0x70;					//（Page 60）
	ADC0LTL = 0x00;         		
	ADC0GTH = 0x10;         		
	ADC0GTL = 0x00;					//按键采用 A/D 转换方式，当A/D值在一定范围内时，系统产生中断再有软件判断是某个按键
	
//定时器初始化（系统时钟为：22.1184MHz）
	CKCON = 0x00;					//定时器使用系统时钟的 12 分频（Page 202）
	TMOD = 0x11;					//定时器 0、1 工作在方式 1 （16 位计数器/定时器）（Page 207）
//	TMOD = 0x21;					//定时器 0 工作在方式 1 （16 位计数器/定时器），定时器 0 工作在方式 3 （作为 UART0 的波特率发生器）（Page 207）
	                        		
	T2CON = 0x30;					//定时器 2 作为 UART0 的波特率发生器	230400	115200	76800	38400	19200	9600	7200	4800	2400	1200（Page 213）
	RCAP2L = 0xfa;					// 定时器 2 捕捉寄存器低字节			0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
	RCAP2H = 0xff;					// 定时器 2 捕捉寄存器高字节			0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xfe	0xfd
	TL2 = RCAP2L;					//定时器 2 的低字节
	TH2 = RCAP2H;					//定时器 2 的低字节（Page 214）
	TR2 = 1;						//启动定时器 2 （在波特率发生器方式，定时器 2 的时基信号为：系统时钟/2）（Page 207）
	                        		
	TMR3CN = 0x00;					//定时器 3 使用系统时钟的 12 分频（使用定时器 3 定时 0.5ms 启动ADC0 转换，及产生 5ms 作为系统循环周期）（Page 216）
	TMR3RLL = 0x65;					//定时器 3 重载寄存器的低字节
	TMR3RLH = 0xfc;					//定时器 3 重载寄存器的高字节
	TMR3L = TMR3RLL;				//定时器 3 的低字节
	TMR3H = TMR3RLH;				//定时器 3 的高字节（Page 217）
	TMR3CN = TMR3CN | 0x04;			//启动定时器 3 （Page 216）
	
	T4CON = 0x30;					//定时器 4 作为 UART1 的波特率发生器	19200	9600	4800	2400	1200（Page 222）
	RCAP4L = 0xbf;					//定时器 4 捕捉寄存器低字节				0xdc	0xb8	0x70	0xe0	0xc0
	RCAP4H = 0xfd;					//定时器 4 捕捉寄存器高字节				0xff	0xff	0xff	0xfe	0xfd
	TL4 = RCAP4L;					//定时器 4 的低字节
	TH4 = RCAP4H;					//定时器 4 的低字节（Page 223）
	T4CON = T4CON | 0x04;			//启动定时器 4 （在波特率发生器方式，定时器 4 的时基信号为：系统时钟/2）（Page 222）
	
//中断初始化
	IT0 = 1;						//INT0 为边沿触发
	IT1 = 1;						//INT1 为边沿触发（Page 105）
	EX0 = 1;						//允许INT0 中断
//	EX1 = 1;						//允许INT1 中断
	ES0 = 1;						//允许UART0中断
//	ET0 = 1;						//允许定时器 0 中断
	PX0 = 1;						//外部中断 0 为高优先级
	EA = 1;							//允许中断（Page 123）
	
	EIE1 = 0x06;					//允许ADC0 窗口比较、SMBUS 中断（Page 107）
	EIE2 = 0x01;					//允许UART1 中断，允许定时器 3 中断（Page 107）
//	EIE2 = 0x43;					//允许UART1 中断，允许定时器 3 中断，允许ADC0 转换结束产生的中断（选择温度传感器时）
	                        		
//串口初始化                		
	PCON = 0x00;					//电源控制寄存器，允许读/写 UART 方式设置位SM2-SM0（同 UART 帧错误和传输错误检测位位置相同）（Page 112）
	SCON0 = 0x50;					//方式 1 （8 位UART 可变波特率），UART0 接收允许（Page 190）
	SCON1 = 0x50;					//方式 1 （8 位UART 可变波特率），UART1 接收允许（Page 199）
	                        		
	PCON = PCON | 0x48;				//电源控制寄存器，允许读/写帧错误FE0、RX 溢出RXOV0 和TX 冲突TXCOL0 状态位（同 UART 方式设置位SM2-SM0位置相同）（Page 112）
	
//IIC初始化 SMB0CN 中的 TOE 不能置为 1，否则将占有定时器 3，以检测 SCL 低电平超时（Page 167，171）
	ENSMB = 1;						//允许SMBus（IIC）（Page 171）
	SMB0CR = 0x92;					//时钟频率预设值（系统时钟为：22.1184MHz时，SCL 的速率为 100KHz 左右）（Page 172）
	SMB0ADR = 0xa0;					//地址寄存器，位7-1：SMBus0 从地址（Page 173）
//	SMB0STA = ;						//SMBus 状态寄存器（Page 173）
//	SMB0DAT = ;						//数据寄存器


// RTC 初始化
	while (SM_BUSY);									// Wait for SMBus to be free.
	BUFF_SMB_TR[0] = 0x00;								// 清停止更新位
	SM_Send(CHIP_RTC,0x000c,0x01,0);		
	while (SM_BUSY);
	SM_Receive(CHIP_RTC,0x0001,0x01,0);
	while (SM_BUSY);									// Wait for SMBus to be free.
	BUFF_SMB_TR[0] = BUFF_SMB_RD[0] & 0x7f;				// 清停止位
	SM_Send(CHIP_RTC,0x0001,0x01,0);
// RTC 初始化

// LCD 初始化
	while (SM_BUSY);									// Wait for SMBus to be free.
	BUFF_SMB_TR[0] = 0x00;								//
	SM_Send(SLAVE_LCD,0xc8e0,0x01,1);
// LCD 初始化

// 82C55 初始化
	wait_for_100ms();
	wait_for_100ms();
	wait_for_100ms(); 
	wait_for_100ms(); 
	wait_for_100ms(); 									// 82C55 初始化之前的延时
	for (i=0x00;i<4;i++) {								
		pp = ADDRESS_82C55A_D + i * 0x200;								
		*pp = 0x90;										// A 口为输入口，B、C 口为输出口，都工作在方式 0
		pp = ADDRESS_82C55A_C + i * 0x200;
		*pp = 0xff;
		pp = ADDRESS_82C55A_B + i * 0x200;
		*pp = 0xff;	
	}
// 82C55 初始化

#endif
