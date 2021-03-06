#include "vm.h"
#include "mcu.h"
#include "kernel.h"
#include "error_code.h"
#include "datastruct.h"

char bdata flag_key0;
sbit f_key0_err 					= flag_key0^0;		// 出现中间值，有可能出错(如组合键)	
sbit f_key1_err 					= flag_key0^1;		// 出现中间值，有可能出错(如组合键)	
sbit f_manual 						= flag_key0^2;		// 手动模式（该标志位不参与控制，只为以后显示用）
sbit f_key0_ed 						= flag_key0^3;		// 模拟输入 0 一次有效输入
sbit f_key1_ed 						= flag_key0^4;		// 模拟输入 1 一次有效输入
sbit f_key2_ed 						= flag_key0^5;		// 模拟输入 2 一次有效输入
sbit f_key3_ed						= flag_key0^6;		// 模拟输入 7 一次有效输入
sbit f_step 						  = flag_key0^7;		// 有步进控制键按下

bit f_key,f_key_now,f_buzz,flick_mark,f_auto_buzz;
bit SYS5ms,SYS25ms,SYS100ms,SYS200ms,f_rd_lamp_input_now;
bit f_uart1_using,f_cpld_chanl0_using,f_cpld_chanl1_using;

bit f_key0_int_bak,f_key1_int_bak,f_key2_int_bak,f_key3_int_bak;

bit f_cpld_trans_ed,f_cpld_trans_num;
bit f_manual_auto;										// 手动/自动转换一次有效

bit f_manual_step; 										// 遥控器上直接放行某相位按下标志
bit f_return_actuate_control;							// 从手动返回感应控制标志

bit f_run_mode,f_lamp_yellow_err,f_lamp_green_or_red_err,f_lamp_err,f_boot,f_key_val,f_mode_switch;

bit f_1s;
unsigned char delay_1s;
// unsigned char lamp_err_delay;

unsigned char TELECONTROL_BAK;							// 遥控器输入备份
unsigned char key_val0,key_val1,key_val2,key_val3;
unsigned char key_val,key_val_temp,key_time,key_buf[4],key_count;
unsigned char key0_val,key1_val,key0_val_bak,key1_val_bak,buzz_delay;

unsigned char pw_count;
unsigned char data comm_baud;
unsigned int g_in_read_p[8];
unsigned int g_in_write_p[8];
unsigned int g_out_read_p[8];
unsigned int g_out_write_p[8];

unsigned int tempture,led_bg_count;

unsigned char data flick_count;													// 灯闪计时，调整该变量值可改变灯闪频率 
unsigned char lamp_output_buffer[2*((MAX_LAMP_NUM - 1)/12 + 1)];				// 交通灯输出缓冲
unsigned char lamp_output_buffer1[2*((MAX_LAMP_NUM - 1)/12 + 1)];				// 交通灯输出 
unsigned char lamp_flick_buffer[2*((MAX_LAMP_NUM - 1)/12 + 1)];					// 交通灯闪光输出缓冲 
unsigned char lamp_input_buffer[11][2*((MAX_LAMP_NUM - 1)/12 + 1)];				// 交通灯状态输入缓冲（第0--6行保存最近7次读入值），第8行存放
		// 各灯的实际状态（由于实行的是半波(10ms)检测, 3ms 一次），通过该行的数据与送显的数据比较，来判断灯的好坏，并进行绿冲突检测

unsigned char detector_err[4],detector_flag[4],read_detector_time,detector_time;
unsigned char detector_count_bak0[MAX_DETECTOR_COUNT];

unsigned char cpld_count,cpld_trans_bak;

unsigned char lamp_health_status[((MAX_LAMP_NUM-1)/8)+1];

unsigned char manual_set_stage;							// 遥控器上直接放行某相位
unsigned char manual_m_control_mode_bak;				// 保存手动前的控制模式
// 090407
unsigned char detector_status_bak[4];
unsigned char detector_counter[4][8];
// 090407


void scankey(void);
void decode_key(void); 
void init_gm8125(void);
void refrash_lamp(void);
void out_init(int port);
int wait_for_100ms(void);
void judge_detector(void);
void conflict_detect(void);
int is_out_empty(int port);
void Trans_frame_ack(void);
void get_lamp_health_status(void);
void delay_200us(unsigned char m);
unsigned char out_getchar(int port);
void front_panel_dis(unsigned char ch);
int write_comm(int fd,byte* out_buffer, int size);
unsigned int cal_crc(unsigned char *ptr, unsigned char len);

extern int is_term_connect_to_yellow(int terminal);
extern void display_status(unsigned char ctemp1,unsigned char ctemp2);
extern void SM_Receive(unsigned char chip_select, unsigned int byte_address,unsigned char rd_count,bit f_addr);
extern void SM_Send(unsigned char chip_select, unsigned int byte_address, unsigned char wr_count,bit f_addr);

unsigned char code key0_table[]={0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40};
unsigned char code scan_table[]={0xfd,0xfb};
//unsigned char code key_table[]={0x0a,0x0f,0x0c,0x08,0x06,0x04,0x02,0x00,0x0b,0x0e,0x0d,0x09,0x07,0x05,0x03,0x01};
unsigned char code key_table[]={0x04,0x0e,0x0c,0x0a,0x07,0x05,0x02,0x0d,0x08,0x0f,0x10,0x0b,0x09,0x06,0x03,0x01};
unsigned int code crc_ta[16]={ 	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
								0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
								};						// CRC 余式表 

/*********************************************************************************************
*	原型：int initialize(void)
*	功能: 灯色输出控制（刷新）子程序
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
int initialize(void)
{
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
//	P0 = P0 | 0xae;	
	P0 = 0xae;				
	P1MDOUT = 0xc8;					// 将 P1.3、P1.6、P1.7 设置为推挽方式，其他设置为 OC 输出方式（Page 157）
//	P1 = P1 | 0x3f;         		// 将 P1.0、P1.1、P1.2、P1.4、P1.5 设置为数字输入
	P1 = 0x3f;
	P2MDOUT = 0xff;					// 将 P2.7、P2.6、P2.5、P2.4、P2.3、P2.2、P2.1、P2.0 设置为推挽方式（Page 157）
	P2 = 0x40;
//	P2 = P2 | 0x80;					// 将 P2.7 设置为数字输入         			
	P3MDOUT = 0x8f;					// 将P3.7、P3.3、P3.2、P3.1、P3.0 设置为推挽方式（Page 158）
//	P3 = P3 | 0x30;         	
	P3 = 0x30;
	
	P3IF = 0x04;
	
	chip_uart_rst = 0;				// 给串口扩展芯片送复位信号 
	chip_uart_rst = 1;				// 给串口扩展芯片送复位信号 
	P74OUT = 0x3e;   				// 将 P4 的高四位、P5、P6设置为推挽方式，P4 的低四位及 P7 设置为 OC 输出方式（Page 161）
	P7 = 0xff;        
	
//外部存储器接口配置初始化  		
//	EMI0CN = 0x??					//XRAM 页选择EMI0CN
	EMI0CF = 0x38;			//0x3c;		//0x38;					//存储器接口配置在 P4-P7，工作在非复用、带块选择的分片方式（Page 130）

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
	EMI0TC = 0x2c;		//1c;		//0x20			// 0x7d;	//3c;	//0c;	//外部存储器接口的时序参数（Page 135）

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
//	AMX0SL = 0x01;					//AMUX 选择 Ain0.1（键盘 1 输入端）（Page 54）
//	AMX0SL = 0x02;					//AMUX 选择 Ain0.2（键盘 2 输入端）（Page 54）
//	AMX0SL = 0x08;					//AMUX 选择 温度传感器（Page 54）
	                        		
	ADC0LTH = 0x70;					//（Page 60）
	ADC0LTL = 0x00;         		
	ADC0GTH = 0x20;         		
	ADC0GTL = 0x00;					//按键采用 A/D 转换方式，当A/D值在一定范围内时，系统产生中断再有软件判断是某个按键
	
//定时器初始化（系统时钟为：22.1184MHz）
	CKCON = 0x00;					//定时器使用系统时钟的 12 分频（Page 202）
	TMOD = 0x11;					//定时器 0、1 工作在方式 1 （16 位计数器/定时器）（Page 207）
//	TMOD = 0x21;					//定时器 0 工作在方式 1 （16 位计数器/定时器），定时器 0 工作在方式 3 （作为 UART0 的波特率发生器）（Page 207）
	                        		
	T2CON = 0x30;					//定时器 2 作为 UART0 的波特率发生器	230400	115200	76800	38400	19200	9600	7200	4800	2400	1200（Page 213）
	RCAP2L = 0xa0;					// 定时器 2 捕捉寄存器低字节			0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
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
	
	pp = ADDRESS_PC_WR2;
	*pp = 0x00;
	
	P3IF = P3IF & 0xbf;
	
//中断初始化 
//W	IT0 = 1;						//INT0 为边沿触发
	IT1 = 1;						//INT1 为边沿触发（Page 105）
	EX0 = 1;						//允许INT0 中断
//	EX1 = 1;						//允许INT1 中断
	ES0 = 1;						//允许UART0中断
//	ET0 = 1;						//允许定时器 0 中断
	PX0 = 1;						//外部中断 0 为高优先级
//	EA = 1;							//允许中断（Page 123）
	EIP2 = 0x11;					// 外部中断 6 与定时器 3 中断为高优先级
	
	EIE1 = 0x06;					//允许ADC0 窗口比较、SMBUS 中断（Page 107）
	EIE2 = 0x11;					//允许外部中断 6 中断，允许定时器 3 中断（Page 107）
//	EIE2 = 0x43;					//允许UART1 中断，允许定时器 3 中断，允许ADC0 转换结束产生的中断（选择温度传感器时）
	                        		
//串口初始化                		
	PCON = 0x00;					//电源控制寄存器，允许读/写 UART 方式设置位SM2-SM0（同 UART 帧错误和传输错误检测位位置相同）（Page 112）
	SCON0 = 0x70;					//方式 1 （8 位UART 可变波特率），UART0 接收允许（Page 190）
	SCON1 = 0x70;					//方式 1 （8 位UART 可变波特率），UART1 接收允许（Page 199）

//IIC初始化 SMB0CN 中的 TOE 不能置为 1，否则将占有定时器 3，以检测 SCL 低电平超时（Page 167，171）
	ENSMB = 1;						//允许SMBus（IIC）（Page 171）
	SMB0CR = 0x92;					//时钟频率预设值（系统时钟为：22.1184MHz时，SCL 的速率为 100KHz 左右）（Page 172）
	SMB0ADR = 0xa0;					//地址寄存器，位7-1：SMBus0 从地址（Page 173）
//	SMB0STA = ;						//SMBus 状态寄存器（Page 173）
//	SMB0DAT = ;						//数据寄存器

// 关闭所有的灯
	pp = ADDRESS_74HC573A;
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/12 + 1;i++) {
		*pp=0xff;					// 低电平灯点亮
		pp = pp + 0x200;
		*pp=0xff;					// 低电平灯点亮
		pp = pp + 0x200;
		}
// 关闭所有的灯

// 外部 RAM 初始化
	for (pp=0x00;pp<0x8000;pp++) {
		*pp = 0xaa;
		_nop_();
		comm_baud = *pp;
		while (comm_baud != 0xaa) {
			delay_200us(250); 
			delay_200us(250);
			buzz = !buzz;
			};
		*pp = 0x00;
		}
// 外部 RAM 初始化

	EA = 1;							//允许中断（Page 123）

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
//	while (SM_BUSY);									// Wait for SMBus to be free.
//	BUFF_SMB_TR[0] = 0x00;								//
//	SM_Send(SLAVE_LCD,0xc8e0,0x01,1);
//	while (SM_BUSY);									// Wait for SMBus to be free.
// LCD 初始化

	load_config_data();

	wait_for_100ms();
	wait_for_100ms();									// 82C55 与 gm8125 初始化之前的延时

// gm8125 初始化	
	chip_uart_rst = 1;
	delay_200us(50); 
	init_gm8125();
// gm8125 初始化
	
// 82C55 初始化				
	for (i=0x00;i<4;i++) {								
		pp = ADDRESS_82C55A_D + i * 0x200;								
		*pp = 0x90;										// A 口为输入口，B、C 口为输出口，都工作在方式 0
		pp = ADDRESS_82C55A_C + i * 0x200;
		*pp = 0xff;
		pp = ADDRESS_82C55A_B + i * 0x200;
		*pp = 0xff;	
		}
// 82C55 初始化
	// 开机后，默认以保存的设定运行
	f_run_mode = 0;										
	return 0;	
}

/*********************************************************************************************
*	原型：void refrash_lamp(void)
*	功能: 灯色输出控制（刷新）子程序
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
void refrash_lamp(void)
{
unsigned char i,j;
unsigned char xdata *pp;
	flick_count++;
//061013	if (flick_count >= 0x06) {						// 灯闪计时，调整该变量值可改变灯闪频率
	if (flick_count >= 0x04) {								// 灯闪计时，调整该变量值可改变灯闪频率
		flick_count = 0x00;
		flick_mark = !flick_mark;	
		}
	for(i=0;i<2*((MAX_LAMP_NUM - 1)/12 + 1);i++){
		if(flick_mark) lamp_output_buffer1[i]=(lamp_flick_buffer[i]) | lamp_output_buffer[i]; //亮
		else lamp_output_buffer1[i] = lamp_output_buffer[i]; //灭
		}
	
	j = 0x00;
//0914	pp = ADDRESS_74HC573A;
	pp = ADDRESS_74HC573K;
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/12 + 1;i++) {
		*pp=~lamp_output_buffer1[j];					// 低电平灯点亮
		j++;
		pp = pp + 0x200;
		*pp=~lamp_output_buffer1[j];					// 低电平灯点亮
		j++;
//0914		pp = pp + 0x200;
		pp = pp - 0x600;
	}
}
/*********************************************************************************************
*	原型：int get_run_mode()
*	功能: 获取当前的运行模式
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
int get_run_mode()
{
	return f_run_mode;
}
/*********************************************************************************************
*	原型：int get_run_mode()
*	功能: 获取当前的运行模式
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
int clear_run_mode()
{
	 f_run_mode=0;
}
/*********************************************************************************************
*	原型：int is_detect_health_flag()
*	功能: 是否检测故障的标志位
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
int is_detect_health_flag()
{
	return f_key_val;
}
/*********************************************************************************************
*	原型：int get_temperature()
*	功能: 获取温度，单位为0.1摄氏度
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
int get_temperature()
{
	return tempture;
	}

/*********************************************************************************************
*	原型：int is_door_illegal_open()
*	功能: 门是否非法打开
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
int is_door_illegal_open()
{
	if (f_out0_int) {
		f_out0_int = 0;
		f_buzz = 1;
		led_bg = 1;					// 点亮 LCD 背光
		led_bg_count = 0;
		return 1;
		}
	return 0;
	}

/*********************************************************************************************
*	原型：void power_check(void)
*	功能: 检测电源状态，若交流电没电，则启动资料保存程序，完成后切断电池供电回路
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
void power_check(void)
{
	if (pw_ac) {
		power = 1;						// 
		pw_count = 0x00;
		
		}
	else pw_count++;
	if (pw_count > 0x30) {				// (100 - 120ms) 检测不到交流电源则启动备份程序
		pw_count = 0x40;
		}
	}

/*********************************************************************************************
*	原型：void wait_for_100ms(void)
*	功能: 定时 100ms 循环
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
int wait_for_100ms(void)
{
	unsigned char xdata * data pp;
	unsigned char i,j,ctemp0,ctemp1;
	do {
		while (!SYS5ms);
		scankey();
		if (SYS25ms) {
			SYS25ms = 0;
			decode_key();
			}
		wr_eeprom_count++;
		if (wr_eeprom_count == 0x02) f_wr_eeprom_now = 0;
		if (wr_eeprom_count >= 0x64) wr_eeprom_count = 0;
//		get_save_phase_plan();
		fun_smbus();
		power_check();
		SYS5ms = 0;	
// 090407
		pp = ADDRESS_74HC244M;
		for (i=0x00;i<4;i++) 
		{
			ctemp0 = ~(*pp);								// 读出当前检测器的状态
			ctemp1 = ctemp0 ^ detector_status_bak[i];
			if (ctemp1 != 0x00) 							// 和上一次的状态不一样
			{
				detector_status_bak[i] = ctemp0;			
				ctemp1 = ctemp1 & ctemp0;					// 只保留从无车到有车的变化					
				if (ctemp1 != 0x00) 
				{
					ctemp0 = 0x01;
					for (j=0x08;j!=0;j--)
					{
						if ((ctemp1 & ctemp0) != 0x00) detector_counter[i][j-1]++;
						ctemp0 = ctemp0 << 1;
					}
				}		
			}
			pp = pp + 0x200;
		}
// 090407
		} while(!SYS100ms);
	SYS100ms = 0;
	
	watchdog=~watchdog;
	
	led_bg_count++;
	if (led_bg_count > 12000) {
		led_bg = 0;
		led_bg_count = 18000;
		}
	else led_bg = 1;
	
//	refrash_lamp();

	judge_detector();
	
	if (f_temperature) {							// 温度
		f_temperature = 0;
		result = ((result - 41857) * 100L) / 1540;
		tempture = (int)(result - 15);					// 减去 1.5 摄氏度
		}
	
	buzz_delay++;
	if (buzz_delay >= 0x02) {
		buzz = 0;
		buzz_delay = 0x05;
		}
	if (f_buzz) {
		f_buzz = 0;
		buzz_delay = 0x00;
		buzz = 1;
		}

//1126	if (f_boot) get_lamp_health_status();

	front_panel_dis(g_controller_status.m_actual_control_mode);		// 前面板控制模式显示（led 指示灯）

	while(!SYS100ms);
	SYS100ms = 0;	
//	if (f_boot) {
		get_lamp_health_status();
		conflict_detect();
		
		cpld_count++;
		if (cpld_count > 0x0f) {						// cpld 两端口超过 1500ms 没有数据传输，就认为 pc104 
			cpld_count = 0x00;							// 丢失中断，由 mcu 主动清掉该中断，并补发丢失中断的数据
			pp = ADDRESS_PC_WR2;
			*pp = 0x00;									// 清中断
			if (f_cpld_trans_num) pp = ADDRESS_PC_WR0;	// 补发数据的通道地址
			else pp = ADDRESS_PC_WR1;			
			*pp = cpld_trans_bak;						// 补发数据
		}
//		}

	refrash_lamp();

//	key_val1 = 0x00;
//	pp = &key_val1;
//	write_comm(7,pp,1);
//	key_val0 = 0x00;
//	pp = &key_val0;
//	write_comm(8,pp,1);


//	uart0_trans_buffer[0][g_out_write_p[0]++] = 0x55;
//	uart0_trans_buffer[0][g_out_write_p[0]++] = 0x54;
//	uart0_trans_buffer[0][g_out_write_p[0]++] = 0x53;
//	uart0_trans_buffer[0][g_out_write_p[0]++] = 0x52;
//
//	if(g_out_write_p[0] == 200) g_out_write_p[0] = 0;
//	
//	uart0_trans_buffer[1][g_out_write_p[1]++] = 0xa5;
//	uart0_trans_buffer[1][g_out_write_p[1]++] = 0xa4;
//	uart0_trans_buffer[1][g_out_write_p[1]++] = 0xa3;
//	uart0_trans_buffer[1][g_out_write_p[1]++] = 0xa2;
//
//	if(g_out_write_p[1] == 200) g_out_write_p[1] = 0;	
	

//	uart0_trans_buffer[0][0] = 0x55;
//	uart0_trans_buffer[0][1] = 0x54;
//	uart0_trans_buffer[0][2] = 0x53;
//	uart0_trans_buffer[0][3] = 0x52;
//	
//	uart0_trans_buffer[1][0] = 0xa5;
//	uart0_trans_buffer[1][1] = 0xa4;
//	uart0_trans_buffer[1][2] = 0xa3;
//	uart0_trans_buffer[1][3] = 0xa2;
//	
//	uart0_trans_buffer[2][0] = 0x05;
//	uart0_trans_buffer[2][1] = 0x04;
//	uart0_trans_buffer[2][2] = 0x03;
//	uart0_trans_buffer[2][3] = 0x02;
//	
//	uart0_trans_buffer[3][0] = 0x5a;
//	uart0_trans_buffer[3][1] = 0x4a;
//	uart0_trans_buffer[3][2] = 0x3a;
//	uart0_trans_buffer[3][3] = 0x2a;
//	
//	uart0_trans_buffer[4][0] = 0x50;
//	uart0_trans_buffer[4][1] = 0x40;
//	uart0_trans_buffer[4][2] = 0x30;
//	uart0_trans_buffer[4][3] = 0x20;			
//	
//	uart0_chanl_sel = 0x08; 
//	UART_CHANL = (UART_CHANL & 0xc7) | uart0_chanl_sel;
//	SBUF0 = out_getchar(1);
//	f_uart0_transed_now0 = 1;
//	f_uart0_transed_now1 = 1;
//	
//	uart0_transed_count[1] = 0x00; 
//	uart0_transed_count[2] = 0x00;
//	uart0_transed_count[3] = 0x00;
//	uart0_transed_count[4] = 0x00;
//	
//	uart0_trans_count[1] = 0x04; 
//	uart0_trans_count[2] = 0x04;
//	uart0_trans_count[3] = 0x04;
//	uart0_trans_count[4] = 0x04;
//	
//	f_uart0_transed_now1 = 1;
//	f_uart0_transed_now2 = 1;
//	f_uart0_transed_now3 = 1;	
//	f_uart0_transed_now4 = 1;
	
//	display_status(g_controller_status.m_minute,g_controller_status.m_second);
//	display_status((char)result,g_controller_status.m_second);
}

/*********************************************************************************************
*	原型：void judge_detector(void)
*	功能: 车辆检测器好坏判断（由于只能根据其输出来判断，因此不太准确，只能作为参考）
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
void judge_detector(void)
{	
unsigned char i,chartemp,j,chanl_start,detector_count_buff[MAX_DETECTOR_COUNT];
	read_detector_time++;
	if (read_detector_time >= 40) {			// 4 秒钟判断一次
		get_detector_counter(&detector_count_buff);
		read_detector_time = 0x00;
		detector_time++;
		for (i=0x00;i<MAX_DETECTOR_COUNT;i++) {
			if (detector_count_buff[i] < detector_count_bak0[i]) chartemp = (64 + detector_count_buff[i]) -  detector_count_bak0[i];
			else chartemp = detector_count_buff[i] -  detector_count_bak0[i];
		
			chanl_start = check_chanl_7;
			for (j=0x00;j<i%8;j++) chanl_start = chanl_start >> 1;
		
			if (chartemp > 8) {				// 4 秒内检测器输出数变化量大于 8，即认为该路检测器有问题
//060211				chanl_start = check_chanl_7;
//060211				for (j=0x00;j<i%8;j++) chanl_start = chanl_start >> 1;
				detector_err[i / 8] = detector_err[i / 8] | chanl_start;
				
				set_error(DETECTOR_ERROR,&g_controller_status.m_error.m_hard_error);			// 保存该故障
				}
			else {							// 否则即认为该路检测器没有问题
				detector_err[i / 8] = detector_err[i / 8] & (~chanl_start);
				}




//			else {
//				if ((chartemp != 0x00) | ((g_config.m_detector_setting.m_attribute[i] & 0x40) == 0x00)) {
//					chanl_start = check_chanl_7;
//					for (j=0x00;j<i%8;j++) chanl_start = chanl_start >> 1;
//					detector_flag[i / 8] = detector_flag[i / 8] | chanl_start;
//					}
//				}
			detector_count_bak0[i] = detector_count_buff[i];
			}
//		if (detector_time >= 248) {		// 启用的检测器如果在 248 * 4 秒内，没有检测到车即认为该路检测器有问题
//			detector_time = 0x00;
//			detector_err[0] = ~detector_flag[0] | detector_err[0];
//			detector_err[1] = ~detector_flag[1] | detector_err[1];
//			detector_err[2] = ~detector_flag[2] | detector_err[2];
//			detector_err[3] = ~detector_flag[3] | detector_err[3];
//			}
		}
	}

/*********************************************************************************************
*	原型：void init_gm8125(void)
*	功能: 串口扩展芯片 GM8125 初始化
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
void init_gm8125(void)
{
#ifdef single_mode
	chip_uart_ms = 1;				// 串口扩展芯片在单通道模式下工作
	UART_CHANL = 0x09;				// 串口 0 在单通道模式下，发送和接受都通过子串口 1 完成
	
	TR2 = 0;						//							230400	115200	76800	38400	19200	9600	7200	4800	2400	1200（Page 213）
	RCAP2L = 0xa0;					// 定时器 2 捕捉寄存器低字节	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
	RCAP2H = 0xff;					// 定时器 2 捕捉寄存器高字节	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xfe	0xfd
	TR2 = 1;
#else 
	UART_CHANL = UART_CHANL & 0xc7;
	P4 = 0xc0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	chip_uart_ms = 0;				// 串口扩展芯片在多通道模式下工作	
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	SBUF0 = 0xf3;					// 设置GM8125子串口波特率为19200bps，母串口波特率为115200bps 10Bit
	ES0 = 0;
	while(TI0 == 0); 
	TI0 = 0;
	delay_200us(50);
	chip_uart_ms = 1; 				// 读命令字 
	while(RI0 == 0); 
	RI0 = 0;
	ReceiveBuff0 = SBUF0; 	
	while (ReceiveBuff0!=0xf3) { 	// 验证写入的命令字是否正确，不正确则重新写 
		ES0 = 0;
		chip_uart_ms = 0;			// GM8125工作在写命令字工作模式下
		SBUF0 = 0xf3;
		while(TI0 == 0); 
		TI0=0; 
		delay_200us(50); 
		chip_uart_ms = 1; 			// 读命令字 
		while(RI0 == 0); 
		RI0 = 0;
		ReceiveBuff0 = SBUF0;
		} 
	chip_uart_ms = 0; 				// 设置GM8125工作在多通道工作模式下
	
	comm_baud = B_9600;
	
	P2MDOUT = 0xff;					// 将 P2.7、P2.6、P2.5、P2.4、P2.3 P2.2 P2.1 P2.0 设置为推挽方式
	P2 = P2 | 0x08;					//
	P4 = 0xff;
	TR2 = 0;						//								230400	115200	76800	38400	19200	9600	7200	4800	2400	1200（Page 213）
	RCAP2L = 0xf4;			//f4		// 定时器 2 捕捉寄存器低字节	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
	RCAP2H = 0xff;						// 定时器 2 捕捉寄存器高字节	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xfe	0xfd
	TR2 = 1;
	RI0 = 0;
	ES0 = 1;
#endif	
	}

/*********************************************************************************************
*	原型：void delay_200us(unsigned char m)
*	功能: 软件延时子程序
*	说明：
*	参数：无
*	返回：无
*********************************************************************************************/
void delay_200us(unsigned char m) 
{ 
unsigned int n,i; 
	n=0; 
	while(n < m) {
		for (i=0x00;i<137;i++) {
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			}
		n++;
		}
	return; 
	}

/*********************************************************************************************
*	原型：void decode_key(void)
*	功能：按键解码
*	说明：根据 AD 值，区分按键，并处理遥控器输入
*	参数：无
*	返回：无
*********************************************************************************************/
void decode_key(void) 
{
unsigned char data ctemp0;
unsigned char xdata *pchar;
	pchar = ADDRESS_TELECONTROL;
	ctemp0 = *pchar;
	ctemp0 = ctemp0 & 0x0f;
	if ((ctemp0 == 0x00) && (TELECONTROL_BAK == 0x0b)) TELECONTROL_BAK = 0x00;
	if ((ctemp0 != TELECONTROL_BAK) && (ctemp0 <= 0x0c) && (ctemp0 != 0x00)) {
		TELECONTROL_BAK = ctemp0;
		ctemp0 = 0x00;
		switch (TELECONTROL_BAK) {					// 遥控按键解码
			case 0x01:
				{
					f_run_mode = 1;						// 以前面板上的设定运行
					ctemp0 = 0x01;
					break;
				}
			case 0x02: 
				{
					f_run_mode = 1;						// 以前面板上的设定运行
					ctemp0 = 0x02;
					break;
				}
			case 0x03:
				{
					f_run_mode = 1;						// 以前面板上的设定运行
					ctemp0 = 0x03;
					break;
				}
			case 0x04:
				{
					f_run_mode = 1;						// 以前面板上的设定运行
					ctemp0 = 0x04;
					break;
				}
			case 0x05:
				{
					f_run_mode = 1;						// 以前面板上的设定运行
					ctemp0 = 0x05;
					if (!f_manual)
					{
						f_manual = 1;
						f_manual_auto = 1;				// 手动/自动转换一次有效
					}
					manual_set_stage = 1;
				 	f_manual_step= 1;
					break;
				}
			case 0x06:
				{
					f_run_mode = 1;						// 以前面板上的设定运行
					ctemp0 = 0x06;
					if (!f_manual) 
					{
						f_manual = 1;
						f_manual_auto = 1;				// 手动/自动转换一次有效
					}
					manual_set_stage = 2;
					f_manual_step = 1;
					break;
				}
			case 0x07: 
				{
					f_run_mode = 1;						// 以前面板上的设定运行
					ctemp0 = 0x07;
					if (!f_manual)
					{
						f_manual = 1;
						f_manual_auto = 1;				// 手动/自动转换一次有效
					}
					manual_set_stage = 3;
					f_manual_step = 1;
					break;
				}
			case 0x08:
				{
					f_run_mode = 1;						// 以前面板上的设定运行
					ctemp0 = 0x08;
					if (!f_manual)
				  {
						f_manual = 1;
						f_manual_auto = 1;				// 手动/自动转换一次有效
					}
					manual_set_stage = 4;
					f_manual_step = 1;
					break;
				}
			case 0x09:	{
				f_manual = 1;						// 手动/自动选择开关已打开处于手动状态
				f_manual_auto = 1;					// 手动/自动转换一次有效
				break;
				}
			case 0x0a:	{
				f_manual = 0;						// 手动/自动选择开关已打开处于自动状态
				f_manual_auto = 1;					// 手动/自动转换一次有效
				break;
				}
			case 0x0b:	f_step = 1;	break;			// 一次有效的"步进按键"
			case 0x0c:	{
				f_manual = 0;						// 手动/自动选择开关已打开处于自动状态
				f_manual_auto = 1;					// 手动/自动转换一次有效
				break;
				}
			default:break;
			}
		if (ctemp0 != 0x00) {
			key0_val = ctemp0;
			f_mode_switch = 1;
			}
		}
	if (f_key0_int) {			 					// 键盘 0 按键
		f_key0_int = 0;
		if (!f_key0_int_bak) {						// 该按键按下后的第一次采样值
			f_key0_int_bak = 1;
			key_val0 = key0_adc0;
			}
		else {										// 该按键按下后的第二次采样值
			if (key_val0 > key0_adc0) ctemp0 = key_val0 - key0_adc0;
			else ctemp0 = key0_adc0 - key_val0;
			if (ctemp0 < 8) {					// 按键按下的二次采样值差值在合理范围即认为为本次按键有效
				if (!f_key0_ed) {
					ctemp0 = 0xff;
//					if ((41 < key0_adc0) & (key0_adc0 < 71)) ctemp0 = 0;                                 
//					if ((77 < key0_adc0) & (key0_adc0  < 107)) ctemp0 = 1;
//					if ((114 < key0_adc0) & (key0_adc0  < 154)) ctemp0 = 2;
//					if ((186 < key0_adc0) & (key0_adc0  < 236)) ctemp0 = 3;
					
					if ((40 < key0_adc0) & (key0_adc0 < 78)) ctemp0 = 0;    
					if ((80 < key0_adc0) & (key0_adc0  < 120)) ctemp0 = 1;  
					if ((130 < key0_adc0) & (key0_adc0  < 191)) ctemp0 = 2; 
					if ((210 < key0_adc0) & (key0_adc0  <= 250)) ctemp0 = 3; 
					
					if (ctemp0 != 0xff) {		// 一次有效的"键盘 0 按键"
						key0_val = ctemp0;
						f_mode_switch = 1;
						f_buzz = 1;
						led_bg = 1;				// 点亮 LCD 背光
						f_key0_ed = 1;			// 模拟输入 0 一次有效输入
						f_key1_ed = 0;
						led_bg_count = 0;
						f_run_mode = 1;			// 以前面板上的设定运行
						}
					}
				}
			else {
				f_key0_ed = 0;
				f_key0_int_bak = 0;	
				}
			key_val0 = key0_adc0;
			}
		}

	if (f_key1_int) {			 				// 键盘 0 按键
		f_key1_int = 0;
		if (!f_key1_int_bak) {					// 该按键按下后的第一次采样值
			f_key1_int_bak = 1;
			key_val1 = key1_adc0;
			}
		else {									// 该按键按下后的第二次采样值
			if (key_val1 > key1_adc0) ctemp0 = key_val1 - key1_adc0;
			else ctemp0 = key1_adc0 - key_val1;
			if (ctemp0 < 8) {					// 按键按下的二次采样值差值在合理范围即认为为本次按键有效
				if (!f_key1_ed) {
					ctemp0 = 0xff;

//					if ((41 < key1_adc0) & (key1_adc0 < 71)) ctemp0 = 0;                                 
//					if ((77 < key1_adc0) & (key1_adc0  < 107)) ctemp0 = 1;
//					if ((114 < key1_adc0) & (key1_adc0  < 154)) ctemp0 = 2;
//					if ((186 < key1_adc0) & (key1_adc0  < 236)) ctemp0 = 3;


					if ((40 < key1_adc0) & (key1_adc0 < 78)) ctemp0 = 0;    
					if ((80 < key1_adc0) & (key1_adc0  < 120)) ctemp0 = 1;  
					if ((130 < key1_adc0) & (key1_adc0  < 191)) ctemp0 = 2; 
					if ((210 < key1_adc0) & (key1_adc0  <= 250)) ctemp0 = 3; 
					if (ctemp0 != 0xff) {					// 一次有效的"键盘 0 按键"
						f_buzz = 1;
						led_bg = 1;							// 点亮 LCD 背光
						f_key0_ed = 0;
						f_key1_ed = 1;						// 模拟输入 1 一次有效输入
						led_bg_count = 0;
						key0_val = ctemp0 + 4;
						f_mode_switch = 1;
						f_run_mode = 1;						// 以前面板上的设定运行
						}
					}
				}
			else {
				f_key1_ed = 0;
				f_key1_int_bak = 0;	
				}
			key_val1 = key1_adc0;
			}
		}

//1219	if (f_manual_use) {			 						// 手动/自动选择开关
//1219		if (!f_key2_int_bak) {							// 该按键按下后的第一次采样值
//1219			f_key2_int_bak = 1;
//1219			key_val2 = key2_adc0;
//1219			}
//1219		else {											// 该按键按下后的第二次采样值
//1219			if (key_val2 > key2_adc0) ctemp0 = key_val2 - key2_adc0;
//1219			else ctemp0 = key2_adc0 - key_val2;
//1219			if (ctemp0 < 8) {						// 按键按下的二次采样值差值在合理范围即认为为本次按键有效		
//1219				if (!f_key2_ed) {
//1219					f_buzz = 1;
//1219					led_bg = 1;							// 点亮 LCD 背光
//1219					f_key2_ed = 1;						// 模拟输入 2 一次有效输入
//1219					f_manual = 1;						// 手动/自动选择开关已打开处于手动状态
//1219					led_bg_count = 0;
//1219					f_auto_buzz = 1;
//1219					f_manual_bak = 1;
//1219					
//1219					manual_m_control_mode_bak = g_controller_status.m_actual_control_mode;		// 保存手动前的实际控制模式
//1219									
//1219					}
//1219				}
//1219			else {
//1219				f_key2_ed = 0;
//1219				f_key2_int_bak = 0;	
//1219				}
//1219			key_val2 = key2_adc0;
//1219			}
//1219		}
//1219	else {
//1219		if (f_manual_bak) {
//1219			f_manual_bak = 0;
//1219//1026			g_config.m_control_mode_selection.m_control_mode = MULTIPLE_SCHEDULE_CONTROL;
//1219			if ((manual_m_control_mode_bak == 4) || (manual_m_control_mode_bak == 5)) g_config.m_control_mode_selection.m_control_mode = manual_m_control_mode_bak;
//1219			else g_config.m_control_mode_selection.m_control_mode = MULTIPLE_SCHEDULE_CONTROL;
//1219			
//1219			save_control_mode_selection();
//1219			}
//1219		f_manual = 0;
//1219		f_key2_ed = 0;
//1219		f_key2_int_bak = 0;	
//1219		if (f_auto_buzz) {
//1219			f_buzz = 1;
//1219			led_bg = 1;									// 点亮 LCD 背光
//1219			f_auto_buzz = 0;
//1219			led_bg_count = 0;
//1219			}
//1219		}

	if (f_key2_int) {			 							// 手动/自动选择按键
		f_key2_int = 0;
		if (!f_key2_ed) {									// 下一次有效的"手动/自动选择按键"必须在该按键释放后
			if (!f_key2_int_bak) {							// 该按键按下后的第一次采样值
				f_key2_int_bak = 1;
				key_val2 = key2_adc0;
				}
			else {											// 该按键按下后的第二次采样值
				if (key_val2 > key2_adc0) ctemp0 = key_val2 - key2_adc0;
				else ctemp0 = key2_adc0 - key_val2;
				if (ctemp0 < 8) {							// 按键按下的二次采样值差值在合理范围即认为为本次按键有效
					f_buzz = 1;
					led_bg = 1;								// 点亮 LCD 背光
					f_key2_ed = 1;							// 模拟输入 2 一次有效输入
					led_bg_count = 0;
					f_manual = !f_manual;					// 手动/自动转换
					f_manual_auto = 1;						// 手动/自动转换一次有效
					}
				else key_val2 = key2_adc0;
				}
			}
		}
	else {													// 手动/自动选择按键已释放
		f_key2_ed = 0;
		f_key2_int_bak = 0;
		}

	if (f_key3_int) {			 							// 步进按键
		f_key3_int = 0;
		if (!f_key3_ed) {									// 下一次有效的"步进按键"必须在该按键释放后
			if (!f_key3_int_bak) {							// 该按键按下后的第一次采样值
				f_key3_int_bak = 1;
				key_val3 = key3_adc0;
				}
			else {											// 该按键按下后的第二次采样值
				if (key_val3 > key3_adc0) ctemp0 = key_val3 - key3_adc0;
				else ctemp0 = key3_adc0 - key_val3;
				if (ctemp0 < 8) {							// 按键按下的二次采样值差值在合理范围即认为为本次按键有效
					f_buzz = 1;
					led_bg = 1;								// 点亮 LCD 背光
					f_key3_ed = 1;							// 模拟输入 7 一次有效输入
					f_step = 1;								// 一次有效的"步进按键"
					led_bg_count = 0;
					}
				else key_val3 = key3_adc0;
				}
			}
		}
	else {													// 步进按键已释放
		f_key3_ed = 0;
		f_key3_int_bak = 0;
		}

	if (f_manual_auto) {									// 手动/自动转换需处理的数据
		f_manual_auto = 0;
		if (f_manual) {										// 保存手动前的实际控制模式
			manual_m_control_mode_bak = g_controller_status.m_actual_control_mode;		
			}
		else {												// 恢复手动前的实际控制模式
// 090610			if ((manual_m_control_mode_bak == 4) || (manual_m_control_mode_bak == 5)) g_config.m_control_mode_selection.m_control_mode = manual_m_control_mode_bak;
// 090610			else g_config.m_control_mode_selection.m_control_mode = MULTIPLE_SCHEDULE_CONTROL;
// 090625		  g_config.m_control_mode_selection.m_control_mode = manual_m_control_mode_bak;
			g_controller_status.m_designate_control_mode=manual_m_control_mode_bak;
//			save_control_mode_selection();					// 保存当前的控制模式
// 071213
			if (g_controller_status.m_designate_control_mode == 5) 
				   f_return_actuate_control = 1;
// 071213
			}
		}
//071210
//071212	if (f_manual_step) {
//071212		f_manual = 0;
//071212		g_config.m_control_mode_selection.m_control_mode = MULTIPLE_SCHEDULE_CONTROL;		
//071212		}
//071210
	}

/*********************************************************************************************
*	原型：void scankey(void)
*	功能：键盘扫描。只处理单个按键，不处理组合按键。每次按键有效按下时，根据时间长短来确定
*		  其键值，按下时间小于 1.2s 时，为基本键值（按键抬起后才执行该键），超过 1.2s 时，
*		  为附加键值（不用等到该按键释放才执行，而是立即执行）。下一个按键必须在上一个按键
*         释放后按下，才能被扫描到并被执行。如几键同时按下时，只有最后按下的那个按键有效。
*	说明：扫描线通过修改 scan_table[i] 来完成。
*	参数：无
*	返回：无
*********************************************************************************************/
void scankey(void)
{
unsigned char i,j,k,ctemp;
unsigned char xdata *pchar;
	pchar = ADDRESS_keyboard;
	key_port = key_port & 0xf9;
	i = *pchar;
	if (i!=0xff) {
		for (i=0;i<=1;i++) {
			key_port = key_port | 0x06;
			key_port = key_port & scan_table[i];		// i+1 表示扫描线的多少
			ctemp = *pchar;
			j = (ctemp & 0x01) ^ 0x01;
			if (j!=0) {
				k = 0;									// k+1 表示输入线的多少
				break;
				}
			j = (ctemp & 0x02) ^ 0x02;
			if (j!=0) {
				k = 1;
				break;
				}
			j = (ctemp & 0x04) ^ 0x04;
			if (j!=0) {
				k = 2;
				break;
				}
			j = (ctemp & 0x08) ^ 0x08;
			if (j!=0) {
				k = 3;
				break;
				}
			j = (ctemp & 0x10) ^ 0x10;
			if (j!=0) {
				k = 4;
				break;
				}
			j = (ctemp & 0x20) ^ 0x20;
			if (j!=0) {
				k = 5;
				break;
				}
			j = (ctemp & 0x40) ^ 0x40;
			if (j!=0) {
				k = 6;
				break;
				}
			j = (ctemp & 0x80) ^ 0x80;
			if (j!=0) {
				k = 7;
				break;
				}
			}
		j = i*8 + k;
		if (key_val_temp == j) {
			key_time++;
			if (key_count>=0x04) f_buzz = 1;	// 键盘缓冲区最多暂存 4 个按键，如超过 4 个则不接收新的按键，并报警
			if (!f_key_now) {
//1024				f_buzz = 1;
				led_bg = 1;			// 点亮 LCD 背光
				led_bg_count = 0;
				f_key_now = 1;			// 获得一个按键值，直到该按键被释放
				key_val = key_table[j]; 
				}
			else {
				if (key_time >= 0x88) {		// 长时间（超过 0.68s 时）按某一按键就赋另一键值
					key_time = 0xf4;
					if (!f_key && (key_count<0x04)) {	// 键盘缓冲区最多暂存 4 个按键，如超过 4 个则不接收新的按键，并报警
						f_key = 1;
						f_buzz = 1;
						key_count++;
						key_buf[0] = key_val + 0x10;
//						if (key_buf[0] == 0x12) f_key_val = !f_key_val;
//						if (f_key_val) {
//							f_lamp_err = 0;				// 复位灯故障标志位
//							f_lamp_yellow_err = 0;
//							f_lamp_green_or_red_err =0;							
//							}
						}
					}
				}
			}                	
		else {
			key_time = 0x00;
			key_val_temp = j;
			}
		}
	else {
		key_time = 0x00;
		key_val_temp = 0xe0;
		if (f_key_now) {
			f_key_now = 0;				// 按键被释放
			if (!f_key && (key_count<0x04)) {	// 键盘缓冲区最多暂存 4 个按键，如超过 4 个则不接收新的按键，并报警
//				key_buf = key_val;

				key_count++;
				for (i=key_count;i>1;i--) key_buf[i-1] = key_buf[i-2];
				key_buf[0] = key_val;
				
				}

			f_key = 0;
//			vm_mcu_send_key(&g_vm,key_val);
			}
		}	
	key_port = key_port | 0x06;
	}


/*********************************************************************************************
*	原型：int set_term_status(phase_t phase)
*	功能: 设置输出端子的状态
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int set_term_status(lamp_status_t* phase)
{
bit bit_temp;
unsigned char data ctemp0,ctemp1,i,j,k,l,buff[3];
unsigned char* pp;
//	for(i=0; i<18; i++)	phase.m_output[i] = 0xaa;
	
	flick_count = 0x00;
	pp = phase;
	l = 0x00;
	bit_temp = 1;
	ctemp1 = 0x00;
	lamp_output_buffer[l] = 0x00;
	lamp_flick_buffer[l] = 0x00;
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/4 + 1;) {

// 端子映射	
		if (bit_temp) {
			bit_temp = 0;
			buff[2] = (*(pp + i + 2) << 4) & 0xc0;
			buff[1] = (*(pp + i + 1) << 4) & 0x30;
			buff[0] = buff[1] | buff[2];
			buff[2] = (*(pp + i + 1) >> 4) & 0x0c; 
			buff[0] = buff[0] | buff[2];
			buff[2] = (*(pp + i) >> 4) & 0x03; 
			buff[0] = buff[0] | buff[2];

			buff[1] = (*(pp + i + 2) << 2) & 0xc0;
			buff[1] = buff[1] | ((*(pp + i + 2) << 4) & 0x30);
			buff[2] = *(pp + i + 1) & 0x0c;
			buff[1] = buff[1] | buff[2];
			buff[2] = (*(pp + i + 2) >> 6) & 0x03;
			buff[1] = buff[1] | buff[2];
	
			buff[2] = ((*(pp + i) << 6) & 0xc0) | (*(pp + i + 1)  & 0x30);
			buff[2] = buff[2] | ((*(pp + i) >> 4) & 0x0c) | ((*(pp  + i) >> 2) & 0x03);
			}
// 端子映射			

//		k = *(pp + i);
		k = buff[ctemp1];
		for (j=0x00;j<4;j++) {
			ctemp0 = k & 0xc0;
			lamp_output_buffer[l] = lamp_output_buffer[l] << 1;
			lamp_flick_buffer[l] = lamp_flick_buffer[l] << 1;
			if (ctemp0 == 0x40) lamp_output_buffer[l] = lamp_output_buffer[l] | 0x01;
			if (ctemp0 == 0x80) lamp_flick_buffer[l] = lamp_flick_buffer[l] | 0x01;
			k = k << 2;
			}
		i++;
		ctemp1++;
		if (((ctemp1%2) != 0x00) & ((ctemp1%3) != 0x00)) {
			l++;
			lamp_flick_buffer[l] = 0x00;
			lamp_output_buffer[l] = 0x00;
			}
		if ((ctemp1%3) == 0x00) {
			bit_temp = 1;
			l++;
			ctemp1 = 0x00;
			lamp_flick_buffer[l] = 0x00;
			lamp_output_buffer[l] = 0x00;
			}
		}	
	return 0;
	}


/*********************************************************************************************
*	原型：void get_lamp_health_status(void)
*	功能: 获取灯的故障信息
*	说明：0 没故障，1表示有故障
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void get_lamp_health_status(void)
{
unsigned char i,j,k,ctemp1,ctemp2;
unsigned char ctemp[((MAX_LAMP_NUM-1)/8)+1];
static unsigned char lamp_health_count[((MAX_LAMP_NUM-1)/8)+1][8];		//,lamp_health_count_dly[((MAX_LAMP_NUM-1)/8)+1][8];
static unsigned char lamp_health_count1[((MAX_LAMP_NUM-1)/8)+1][8];
//	pp = health;
	j = 0x00;
	f_rd_lamp_input_now = 1;
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/12 + 1;i++) {	// 
		lamp_input_buffer[10][j] = lamp_input_buffer[0][j] & lamp_input_buffer[1][j] & lamp_input_buffer[2][j] & lamp_input_buffer[3][j]
								 & lamp_input_buffer[4][j] & lamp_input_buffer[5][j] & lamp_input_buffer[6][j] & lamp_input_buffer[7][j]
								 & lamp_input_buffer[8][j] & lamp_input_buffer[9][j];			// & lamp_input_buffer[10][j] & lamp_input_buffer[11][j]
//								 & lamp_input_buffer[12][j] & lamp_input_buffer[13][j] & lamp_input_buffer[14][j] & lamp_input_buffer[15][j]
//								 & lamp_input_buffer[16][j] & lamp_input_buffer[17][j] & lamp_input_buffer[18][j] & lamp_input_buffer[19][j];
		lamp_input_buffer[10][j] = (~lamp_input_buffer[10][j]) ^ lamp_output_buffer1[j];
		j++;
		lamp_input_buffer[10][j] = lamp_input_buffer[0][j] & lamp_input_buffer[1][j] & lamp_input_buffer[2][j] & lamp_input_buffer[3][j]
								 & lamp_input_buffer[4][j] & lamp_input_buffer[5][j] & lamp_input_buffer[6][j] & lamp_input_buffer[7][j]
								 & lamp_input_buffer[8][j] & lamp_input_buffer[9][j];			// & lamp_input_buffer[10][j] & lamp_input_buffer[11][j]
//								 & lamp_input_buffer[12][j] & lamp_input_buffer[13][j] & lamp_input_buffer[14][j] & lamp_input_buffer[15][j]
//								 & lamp_input_buffer[16][j] & lamp_input_buffer[17][j] & lamp_input_buffer[18][j] & lamp_input_buffer[19][j];
		lamp_input_buffer[10][j] = (~lamp_input_buffer[10][j]) ^ lamp_output_buffer1[j];
		j++;
		}	
	f_rd_lamp_input_now = 0;
	k = 0x00;
//	xxxx 1111  1111 1111
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/8;i++) {			// 一块灯控板有 12 个灯，读回的状态占用 12 位
// 端子反映射			
		ctemp1 = (lamp_input_buffer[10][k+1] << 2) & 0x08; 						// 红 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k] << 2) & 0x04);              // 黄 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+1] << 1) & 0x02);            // 绿 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+1] >> 3) & 0x01);			// 红 2
		
		ctemp2 = (lamp_input_buffer[10][k] << 6) & 0x80;                         // 黄 2
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+1] << 4) & 0x40);            // 绿 2
		ctemp2 = ctemp2 | (lamp_input_buffer[10][k+1] & 0x20);					// 红 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k] << 2) & 0x10);				// 黄 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+1] >> 1) & 0x08);			// 绿 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+1] >> 5) & 0x04);			// 红 4
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k] >> 2) & 0x02);				// 黄 4
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+1] >> 6) & 0x01);			// 绿 4
		
		lamp_input_buffer[10][k] = ctemp1;
		lamp_input_buffer[10][k+1] = ctemp2;
		
		ctemp1 = (lamp_input_buffer[10][k+3] << 2) & 0x08; 						// 红 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+2] << 2) & 0x04);            // 黄 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+3] << 1) & 0x02);            // 绿 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+3] >> 3) & 0x01);			// 红 2
		
		ctemp2 = (lamp_input_buffer[10][k+2] << 6) & 0x80;                       // 黄 2
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+3] << 4) & 0x40);            // 绿 2
		ctemp2 = ctemp2 | (lamp_input_buffer[10][k+3] & 0x20);					// 红 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+2] << 2) & 0x10);			// 黄 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+3] >> 1) & 0x08);			// 绿 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+3] >> 5) & 0x04);			// 红 4
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+2] >> 2) & 0x02);			// 黄 4
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+3] >> 6) & 0x01);			// 绿 4
		
		lamp_input_buffer[10][k+2] = ctemp1;
		lamp_input_buffer[10][k+3] = ctemp2;
// 端子反映射

		j = lamp_input_buffer[10][k+1] >> 4;				// 将 2 块灯控板合并为 3 个字节
		ctemp[i] = (lamp_input_buffer[10][k] << 4) | j;
		ctemp[i + 1] = lamp_input_buffer[10][k+1] << 4 | (lamp_input_buffer[10][k+2] & 0x0f);
		ctemp[i + 2] = lamp_input_buffer[10][k+3];

//w		j = lamp_input_buffer[10][k+1] >> 4;				// 将 2 块灯控板合并为 3 个字节
//w		*(pp + i) = (lamp_input_buffer[10][k] << 4) | j;
//w		*(pp + i + 1) = lamp_input_buffer[10][k+1] << 4 | (lamp_input_buffer[10][k+2] & 0x0f);
//w		*(pp + i + 2) = lamp_input_buffer[10][k+3];
		i = i + 0x02;
		k = k + 0x04;
		}
//	xxxx 1111  1111 1111


//w1031				每个端子的输出状态若在 1400ms 内连续出错，就认为其有故障 
// 090612
	f_1s = 0;
	delay_1s++;
	if (delay_1s >= 10) {
		f_1s = 1;
		delay_1s = 0x00;
		}
// 090612	
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/8 + 1;i++) {
		ctemp1 = 0x80;
		for (j=0x00;j<8;j++) {
			if ((ctemp[i] & ctemp1) == 0x00) {
				lamp_health_count[i][j] = 0x00;
				if (lamp_health_count1[i][j] != 0x00) {
					if (f_1s) lamp_health_count1[i][j]--;
					}
				else lamp_health_status[i] = lamp_health_status[i] & (~ctemp1);
				}
			else {
				lamp_health_count[i][j]++;
				if (lamp_health_count[i][j] > 200) lamp_health_count[i][j] = 200;
				}
//    		if (!f_lamp_err) {								// 
				if (lamp_health_count[i][j] >= 0x0d) {		// 某个灯连续 1400ms 检测有问题，就认为该灯有故障
					lamp_health_count1[i][j] = 200;			// 某个灯一旦检测到有故障，就必须连续 200s 检测无问题，才能取消该故障
					lamp_health_status[i] = lamp_health_status[i] | ctemp1;
					}
//				}
			ctemp1 = ctemp1 >> 1;
			}
		}	
//w1031
//w1031	return 0;
	}

/*********************************************************************************************
*	原型：void conflict_detect(void)
*	功能: 冲突检测
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void conflict_detect(void)
{
unsigned char i,j,ctemp1,ctemp2;
	ctemp1 = 0x00;
	f_lamp_yellow_err = 0;
	f_lamp_green_or_red_err = 0;
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/8 + 1;i++) {
		if (lamp_health_status[i] != 0x00) {
			ctemp2 = 0x80;
			for (j=0x00;j<8;j++) {
				if ((lamp_health_status[i] & ctemp2) != 0x00) {								// 当前端子有故障
					f_lamp_err = 1;															// 当前端子有故障标志位
// 090224					f_lamp_err = 1;															// 当前端子有故障标志位
					ctemp1 = 8 * i + j;
					if (is_term_connect_to_yellow(i+1) == 0x00) f_lamp_green_or_red_err = 1;// 红灯或绿灯有故障
					else f_lamp_yellow_err = 1;												// 黄灯有故障

//					if ((lamp_output_buffer1[i] & ctemp2) != 0x00) {						// 当前该端子设定为亮
//						
//						}
//					else {																	// 当前该端子设定为灭
//						if (is_term_connect_to_green(i+1) != 0x00) 
//						}
					
					}
				ctemp2 = ctemp2 >> 1;
				}
			
			}
		}
// 090611	
//	if (f_lamp_err) {
//		lamp_err_delay++;
//		if (lamp_err_delay >= 200) {														// 故障状态保留 20s
//			f_lamp_err = 0;
//			lamp_err_delay = 0x00;
//			}
//		}
//	else lamp_err_delay = 0x00;
// 090611
	}




/*********************************************************************************************
*	原型：int get_lamp_health(lamp_health_t* health)
*	功能: 获取灯的故障信息
*	说明：0 没故障，1表示有故障
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int get_lamp_health(lamp_health_t* health)
{
unsigned char i;
unsigned char xdata *pp;
	pp = health;
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/8 + 1;i++) *(pp + i) = lamp_health_status[i];
	return 0;
	}

/*********************************************************************************************
*	原型：int get_detector_status(detector_status_t* status)
*	功能: 获取检测器的检测信息
*	说明：0表示没车，1表示有车
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int get_detector_status(detector_status_t* status)
{
unsigned char data i;
unsigned char xdata *pp,*qq;
	qq = status;
	pp = ADDRESS_74HC244M;
	for (i=0x00;i<4;i++) {
		*(qq + i) = ~(*pp);
		pp = pp + 0x200;
		}
	return 0;
	}

/*********************************************************************************************
*	原型：int get_detector_health(detector_health_t* health)
*	功能: 获取检测器的故障信息
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int get_detector_health(detector_health_t* health)
{
unsigned char *pp;
	pp = health;
	*pp = detector_err[0];
	*(pp+1) = detector_err[1];
	*(pp+2) = detector_err[2];
	*(pp+3) = detector_err[3];
	return 0;
	}

/*********************************************************************************************
*	原型：int get_detector_counter(detector_counter_t* counter)
*	功能: 获取检测器计数值
*	说明：计数范围为 0 -- 63
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int get_detector_counter(detector_counter_t* counter)
{
unsigned char data i,j,k,chanl_start;
unsigned char xdata *pp,*pp1;
// 090407	pp1 = counter;
// 090407	j = 0x00;
// 090407	for (i=0x00;i<4;i++) {
// 090407		chanl_start = check_chanl_7;
// 090407		for (k = 0x00;k < 0x08;k++) {
// 090407			pp = ADDRESS_82C55A_B + i * 0x200;
// 090407			*pp = ~chanl_start;	
// 090407			pp = ADDRESS_82C55A_A + i * 0x200;
// 090407			*(pp1 + j) = *pp;
// 090407			chanl_start = chanl_start >> 1;
// 090407			j++;
// 090407			}
// 090407		}	

	pp1 = counter;
	j = 0x00;
	for (i=0x00;i<4;i++) {
// 090407		chanl_start = check_chanl_7;
		for (k = 0x00;k < 0x08;k++) {
// 090407			pp = ADDRESS_82C55A_B + i * 0x200;
// 090407			*pp = ~chanl_start;	
// 090407			pp = ADDRESS_82C55A_A + i * 0x200;
// 090407			*(pp1 + j) = *pp;
// 090407			chanl_start = chanl_start >> 1;
			*(pp1 + j) = detector_counter[i][k];
			j++;
			}
		}	
	return 0;
	}

//061016/*********************************************************************************************
//061016*	原型：clear_detector_counter(int detector)
//061016*	功能: 清除指定检测器的计数值
//061016*	说明：
//061016*	参数：无                  
//061016*	返回：无                  
//061016*********************************************************************************************/
//061016int clear_detector_counter(int detector)
//061016{
//061016unsigned char ctemp1,i,chanl_start;
//061016unsigned char xdata *pp;
//061016	ctemp1 = (char)detector;
//061016	chanl_start = check_chanl_7;
//061016	for (i=0x00;i<(ctemp1-1)%8;i++) chanl_start = chanl_start >> 1;
//061016	pp = ADDRESS_82C55A_C + ((ctemp1 - 1) / 8) * 0x200;
//061016	*pp = ~chanl_start;
//061016	*pp = 0xff;								// 将检测器复位端清零
//061016	
//061016	get_detector_counter(&detector_count_bak0);
//061016	read_detector_time = 0x00;
//061016	}

/*********************************************************************************************
*	原型：void in_init(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void in_init(int port)
{
	g_in_read_p[port-1] = 0;
	g_in_write_p[port-1] = 0;
	}

/*********************************************************************************************
*	原型：int open_comm(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void in_putchar(int port,unsigned char ch)
{
	uart0_receive_buffer[port-1][g_in_write_p[port-1]++] = ch;
	if(g_in_write_p[port-1] == uart0_trans_max)
		g_in_write_p[port-1] = 0;
	}

/*********************************************************************************************
*	原型：int open_comm(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
unsigned char in_getchar(int port)
{
	unsigned char ch;
	ch = uart0_receive_buffer[port - 1][g_in_read_p[port-1]++];
	if(g_in_read_p[port-1] == uart0_trans_max)
		g_in_read_p[port-1] = 0;
	return ch;
	}

//061016/*********************************************************************************************
//061016*	原型：int open_comm(int port)
//061016*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
//061016*	说明：
//061016*	参数：无                  
//061016*	返回：无                  
//061016*********************************************************************************************/
//061016int is_in_full(int port)
//061016{
//061016	if((g_in_write_p[port-1] + 1)%uart0_trans_max  == g_in_read_p[port-1])
//061016		return 1;
//061016	else
//061016		return 0;
//061016	}

/*********************************************************************************************
*	原型：int open_comm(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int is_in_empty(int port)
{
	if(g_in_read_p[port-1] == g_in_write_p[port-1])
		return 1;
	else
		return 0;
	}

/*********************************************************************************************
*	原型：int open_comm(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void out_init(int port)
{
	g_out_read_p[port-1] = 0;
	g_out_write_p[port-1] = 0;
	}

/*********************************************************************************************
*	原型：int open_comm(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void out_putchar(int port,unsigned char ch)
{
	uart0_trans_buffer[port-1][g_out_write_p[port-1]++] = ch;
//	uart0_trans_buffer[port-1][g_out_write_p[port-1]++] = 0xaa;
	if(g_out_write_p[port-1] == uart0_trans_max)
		g_out_write_p[port-1] = 0;
	}

/*********************************************************************************************
*	原型：int open_comm(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
unsigned char out_getchar(int port)                              
{
unsigned char ch ;
	ch = uart0_trans_buffer[port - 1][g_out_read_p[port-1]++];
	if(g_out_read_p[port-1] == uart0_trans_max)
		g_out_read_p[port-1] = 0;
	return ch;
	}

/*********************************************************************************************
*	原型：int open_comm(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int is_out_full(int port)
{
	if((g_out_write_p[port-1] + 1)%uart0_trans_max  == g_out_read_p[port-1])
		return 1;
	else
		return 0;
	}

/*********************************************************************************************
*	原型：int open_comm(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int is_out_empty(int port)
{
	if(g_out_read_p[port-1] == g_out_write_p[port-1])
		return 1;
	else
		return 0;
	}

/*********************************************************************************************
*	原型：int open_comm(int port)
*	功能: 打开 fd 对应的通信端口，成功返回该 fd，失败返回 -1
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int open_comm(int port)
{
unsigned char i,ctemp1,j;	
	i = (char)port;
	if (i<=6) {
		j = g_config.m_comm_param[port-1].m_baud;
		if (j == comm_baud) {				// 和当前波特率一样
			in_init(port);
			out_init(port);
			ctemp1 = 0x01;
			for (i=0x01;i<port;i++) ctemp1 = ctemp1 << 1;
			flag1_uart0 = flag1_uart0 | ctemp1;							// 将对应端口正在接收数据标志置位
			return port;
			} 
		else {															// 和当前波特率不一样，需更改
			if (((flag0_uart0 & 0x1f) | (flag1_uart0 & 0x1f)) == 0x00) {// 当前串口都没使用，可以更改波特率
				
				UART_CHANL = UART_CHANL & 0xc7;							// 将 STADD0 - STADD2 清零
				P4 = 0xc0;												// 将 SRADD0 - SRADD2 清零
				_nop_();
				_nop_();
				_nop_();
				_nop_();
				chip_uart_ms = 0;										// 串口扩展芯片在多通道模式下工作
				_nop_();
				_nop_();
				_nop_();
				_nop_();
				SBUF0 = 0xf0 | (g_config.m_comm_param[port-1].m_baud - 2);	// 设置GM8125子串口波特率为1200 - 19200bps，母串口波特率为7200 - 115200bps 10Bit
				ES0 = 0;
				while(TI0 == 0); 
				TI0 = 0;
				delay_200us(50);
				chip_uart_ms = 1; 							// 读命令字 
				while(RI0 == 0); 
				RI0 = 0;
				ReceiveBuff0 = SBUF0;
				ctemp1 = 0x00;
				while (ReceiveBuff0 != (0xf0 | (g_config.m_comm_param[port-1].m_baud - 2))) { 	// 验证写入的命令字是否正确，不正确则重新写 
					ES0 = 0;
					chip_uart_ms = 0;					// GM8125工作在写命令字工作模式下
					SBUF0 = 0xf0 | (g_config.m_comm_param[port-1].m_baud - 2);
					while(TI0 == 0); 
					TI0=0; 
					delay_200us(50); 
					chip_uart_ms = 1; 					// 读命令字 
					while(RI0 == 0); 
					RI0 = 0;
					ReceiveBuff0 = SBUF0;
					ctemp1++;
					if (ctemp1 > 0x0a) return -1;		// 修改波特率失败
					} 
				chip_uart_ms = 0; 				// 设置GM8125工作在多通道工作模式下
    	
				P2MDOUT = 0xff;					// 将 P2.7、P2.6、P2.5、P2.4、P2.3 P2.2 P2.1 P2.0 设置为推挽方式
//				P2 = 0x08;						// 
				P2 = P2 | 0x08;
				P4 = 0xff;
		
				ES0 = 0;		
				TR2 = 0;
				RCAP2H = 0xff;
				switch (g_config.m_comm_param[port-1].m_baud) {
					case B_1200:						//								230400	115200	76800	57600	19200	9600	7200	4800	2400	1200（Page 213）
						RCAP2L = 0xa0;					// 定时器 2 捕捉寄存器低字节	0xfd	0xfa	0xf7	0xf4	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;
					case B_2400:
						RCAP2L = 0xd0;					// 定时器 2 捕捉寄存器低字节	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;
					case B_4800:
						RCAP2L = 0xe8;					// 定时器 2 捕捉寄存器低字节	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;
					case B_9600:
						RCAP2L = 0xf4;					// 定时器 2 捕捉寄存器低字节	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;		
					case B_19200:
						RCAP2L = 0xfa;					// 定时器 2 捕捉寄存器低字节	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;
					default: RCAP2L = 0xf4;
					
					}								
				TR2 = 1;
				ES0 = 1;
				in_init(port);
				out_init(port);
				ctemp1 = 0x01;
				for (i=0x01;i<port;i++) ctemp1 = ctemp1 << 1;
				flag1_uart0 = flag1_uart0 | ctemp1;				// 将对应端口正在接收数据标志置位
				comm_baud = g_config.m_comm_param[port-1].m_baud;
				return port;
				}
			else return -1;										// 当前串口有正在使用的，不可以更改波特率
			}
		}
	else {
		if (i<=8) {
			in_init(port);
			out_init(port);
			return port;
			}
		else return -1;
		}
	}

//061016/*********************************************************************************************
//061016*	原型：int close_comm(int fd)
//061016*	功能: 关闭 fd 对应的通信端口
//061016*	说明：
//061016*	参数：无                  
//061016*	返回：无                  
//061016*********************************************************************************************/
//061016int close_comm(int fd)
//061016{
//061016unsigned char i,ctemp1;
//061016	i = (char)fd;
//061016	if (i<=5) {
//061016		ctemp1 = 0x01;
//061016		for (i=0x01;i<fd;i++) ctemp1 = ctemp1 << 1;
//061016		flag0_uart0 = ~ctemp1 & flag0_uart0;				// 将对应端口正在发送数据标志清零
//061016		flag1_uart0 = ~ctemp1 & flag1_uart0;				// 将对应端口正在接收数据标志清零		
//061016//		return 0;
//061016		}
//061016	else {
//061016		if (i==6) {
//061016			
//061016			}
//061016		else {
//061016			if (i<=8) {
//061016				
//061016				}
//061016			else return -1;
//061016			}
//061016		}
//061016	return 0;	
//061016	}

/*********************************************************************************************
*	原型：int read_comm(int fd,byte*  in_buffer, int size)
*	功能: 从串口 x 读取数据
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int read_comm(int fd,byte*  in_buffer, int size)
{
unsigned char j;
unsigned int i;
	if(size <1)	return size;
	j = (char)fd;	
	if (j <= 8) {									// 
		if (j <= 5) ES0 = 0;						// 关串口 0 中断
		else {
			if (j==6) EIE2 = EIE2 & 0xbf;			// ES1 = 0  关串口 1 中断
			else EX0 = 0;
			}
		
		i=0;
		while(!is_in_empty(fd)){
			in_buffer[i++] = in_getchar(fd);
			if(i==size)
				break;
			}
		
		if (j <= 5) ES0 = 1;						// 开串口 0 中断
		else {
			if (j==6) EIE2 = EIE2 | 0x40;			// ES1 = 1 开串口 1 中断
			else EX0 = 1;
			}
//		ES0 = 1;									// 
		} 
	else return -1;									// 接收端口超过 5 出错
	return i;										// 返回读回的字节数
	}

/*********************************************************************************************
*	原型：int write_comm(int fd,byte* out_buffer, int size)
*	功能: 向串口 x 写数据
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int write_comm(int fd,byte* out_buffer, int size)
{
unsigned char j,ctemp1,k;
unsigned char xdata * data pchar;
unsigned int i;
	if(size < 1) return size;
	k = (char)fd;
	if (k>8) return -1;											// 端口超过 8 出错
	
	i=0;	

	EIE2 = EIE2 & 0xef;											// 关外部中断 6
	while(!is_out_full(fd)){
		out_putchar(fd,out_buffer[i++]);
		if(i==size)	break;
		}
	EIE2 = EIE2 | 0x10;											// 开外部中断 6


	if (k <= 5) {												// 串口 0 扩展的 5 个子串口
		ctemp1 = 0x01;
		for (j=0x01;j<fd;j++) ctemp1 = ctemp1 << 1;
		if ((flag0_uart0 & 0x1f) == 0x00) {						// 各子串口都没数据发送，要启动串口
			uart0_chanl_sel = fd << 3;
			UART_CHANL = (UART_CHANL & 0xc7) | uart0_chanl_sel;	// 选择要发送的端口
			SBUF0 = uart0_trans_buffer[fd - 1][g_out_read_p[fd-1]++];
			if(g_out_read_p[fd-1] == uart0_trans_max) g_out_read_p[fd-1] = 0;

			}
		flag0_uart0 = flag0_uart0 | ctemp1;						// 将对应端口有数据发送标志置位
		}
	else {
		switch (k) {
			case 6:{											// 串口 1 
				if (!f_uart1_using) {
					SBUF1 = uart0_trans_buffer[5][g_out_read_p[5]++];
					if(g_out_read_p[5] == uart0_trans_max) g_out_read_p[5] = 0;		
					f_uart1_using = 1;
					}
				break;
				}
			case 7:{											// CPLD 端口 0
				if (!f_cpld_chanl0_using) {
					pchar = ADDRESS_PC_WR0;
					EIE2 = EIE2 & 0xef;							// 关外部中断 6		
					f_cpld_trans_num = 1;
					cpld_trans_bak = uart0_trans_buffer[6][g_out_read_p[6]++];
					*pchar = cpld_trans_bak;
					if(g_out_read_p[6] == uart0_trans_max) g_out_read_p[6] = 0;		
					EIE2 = EIE2 | 0x10;							// 开外部中断 6
					f_cpld_chanl0_using = 1;
					cpld_count = 0x00;
					}
				break;
				}
			default:{											// CPLD 端口 1
				if (!f_cpld_chanl1_using) {
					pchar = ADDRESS_PC_WR1;
					EIE2 = EIE2 & 0xef;	
					f_cpld_trans_num = 0;
					cpld_trans_bak = uart0_trans_buffer[7][g_out_read_p[7]++];
					*pchar = cpld_trans_bak;
					if(g_out_read_p[7] == uart0_trans_max) g_out_read_p[7] = 0;	
					EIE2 = EIE2 | 0x10;	
					f_cpld_chanl1_using = 1;
					cpld_count = 0x00;
					}	
				}
			}
		} 
	return i;
	}

/*********************************************************************************************
*	原型：int is_manual_control_state()
*	功能: 是否处于手动控制状态
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int is_manual_control_state()
{
	if (f_manual) return 1;
	else return 0;
	}

/*********************************************************************************************
*	原型：unsigned char get_front_panel_control_mode()
*	功能: 前面板设定的控制模式
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
unsigned char get_front_panel_control_mode()
{
unsigned char ctemp0;
//	if (!f_manual) {
		switch(key0_val) {
			case 0: ctemp0 = ONLINE_HARMONY_CONTROL;break;				// 联机协调控制
			case 1: ctemp0 = ALL_RED_CONTROL;break;						// 全红控制
			case 2: ctemp0 = YELLOW_FLASH_CONTROL;break;				// 黄闪控制
			case 3: ctemp0 = MULTIPLE_SCHEDULE_CONTROL;break;			// 多段控制
			case 4: ctemp0 = ACTUATE_CONTROL;break;						// 感应控制			
			case 5: ctemp0 = 0;break;									// 返回
			default : ctemp0 = 0;break;
			}
// 090803 zjl			
//		if (ctemp0 != 0) g_config.m_control_mode_selection.m_control_mode = ctemp0;
		
		return ctemp0;
//		}
//	else return 0;
	}

/*********************************************************************************************
*	原型：void front_panel_dis(unsigned char ch)
*	功能: 前面板控制模式显示（led 指示灯）
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void front_panel_dis(unsigned char ch)
{
unsigned char ctemp0;
unsigned char xdata * data pchar;
static unsigned char mode_bak;
// 060612
	if ((ch != mode_bak) | g_controller_status.m_is_degraded) {
		mode_bak = ch;
		f_key0_ed = 0;													// 
		f_key1_ed = 0;
		TELECONTROL_BAK = 0x00;
		}
// 060612
	switch(ch) {
			case 1: ctemp0 = 0x40;break;								// 灭灯控制
			case 2: ctemp0 = 0x04;break;								// 全红控制
			case 3: ctemp0 = 0x08;break;								// 黄闪控制
			case 4: ctemp0 = 0x10;break;								// 多段控制			
			case 5: ctemp0 = 0x20;break;								// 感应控制
//			case 6: ctemp0 = ;break;									// 
			case 7: ctemp0 = 0x02;break;								// 联机协调控制
			case 8: ctemp0 = 0x01;break;								// 手动控制
			default : ctemp0 = 0;break;
		}
	pchar = ADDRESS_FRONT_PANEL;
	*pchar = ctemp0;
	}
/*********************************************************************************************
*	原型：int is_step_forward_button_pressed()
*	功能: 是否按了步进按钮
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int is_step_forward_button_pressed()
{
//	if (f_manual) {
		if (f_step) {
//			f_step = 0;
			return 1;
			}
//		}
	return 0;
}

/*********************************************************************************************
*	原型：int clear_step_forward_button_state()
*	功能: 清除步进按钮状态
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int clear_step_forward_button_state()
{
	f_step = 0;
	return 0;
}
/*********************************************************************************************
*	原型：int is_need_change_stage()
*	功能: 是否需要改变到下个状态
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int is_need_change_stage()
{
	if (f_manual)
	{
		if (f_manual_step)
		{
			return 1;
		}
	}
	return 0;
}
/*********************************************************************************************
*	原型：int is_need_change_stage_pressed()
*	功能: 
*	说明：
*	参数：无                  
*	返回：需要改变到的相位数                  
*********************************************************************************************/
int get_manual_next_stage_count()
{
	return manual_set_stage;
}
/*********************************************************************************************
*	原型：int clear_need_change_stage_flag()
*	功能: 清除需要改变到下个相位的标志
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int clear_need_change_stage_flag()
{
	f_manual_step = 0;
	return 0;
}
///*********************************************************************************************
//*	原型：unsigned int cal_crc(unsigned char *ptr, unsigned char len)
//*	功能: 完成 CRC 校验
//*	说明：
//*	参数：无                  
//*	返回：无                  
//*********************************************************************************************/
//unsigned int cal_crc(unsigned char *ptr, unsigned char len) 
//{
//unsigned int crc;
//unsigned char da;
//	crc=0;
//	while(len--!=0) {
//		da = ((char)(crc/256))/16; 							// 暂存CRC 的高四位 
//		crc <<= 4; 											// CRC 右移4 位，相当于取CRC 的低12 位）
//		crc ^= crc_ta[da^(*ptr/16)]; 						// CRC 的高4 位和本字节的前半字节相加后查
//															// 表计算CRC，然后加上上一次CRC 的余数
//		da = ((char)(crc/256))/16; 							// 暂存CRC 的高4 位 
//		crc <<= 4; 											// CRC 右移4 位， 相当于CRC 的低12 位）
//		crc ^= crc_ta[da^(*ptr&0x0f)]; 						// CRC 的高4 位和本字节的后半字节相加后查
//															// 表计算CRC，然后再加上上一次CRC 的余数
//		Ptr++;
//		}
//	return(crc);
//	}

// 修改纪录
// 090611 修改保留灯故障现场，只保留20秒，20s后重新检测
// 090407 修改检测器计数方式。原来检测器的计数是有检测器板上的硬件计数器来完成，修改为由单片机软件计数，来避免由于8255不能
//        正常复位时，感应控制不正常的问题
// 090224 修改保留灯故障现场。原来只要检测到有灯故障就一直停留在这个状态，修改为继续检测
// 071213 修改由感应控制到手动，再由手动返回时，不经过黄闪、全红阶段，而是顺着原来的顺序
// 071210 增加原来由遥控器控制直接跳到某个绿灯放行步时，前面加上缓冲绿闪、黄灯、全红阶段
// 070301 调试信号机背板。每个灯控板、车辆检测板中的 74HC244 及车辆检测板中的 82C55片选都要加一个上拉电阻，
//        否则系统控制板没电或者背板与系统板没连接时，这些芯片的片选悬空呈现不定状态，造成总线冲突，使电流
//        加大，长期会使部分芯片损坏。目前，该上拉电阻加在系统板上，阻值为 2K。灯控板、车辆检测板加上拉电阻
//        时，应该将系统板上的上拉电阻取消，或者系统板上的上拉电阻取较大的值，为对应灯控板、车辆检测板上的上
//        拉电阻十倍以上，如灯控板、车辆检测板上的上拉电阻取2K，系统板上的上拉电阻取20K 以上，否则系统板停电
//        时，由于这两个电阻的分压关系，会使片选端的电压拉成低电平
// 070203 前面板 a /d 输入分压电阻有改变，R28、R7 由 1K 改为 2.7K；R29、R8由1K 改为 4.3K
// 070202 遥控器检测输入 244 片选漏加上拉电阻，前面板状态显示驱动 373片选漏加上拉电阻
// 061013 调快绿闪的频率（mcu.c 有改）
// 061011 增加遥控器可直接控制某一方向绿灯，即任何情况下，摁一下遥控器对应的按键即可以放行对应方向的车流
// 061011 退出时需按解除手动按键，转为自动控制即可。只能用于四相位和两相位，且相位步必须依次是全红、绿灯、
// 061011 绿闪、黄灯时才可以用（mcu.c 有改）
// 061010 感应控制时，不判断端子号为 9 18 27 36 等是否接绿灯，这样可将这些端子设成常绿。否则感应控制时，
// 061010 任何端子都不能设为常绿（actuate_control.c 有改）
// 060615 修改原来在前端设置为"手动控制"时，中心就无法再改变控制模式（smbus.c 有改） 
// 060613 增加每次只要前端控制模式输入有效时，就重新调用该控制模式的算法（预设与实际运行都为感应控制时，不再重行调用，
// 060613 因为重行调用会有全红出现），原来只有在控制模式改变时，才调用（mcu.c、kernel.c 有改）
// 060612 将原键盘输入（AD） 0、1 锁存，改为不锁存，硬件将 74HC573(U4) 的 LE 端固定接高电平，软件则增加每次模式改变，
// 060612 清除上次设定的标志位，且处于降级运行时，也清除标志位，为重新输入与上次预设模式相同的控制模式。

// 060612 kernel /fb-linux.img ro root=/dev/hdb2 single vga=785 video=vesa:ypan init=/bin/sh
// 复制 DOC 盘方法
// cd /util
// /util/bin/insmod doc.o
// cp fla.img /dev/msys/fla 
// sync


// 071212
//	int fun(int someVal);
//	int main(){
//	　fun(10);
//	}
//	然而你可能需要在某个时刻将函数指针保存下来，并在以后的另一个时刻调用它，像这样：
//	int fun(int);
//	typedef int (*func_handle)(int);
//	int main(){
//	　func_handle fh=fun;
//	　... //do something
//	　fh(10);
//	}
//	如果fun形式为void fun(int)呢？如你所见，fun可能有无数种形式，对fun的每一个形式都typedef一个对应的func_handle
// 071212

//
//	函数名与函数指针                                                                                                                                                                                                            
//	                                                                                                                                                                                                                            
//	一 通常的函数调用                                                                                                                                                                                                           
//	    一个通常的函数调用的例子：                                                                                                                                                                                              
//	//自行包含头文件                                                                                                                                                                                                            
//	void MyFun(int x);    //此处的申明也可写成：void MyFun( int );                                                                                                                                                              
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //这里是调用MyFun(10);函数                                                                                                                                                                                
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	void MyFun(int x)  //这里定义一个MyFun函数                                                                                                                                                                                  
//	{                                                                                                                                                                                                                           
//	   printf(“%d\n”,x);                                                                                                                                                                                                      
//	}                                                                                                                                                                                                                           
//	    这个MyFun函数是一个无返回值的函数，它并不完成什么事情。这种调用函数的格式你应该是很熟悉的吧！看主函数中调用MyFun函数的书写格式：                                                                                        
//	MyFun(10);                                                                                                                                                                                                                  
//	    我们一开始只是从功能上或者说从数学意义上理解MyFun这个函数，知道MyFun函数名代表的是一个功能（或是说一段代码）。                                                                                                          
//	    直到——                                                                                                                                                                                                                
//	    学习到函数指针概念时。我才不得不在思考：函数名到底又是什么东西呢？                                                                                                                                                      
//	    （不要以为这是没有什么意义的事噢！呵呵，继续往下看你就知道了。）                                                                                                                                                        
//	                                                                                                                                                                                                                            
//	二 函数指针变量的申明                                                                                                                                                                                                       
//	    就象某一数据变量的内存地址可以存储在相应的指针变量中一样，函数的首地址也以存储在某个函数指针变量里的。这样，我就可以通过这个函数指针变量来调用所指向的函数了。                                                          
//	    在C系列语言中，任何一个变量，总是要先申明，之后才能使用的。那么，函数指针变量也应该要先申明吧？那又是如何来申明呢？以上面的例子为例，我来申明一个可以指向MyFun函数的函数指针变量FunP。下面就是申明FunP变量的方法：      
//	void (*FunP)(int) ;   //也可写成void (*FunP)(int x);                                                                                                                                                                        
//	    你看，整个函数指针变量的申明格式如同函数MyFun的申明处一样，只不过——我们把MyFun改成（*FunP）而已，这样就有了一个能指向MyFun函数的指针FunP了。（当然，这个FunP指针变量也可以指向所有其它具有相同参数及返回值的函数了。）
//	                                                                                                                                                                                                                            
//	三 通过函数指针变量调用函数                                                                                                                                                                                                 
//	    有了FunP指针变量后，我们就可以对它赋值指向MyFun，然后通过FunP来调用MyFun函数了。看我如何通过FunP指针变量来调用MyFun函数的：                                                                                             
//	//自行包含头文件                                                                                                                                                                                                            
//	void MyFun(int x);    //这个申明也可写成：void MyFun( int );                                                                                                                                                                
//	void (*FunP)(int );   //也可申明成void(*FunP)(int x)，但习惯上一般不这样。                                                                                                                                                  
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //这是直接调用MyFun函数                                                                                                                                                                                   
//	   FunP=&MyFun;  //将MyFun函数的地址赋给FunP变量                                                                                                                                                                            
//	   (*FunP)(20);    //这是通过函数指针变量FunP来调用MyFun函数的。                                                                                                                                                            
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	void MyFun(int x)  //这里定义一个MyFun函数                                                                                                                                                                                  
//	{                                                                                                                                                                                                                           
//	   printf(“%d\n”,x);                                                                                                                                                                                                      
//	}                                                                                                                                                                                                                           
//	    请看黑体字部分的代码及注释。                                                                                                                                                                                            
//	    运行看看。嗯，不错，程序运行得很好。                                                                                                                                                                                    
//	    哦，我的感觉是：MyFun与FunP的类型关系类似于int 与int *的关系。函数MyFun好像是一个如int的变量（或常量），而FunP则像一个如int *一样的指针变量。                                                                           
//	int i,*pi;                                                                                                                                                                                                                  
//	pi=&i;    //与FunP=&MyFun比较。                                                                                                                                                                                             
//	    （你的感觉呢？）                                                                                                                                                                                                        
//	    呵呵，其实不然——                                                                                                                                                                                                      
//	                                                                                                                                                                                                                            
//	四 调用函数的其它书写格式                                                                                                                                                                                                   
//	函数指针也可如下使用，来完成同样的事情：                                                                                                                                                                                    
//	//自行包含头文件                                                                                                                                                                                                            
//	void MyFun(int x);                                                                                                                                                                                                          
//	void (*FunP)(int );    //申明一个用以指向同样参数，返回值函数的指针变量。                                                                                                                                                   
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //这里是调用MyFun(10);函数                                                                                                                                                                                
//	   FunP=MyFun;  //将MyFun函数的地址赋给FunP变量                                                                                                                                                                             
//	   FunP(20);    //这是通过函数指针变量来调用MyFun函数的。                                                                                                                                                                   
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	void MyFun(int x)  //这里定义一个MyFun函数                                                                                                                                                                                  
//	{                                                                                                                                                                                                                           
//	   printf(“%d\n”,x);                                                                                                                                                                                                      
//	}                                                                                                                                                                                                                           
//	我改了黑体字部分（请自行与之前的代码比较一下）。                                                                                                                                                                            
//	运行试试，啊！一样地成功。                                                                                                                                                                                                  
//	咦？                                                                                                                                                                                                                        
//	FunP=MyFun;                                                                                                                                                                                                                 
//	可以这样将MyFun值同赋值给FunP，难道MyFun与FunP是同一数据类型（即如同的int 与int的关系），而不是如同int 与int*的关系了？（有没有一点点的糊涂了？）                                                                           
//	看来与之前的代码有点矛盾了，是吧！所以我说嘛！                                                                                                                                                                              
//	请容许我暂不给你解释，继续看以下几种情况（这些可都是可以正确运行的代码哟！）：                                                                                                                                              
//	代码之三：                                                                                                                                                                                                                  
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //这里是调用MyFun(10);函数                                                                                                                                                                                
//	   FunP=&MyFun;  //将MyFun函数的地址赋给FunP变量                                                                                                                                                                            
//	   FunP(20);    //这是通过函数指针变量来调用MyFun函数的。                                                                                                                                                                   
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	代码之四：                                                                                                                                                                                                                  
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //这里是调用MyFun(10);函数                                                                                                                                                                                
//	   FunP=MyFun;  //将MyFun函数的地址赋给FunP变量                                                                                                                                                                             
//	   (*FunP)(20);    //这是通过函数指针变量来调用MyFun函数的。                                                                                                                                                                
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	真的是可以这样的噢！                                                                                                                                                                                                        
//	（哇！真是要晕倒了！）                                                                                                                                                                                                      
//	还有呐！看——                                                                                                                                                                                                              
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   （*MyFun）(10);     //看，函数名MyFun也可以有这样的调用格式                                                                                                                                                              
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	你也许第一次见到吧：函数名调用也可以是这样写的啊！（只不过我们平常没有这样书写罢了。）                                                                                                                                      
//	那么，这些又说明了什么呢？                                                                                                                                                                                                  
//	呵呵！假使我是“福尔摩斯”，依据以往的知识和经验来推理本篇的“新发现”，必定会由此分析并推断出以下的结论：                                                                                                                  
//	1. 其实，MyFun的函数名与FunP函数指针都是一样的，即都是函数指针。MyFun函数名是一个函数指针常量，而FunP是一个函数数指针变量，这是它们的关系。                                                                                 
//	2. 但函数名调用如果都得如(*MyFun)(10)；这样，那书写与读起来都是不方便和不习惯的。所以C语言的设计者们才会设计成又可允许MyFun(10);这种形式地调用（这样方便多了并与数学中的函数形式一样，不是吗？）。                          
//	3. 为统一起见，FunP函数指针变量也可以FunP(10)的形式来调用。                                                                                                                                                                 
//	4. 赋值时，即可FunP=&MyFun形式，也可FunP=MyFun。                                                                                                                                                                            
//	上述代码的写法，随便你爱怎么着！                                                                                                                                                                                            
//	请这样理解吧！这可是有助于你对函数指针的应用喽！                                                                                                                                                                            
//	最后——                                                                                                                                                                                                                    
//	补充说明一点：在函数的申明处：                                                                                                                                                                                              
//	void MyFun(int );    //不能写成void (*MyFun)(int )。                                                                                                                                                                        
//	void (*FunP)(int );   //不能写成void FunP(int )。                                                                                                                                                                           
//	（请看注释）这一点是要注意的。                                                                                                                                                                                              
//	                                                                                                                                                                                                                            
//	五 定义某一函数的指针类型：                                                                                                                                                                                                 
//	就像自定义数据类型一样，我们也可以先定义一个函数指针类型，然后再用这个类型来申明函数指针变量。                                                                                                                              
//	我先给你一个自定义数据类型的例子。                                                                                                                                                                                          
//	typedef int* PINT;    //为int* 类型定义了一个PINT的别名                                                                                                                                                                     
//	int main()                                                                                                                                                                                                                  
//	{                                                                                                                                                                                                                           
//	  int x;                                                                                                                                                                                                                    
//	  PINT px=&x;   //与int * px=&x;是等价的。PINT类型其实就是int * 类型                                                                                                                                                        
//	  *px=10;       //px就是int*类型的变量                                                                                                                                                                                      
//	  return 0;                                                                                                                                                                                                                 
//	}                                                                                                                                                                                                                           
//	根据注释，应该不难看懂吧！（虽然你可能很少这样定义使用，但以后学习Win32编程时会经常见到的。）                                                                                                                               
//	下面我们来看一下函数指针类型的定义及使用：（请与上对照！）                                                                                                                                                                  
//	//自行包含头文件                                                                                                                                                                                                            
//	void MyFun(int x);    //此处的申明也可写成：void MyFun( int );                                                                                                                                                              
//	typedef void (*FunType)(int );   //这样只是定义一个函数指针类型                                                                                                                                                             
//	FunType FunP;              //然后用FunType类型来申明全局FunP变量                                                                                                                                                            
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	//FunType FunP;    //函数指针变量当然也是可以是局部的 ，那就请在这里申明了。                                                                                                                                                
//	   MyFun(10);                                                                                                                                                                                                               
//	   FunP=&MyFun;                                                                                                                                                                                                             
//	   (*FunP)(20);                                                                                                                                                                                                             
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	void MyFun(int x)                                                                                                                                                                                                           
//	{                                                                                                                                                                                                                           
//	   printf(“%d\n”,x);                                                                                                                                                                                                      
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	看黑体部分：                                                                                                                                                                                                                
//	首先，在void (*FunType)(int ); 前加了一个typedef 。这样只是定义一个名为FunType函数指针类型，而不是一个FunType变量。                                                                                                         
//	然后，FunType FunP;  这句就如PINT px;一样地申明一个FunP变量。                                                                                                                                                               
//	其它相同。整个程序完成了相同的事。                                                                                                                                                                                          
//	这样做法的好处是：                                                                                                                                                                                                          
//	有了FunType类型后，我们就可以同样地、很方便地用FunType类型来申明多个同类型的函数指针变量了。如下：                                                                                                                          
//	FunType FunP2;                                                                                                                                                                                                              
//	FunType FunP3;                                                                                                                                                                                                              
//	//……                                                                                                                                                                                                                      
//	                                                                                                                                                                                                                            
//	六 函数指针作为某个函数的参数                                                                                                                                                                                               
//	既然函数指针变量是一个变量，当然也可以作为某个函数的参数来使用的。所以，你还应知道函数指针是如何作为某个函数的参数来传递使用的。                                                                                            
//	给你一个实例：                                                                                                                                                                                                              
//	要求：我要设计一个CallMyFun函数，这个函数可以通过参数中的函数指针值不同来分别调用MyFun1、MyFun2、MyFun3这三个函数（注：这三个函数的定义格式应相同）。                                                                       
//	实现：代码如下：                                                                                                                                                                                                            
//	//自行包含头文件                                                                                                                                                                                                            
//	void MyFun1(int x);                                                                                                                                                                                                         
//	void MyFun2(int x);                                                                                                                                                                                                         
//	void MyFun3(int x);                                                                                                                                                                                                         
//	typedef void (*FunType)(int ); //②. 定义一个函数指针类型FunType,与①函数类型一至                                                                                                                                           
//	void CallMyFun(FunType fp,int x);                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   CallMyFun(MyFun1,10);   //⑤. 通过CallMyFun函数分别调用三个不同的函数                                                                                                                                                    
//	   CallMyFun(MyFun2,20);                                                                                                                                                                                                    
//	   CallMyFun(MyFun3,30);                                                                                                                                                                                                    
//	}                                                                                                                                                                                                                           
//	void CallMyFun(FunType fp,int x) //③. 参数fp的类型是FunType。                                                                                                                                                              
//	{                                                                                                                                                                                                                           
//	  fp(x);//④. 通过fp的指针执行传递进来的函数，注意fp所指的函数是有一个参数的                                                                                                                                                
//	}                                                                                                                                                                                                                           
//	void MyFun1(int x) // ①. 这是个有一个参数的函数，以下两个函数也相同                                                                                                                                                        
//	{                                                                                                                                                                                                                           
//	   printf(“函数MyFun1中输出：%d\n”,x);                                                                                                                                                                                    
//	}                                                                                                                                                                                                                           
//	void MyFun2(int x)                                                                                                                                                                                                          
//	{                                                                                                                                                                                                                           
//	   printf(“函数MyFun2中输出：%d\n”,x);                                                                                                                                                                                    
//	}                                                                                                                                                                                                                           
//	void MyFun3(int x)                                                                                                                                                                                                          
//	{                                                                                                                                                                                                                           
//	   printf(“函数MyFun3中输出：%d\n”,x);                                                                                                                                                                                    
//	}

//                                                                                                                                                                                                                           