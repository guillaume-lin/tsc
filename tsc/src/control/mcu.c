#include "vm.h"
#include "mcu.h"
#include "kernel.h"
#include "error_code.h"
#include "datastruct.h"

char bdata flag_key0;
sbit f_key0_err 					= flag_key0^0;		// �����м�ֵ���п��ܳ���(����ϼ�)	
sbit f_key1_err 					= flag_key0^1;		// �����м�ֵ���п��ܳ���(����ϼ�)	
sbit f_manual 						= flag_key0^2;		// �ֶ�ģʽ���ñ�־λ��������ƣ�ֻΪ�Ժ���ʾ�ã�
sbit f_key0_ed 						= flag_key0^3;		// ģ������ 0 һ����Ч����
sbit f_key1_ed 						= flag_key0^4;		// ģ������ 1 һ����Ч����
sbit f_key2_ed 						= flag_key0^5;		// ģ������ 2 һ����Ч����
sbit f_key3_ed						= flag_key0^6;		// ģ������ 7 һ����Ч����
sbit f_step 						  = flag_key0^7;		// �в������Ƽ�����

bit f_key,f_key_now,f_buzz,flick_mark,f_auto_buzz;
bit SYS5ms,SYS25ms,SYS100ms,SYS200ms,f_rd_lamp_input_now;
bit f_uart1_using,f_cpld_chanl0_using,f_cpld_chanl1_using;

bit f_key0_int_bak,f_key1_int_bak,f_key2_int_bak,f_key3_int_bak;

bit f_cpld_trans_ed,f_cpld_trans_num;
bit f_manual_auto;										// �ֶ�/�Զ�ת��һ����Ч

bit f_manual_step; 										// ң������ֱ�ӷ���ĳ��λ���±�־
bit f_return_actuate_control;							// ���ֶ����ظ�Ӧ���Ʊ�־

bit f_run_mode,f_lamp_yellow_err,f_lamp_green_or_red_err,f_lamp_err,f_boot,f_key_val,f_mode_switch;

bit f_1s;
unsigned char delay_1s;
// unsigned char lamp_err_delay;

unsigned char TELECONTROL_BAK;							// ң�������뱸��
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

unsigned char data flick_count;													// ������ʱ�������ñ���ֵ�ɸı����Ƶ�� 
unsigned char lamp_output_buffer[2*((MAX_LAMP_NUM - 1)/12 + 1)];				// ��ͨ���������
unsigned char lamp_output_buffer1[2*((MAX_LAMP_NUM - 1)/12 + 1)];				// ��ͨ����� 
unsigned char lamp_flick_buffer[2*((MAX_LAMP_NUM - 1)/12 + 1)];					// ��ͨ������������� 
unsigned char lamp_input_buffer[11][2*((MAX_LAMP_NUM - 1)/12 + 1)];				// ��ͨ��״̬���뻺�壨��0--6�б������7�ζ���ֵ������8�д��
		// ���Ƶ�ʵ��״̬������ʵ�е��ǰ벨(10ms)���, 3ms һ�Σ���ͨ�����е����������Ե����ݱȽϣ����жϵƵĺû����������̳�ͻ���

unsigned char detector_err[4],detector_flag[4],read_detector_time,detector_time;
unsigned char detector_count_bak0[MAX_DETECTOR_COUNT];

unsigned char cpld_count,cpld_trans_bak;

unsigned char lamp_health_status[((MAX_LAMP_NUM-1)/8)+1];

unsigned char manual_set_stage;							// ң������ֱ�ӷ���ĳ��λ
unsigned char manual_m_control_mode_bak;				// �����ֶ�ǰ�Ŀ���ģʽ
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
								};						// CRC ��ʽ�� 

/*********************************************************************************************
*	ԭ�ͣ�int initialize(void)
*	����: ��ɫ������ƣ�ˢ�£��ӳ���
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
int initialize(void)
{
	unsigned char i;
	unsigned char xdata * data pp;

//������ʼ��
	OSCXCN = 0x67;					//����������ʽ������Ƶ�� f > 6.7MHz ��Page 121��
	while ((OSCXCN & 0x80) != 0x80);//ֱ�����������������в��ҹ����ȶ�

	OSCICN = 0x88;					//ѡ���ⲿ������Ϊϵͳʱ�ӣ��ڲ�������ֹ������ʱ�Ӷ�ʧ���������⵽ʱ�Ӷ�ʧʱ�����100 ΢��ʱ��������λ��

//��λԴ	
//	RSTSRC

	WDTCN = 0xde;					// disable watchdog timer
	WDTCN = 0xad;           		
                            		
//��ѹ��׼��ʼ��            		
	REF0CN = 0x07;					// ADC0 ��ѹ��׼ȡ��VREF0 ���ţ��ڲ��¶ȴ�������ƫѹ����������ѹ��׼������������Page 82��
         
                            		
//�˿ڳ�ʼ��                		    
	XBR0 = 0x0d;					// ���䴮��0(2�����)��IIC(2�����)��PCA0(1�����)���˿����ţ�Page 152�� 
	XBR1 = 0x14;					// �����ж�0���ж�1���˿����ţ�Page 153��
	XBR2 = 0x84;					// ���䴮��1���˿����ţ�������ȫ�ֽ�ֹ��Page 154�� 
	P1MDIN = 0xff;					// �˿�P1���ű�����Ϊ�������������ʽ��Page 156��
	XBR2 = XBR2 | 0x40;				// ʹ�ܽ��濪��
	                        			
	P0MDOUT = 0x51;					// ��UART0 ��TX ����TX0 P0.0��UART1 ��TX ����TX1 P0.4��PCA0 ���������CEX0 P0.6������Ϊ���췽ʽ��Page 155��
//	P0 = P0 | 0xae;	
	P0 = 0xae;				
	P1MDOUT = 0xc8;					// �� P1.3��P1.6��P1.7 ����Ϊ���췽ʽ����������Ϊ OC �����ʽ��Page 157��
//	P1 = P1 | 0x3f;         		// �� P1.0��P1.1��P1.2��P1.4��P1.5 ����Ϊ��������
	P1 = 0x3f;
	P2MDOUT = 0xff;					// �� P2.7��P2.6��P2.5��P2.4��P2.3��P2.2��P2.1��P2.0 ����Ϊ���췽ʽ��Page 157��
	P2 = 0x40;
//	P2 = P2 | 0x80;					// �� P2.7 ����Ϊ��������         			
	P3MDOUT = 0x8f;					// ��P3.7��P3.3��P3.2��P3.1��P3.0 ����Ϊ���췽ʽ��Page 158��
//	P3 = P3 | 0x30;         	
	P3 = 0x30;
	
	P3IF = 0x04;
	
	chip_uart_rst = 0;				// ��������չоƬ�͸�λ�ź� 
	chip_uart_rst = 1;				// ��������չоƬ�͸�λ�ź� 
	P74OUT = 0x3e;   				// �� P4 �ĸ���λ��P5��P6����Ϊ���췽ʽ��P4 �ĵ���λ�� P7 ����Ϊ OC �����ʽ��Page 161��
	P7 = 0xff;        
	
//�ⲿ�洢���ӿ����ó�ʼ��  		
//	EMI0CN = 0x??					//XRAM ҳѡ��EMI0CN
	EMI0CF = 0x38;			//0x3c;		//0x38;					//�洢���ӿ������� P4-P7�������ڷǸ��á�����ѡ��ķ�Ƭ��ʽ��Page 130��

	EA = 0;							//��ֹ�����ж�
//	FLSCL = FLSCL | 0X01;			//�������û�����д/����FLASH
//	PSCTL = PSCTL | 0x02;			//����FLASH ����������Page 123��
//	PSCTL = PSCTL | 0x01;			//����FLASH д��Page 123��
//	                            	
//                                	
//	PSCTL = PSCTL & 0xfd;			//��ֹFLASH ����������Page 123��
//	PSCTL = PSCTL & 0xfe;			//��ֹFLASH д��Page 123��
//	FLSCL = FLSCL & 0xfe;			//��ֹ���û�����д/����FLASH��Page 123��
//	
//	PSCTL = PSCTL | 0x04;			//���û���������FLASH ʱ��������128 �ֽڵ���ʱ�洢��������EEPROM����Page 123��
	EMI0TC = 0x2c;		//1c;		//0x20			// 0x7d;	//3c;	//0c;	//�ⲿ�洢���ӿڵ�ʱ�������Page 135��

//�ɱ�̼��������У�PCA0����ʼ����������һ����� CEX0 ������ P0.4��
	PCA0CN = PCA0CN | 0x40;			//����PCA0 ��������ʱ����Page 233��
	PCA0MD = 0x80;					//��ϵͳ���������ڵȴ���ʽʱ��PCA0 ֹͣ������PCA0 ��������ʱ��Ϊϵͳʱ�ӵ� 12 ��Ƶ����ֹCF �жϣ�Page 234��
	PCA0CPM0 = 0x46;				//����Ƶ�������ʽ����ֹ CCF0 �жϣ�Page 235��
	PCA0CPL0 = 0x5c;				//PCA0 ��׽ģ����ֽڣ�Page 236��
	PCA0CPH0 = 0x5c;				//PCA0 ��׽ģ����ֽڣ�Page 236��
	
//ADC0 ��ʼ�������� 1 �� Ain0.0 ����,���� 2 �� Ain0.1 ����
	ADC0CF = 0x79;					//CLKSAR0 Ϊ 2.21184MHz���ڲ��Ŵ�������Ϊ 2 ��Page 55��
	ADC0CN = 0x85;					//ADC0 ����һֱ���ٷ�ʽ����ʱ�� 3 �������ADC0 ת����ADC0H:ADC0L �Ĵ������������
//	ADC0CN = 0xc4;					//ADC0 ������������ٷ�ʽ����ʱ�� 3 �������ADC0 ת����ADC0H:ADC0L �Ĵ��������Ҷ���
	AMX0CF = 0x00;					//ȫ�����루Ain0.0 - Ain0.7�������ڵ��˷�ʽ��Page 53��
	                        		
	AMX0SL = 0x00;					//AMUX ѡ�� Ain0.0������ 0 ����ˣ���Page 54��
//	AMX0SL = 0x01;					//AMUX ѡ�� Ain0.1������ 1 ����ˣ���Page 54��
//	AMX0SL = 0x02;					//AMUX ѡ�� Ain0.2������ 2 ����ˣ���Page 54��
//	AMX0SL = 0x08;					//AMUX ѡ�� �¶ȴ�������Page 54��
	                        		
	ADC0LTH = 0x70;					//��Page 60��
	ADC0LTL = 0x00;         		
	ADC0GTH = 0x20;         		
	ADC0GTL = 0x00;					//�������� A/D ת����ʽ����A/Dֵ��һ����Χ��ʱ��ϵͳ�����ж����������ж���ĳ������
	
//��ʱ����ʼ����ϵͳʱ��Ϊ��22.1184MHz��
	CKCON = 0x00;					//��ʱ��ʹ��ϵͳʱ�ӵ� 12 ��Ƶ��Page 202��
	TMOD = 0x11;					//��ʱ�� 0��1 �����ڷ�ʽ 1 ��16 λ������/��ʱ������Page 207��
//	TMOD = 0x21;					//��ʱ�� 0 �����ڷ�ʽ 1 ��16 λ������/��ʱ��������ʱ�� 0 �����ڷ�ʽ 3 ����Ϊ UART0 �Ĳ����ʷ���������Page 207��
	                        		
	T2CON = 0x30;					//��ʱ�� 2 ��Ϊ UART0 �Ĳ����ʷ�����	230400	115200	76800	38400	19200	9600	7200	4800	2400	1200��Page 213��
	RCAP2L = 0xa0;					// ��ʱ�� 2 ��׽�Ĵ������ֽ�			0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
	RCAP2H = 0xff;					// ��ʱ�� 2 ��׽�Ĵ������ֽ�			0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xfe	0xfd
	TL2 = RCAP2L;					//��ʱ�� 2 �ĵ��ֽ�
	TH2 = RCAP2H;					//��ʱ�� 2 �ĵ��ֽڣ�Page 214��
	TR2 = 1;						//������ʱ�� 2 ���ڲ����ʷ�������ʽ����ʱ�� 2 ��ʱ���ź�Ϊ��ϵͳʱ��/2����Page 207��
	                        		
	TMR3CN = 0x00;					//��ʱ�� 3 ʹ��ϵͳʱ�ӵ� 12 ��Ƶ��ʹ�ö�ʱ�� 3 ��ʱ 0.5ms ����ADC0 ת���������� 5ms ��Ϊϵͳѭ�����ڣ���Page 216��
	TMR3RLL = 0x65;					//��ʱ�� 3 ���ؼĴ����ĵ��ֽ�
	TMR3RLH = 0xfc;					//��ʱ�� 3 ���ؼĴ����ĸ��ֽ�
	TMR3L = TMR3RLL;				//��ʱ�� 3 �ĵ��ֽ�
	TMR3H = TMR3RLH;				//��ʱ�� 3 �ĸ��ֽڣ�Page 217��
	TMR3CN = TMR3CN | 0x04;			//������ʱ�� 3 ��Page 216��
	
	T4CON = 0x30;					//��ʱ�� 4 ��Ϊ UART1 �Ĳ����ʷ�����	19200	9600	4800	2400	1200��Page 222��
	RCAP4L = 0xbf;					//��ʱ�� 4 ��׽�Ĵ������ֽ�				0xdc	0xb8	0x70	0xe0	0xc0
	RCAP4H = 0xfd;					//��ʱ�� 4 ��׽�Ĵ������ֽ�				0xff	0xff	0xff	0xfe	0xfd
	TL4 = RCAP4L;					//��ʱ�� 4 �ĵ��ֽ�
	TH4 = RCAP4H;					//��ʱ�� 4 �ĵ��ֽڣ�Page 223��
	T4CON = T4CON | 0x04;			//������ʱ�� 4 ���ڲ����ʷ�������ʽ����ʱ�� 4 ��ʱ���ź�Ϊ��ϵͳʱ��/2����Page 222��
	
	pp = ADDRESS_PC_WR2;
	*pp = 0x00;
	
	P3IF = P3IF & 0xbf;
	
//�жϳ�ʼ�� 
//W	IT0 = 1;						//INT0 Ϊ���ش���
	IT1 = 1;						//INT1 Ϊ���ش�����Page 105��
	EX0 = 1;						//����INT0 �ж�
//	EX1 = 1;						//����INT1 �ж�
	ES0 = 1;						//����UART0�ж�
//	ET0 = 1;						//������ʱ�� 0 �ж�
	PX0 = 1;						//�ⲿ�ж� 0 Ϊ�����ȼ�
//	EA = 1;							//�����жϣ�Page 123��
	EIP2 = 0x11;					// �ⲿ�ж� 6 �붨ʱ�� 3 �ж�Ϊ�����ȼ�
	
	EIE1 = 0x06;					//����ADC0 ���ڱȽϡ�SMBUS �жϣ�Page 107��
	EIE2 = 0x11;					//�����ⲿ�ж� 6 �жϣ�������ʱ�� 3 �жϣ�Page 107��
//	EIE2 = 0x43;					//����UART1 �жϣ�������ʱ�� 3 �жϣ�����ADC0 ת�������������жϣ�ѡ���¶ȴ�����ʱ��
	                        		
//���ڳ�ʼ��                		
	PCON = 0x00;					//��Դ���ƼĴ�����������/д UART ��ʽ����λSM2-SM0��ͬ UART ֡����ʹ��������λλ����ͬ����Page 112��
	SCON0 = 0x70;					//��ʽ 1 ��8 λUART �ɱ䲨���ʣ���UART0 ����������Page 190��
	SCON1 = 0x70;					//��ʽ 1 ��8 λUART �ɱ䲨���ʣ���UART1 ����������Page 199��

//IIC��ʼ�� SMB0CN �е� TOE ������Ϊ 1������ռ�ж�ʱ�� 3���Լ�� SCL �͵�ƽ��ʱ��Page 167��171��
	ENSMB = 1;						//����SMBus��IIC����Page 171��
	SMB0CR = 0x92;					//ʱ��Ƶ��Ԥ��ֵ��ϵͳʱ��Ϊ��22.1184MHzʱ��SCL ������Ϊ 100KHz ���ң���Page 172��
	SMB0ADR = 0xa0;					//��ַ�Ĵ�����λ7-1��SMBus0 �ӵ�ַ��Page 173��
//	SMB0STA = ;						//SMBus ״̬�Ĵ�����Page 173��
//	SMB0DAT = ;						//���ݼĴ���

// �ر����еĵ�
	pp = ADDRESS_74HC573A;
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/12 + 1;i++) {
		*pp=0xff;					// �͵�ƽ�Ƶ���
		pp = pp + 0x200;
		*pp=0xff;					// �͵�ƽ�Ƶ���
		pp = pp + 0x200;
		}
// �ر����еĵ�

// �ⲿ RAM ��ʼ��
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
// �ⲿ RAM ��ʼ��

	EA = 1;							//�����жϣ�Page 123��

// RTC ��ʼ��
	while (SM_BUSY);									// Wait for SMBus to be free.
	BUFF_SMB_TR[0] = 0x00;								// ��ֹͣ����λ
	SM_Send(CHIP_RTC,0x000c,0x01,0);		
	while (SM_BUSY);
	SM_Receive(CHIP_RTC,0x0001,0x01,0);
	while (SM_BUSY);									// Wait for SMBus to be free.
	BUFF_SMB_TR[0] = BUFF_SMB_RD[0] & 0x7f;				// ��ֹͣλ
	SM_Send(CHIP_RTC,0x0001,0x01,0);
// RTC ��ʼ��

// LCD ��ʼ��
//	while (SM_BUSY);									// Wait for SMBus to be free.
//	BUFF_SMB_TR[0] = 0x00;								//
//	SM_Send(SLAVE_LCD,0xc8e0,0x01,1);
//	while (SM_BUSY);									// Wait for SMBus to be free.
// LCD ��ʼ��

	load_config_data();

	wait_for_100ms();
	wait_for_100ms();									// 82C55 �� gm8125 ��ʼ��֮ǰ����ʱ

// gm8125 ��ʼ��	
	chip_uart_rst = 1;
	delay_200us(50); 
	init_gm8125();
// gm8125 ��ʼ��
	
// 82C55 ��ʼ��				
	for (i=0x00;i<4;i++) {								
		pp = ADDRESS_82C55A_D + i * 0x200;								
		*pp = 0x90;										// A ��Ϊ����ڣ�B��C ��Ϊ����ڣ��������ڷ�ʽ 0
		pp = ADDRESS_82C55A_C + i * 0x200;
		*pp = 0xff;
		pp = ADDRESS_82C55A_B + i * 0x200;
		*pp = 0xff;	
		}
// 82C55 ��ʼ��
	// ������Ĭ���Ա�����趨����
	f_run_mode = 0;										
	return 0;	
}

/*********************************************************************************************
*	ԭ�ͣ�void refrash_lamp(void)
*	����: ��ɫ������ƣ�ˢ�£��ӳ���
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
void refrash_lamp(void)
{
unsigned char i,j;
unsigned char xdata *pp;
	flick_count++;
//061013	if (flick_count >= 0x06) {						// ������ʱ�������ñ���ֵ�ɸı����Ƶ��
	if (flick_count >= 0x04) {								// ������ʱ�������ñ���ֵ�ɸı����Ƶ��
		flick_count = 0x00;
		flick_mark = !flick_mark;	
		}
	for(i=0;i<2*((MAX_LAMP_NUM - 1)/12 + 1);i++){
		if(flick_mark) lamp_output_buffer1[i]=(lamp_flick_buffer[i]) | lamp_output_buffer[i]; //��
		else lamp_output_buffer1[i] = lamp_output_buffer[i]; //��
		}
	
	j = 0x00;
//0914	pp = ADDRESS_74HC573A;
	pp = ADDRESS_74HC573K;
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/12 + 1;i++) {
		*pp=~lamp_output_buffer1[j];					// �͵�ƽ�Ƶ���
		j++;
		pp = pp + 0x200;
		*pp=~lamp_output_buffer1[j];					// �͵�ƽ�Ƶ���
		j++;
//0914		pp = pp + 0x200;
		pp = pp - 0x600;
	}
}
/*********************************************************************************************
*	ԭ�ͣ�int get_run_mode()
*	����: ��ȡ��ǰ������ģʽ
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
int get_run_mode()
{
	return f_run_mode;
}
/*********************************************************************************************
*	ԭ�ͣ�int get_run_mode()
*	����: ��ȡ��ǰ������ģʽ
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
int clear_run_mode()
{
	 f_run_mode=0;
}
/*********************************************************************************************
*	ԭ�ͣ�int is_detect_health_flag()
*	����: �Ƿ�����ϵı�־λ
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
int is_detect_health_flag()
{
	return f_key_val;
}
/*********************************************************************************************
*	ԭ�ͣ�int get_temperature()
*	����: ��ȡ�¶ȣ���λΪ0.1���϶�
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
int get_temperature()
{
	return tempture;
	}

/*********************************************************************************************
*	ԭ�ͣ�int is_door_illegal_open()
*	����: ���Ƿ�Ƿ���
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
int is_door_illegal_open()
{
	if (f_out0_int) {
		f_out0_int = 0;
		f_buzz = 1;
		led_bg = 1;					// ���� LCD ����
		led_bg_count = 0;
		return 1;
		}
	return 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�void power_check(void)
*	����: ����Դ״̬����������û�磬���������ϱ��������ɺ��жϵ�ع����·
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
void power_check(void)
{
	if (pw_ac) {
		power = 1;						// 
		pw_count = 0x00;
		
		}
	else pw_count++;
	if (pw_count > 0x30) {				// (100 - 120ms) ��ⲻ��������Դ���������ݳ���
		pw_count = 0x40;
		}
	}

/*********************************************************************************************
*	ԭ�ͣ�void wait_for_100ms(void)
*	����: ��ʱ 100ms ѭ��
*	˵����
*	��������
*	���أ���
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
			ctemp0 = ~(*pp);								// ������ǰ�������״̬
			ctemp1 = ctemp0 ^ detector_status_bak[i];
			if (ctemp1 != 0x00) 							// ����һ�ε�״̬��һ��
			{
				detector_status_bak[i] = ctemp0;			
				ctemp1 = ctemp1 & ctemp0;					// ֻ�������޳����г��ı仯					
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
	
	if (f_temperature) {							// �¶�
		f_temperature = 0;
		result = ((result - 41857) * 100L) / 1540;
		tempture = (int)(result - 15);					// ��ȥ 1.5 ���϶�
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

	front_panel_dis(g_controller_status.m_actual_control_mode);		// ǰ������ģʽ��ʾ��led ָʾ�ƣ�

	while(!SYS100ms);
	SYS100ms = 0;	
//	if (f_boot) {
		get_lamp_health_status();
		conflict_detect();
		
		cpld_count++;
		if (cpld_count > 0x0f) {						// cpld ���˿ڳ��� 1500ms û�����ݴ��䣬����Ϊ pc104 
			cpld_count = 0x00;							// ��ʧ�жϣ��� mcu ����������жϣ���������ʧ�жϵ�����
			pp = ADDRESS_PC_WR2;
			*pp = 0x00;									// ���ж�
			if (f_cpld_trans_num) pp = ADDRESS_PC_WR0;	// �������ݵ�ͨ����ַ
			else pp = ADDRESS_PC_WR1;			
			*pp = cpld_trans_bak;						// ��������
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
*	ԭ�ͣ�void judge_detector(void)
*	����: ����������û��жϣ�����ֻ�ܸ�����������жϣ���˲�̫׼ȷ��ֻ����Ϊ�ο���
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
void judge_detector(void)
{	
unsigned char i,chartemp,j,chanl_start,detector_count_buff[MAX_DETECTOR_COUNT];
	read_detector_time++;
	if (read_detector_time >= 40) {			// 4 �����ж�һ��
		get_detector_counter(&detector_count_buff);
		read_detector_time = 0x00;
		detector_time++;
		for (i=0x00;i<MAX_DETECTOR_COUNT;i++) {
			if (detector_count_buff[i] < detector_count_bak0[i]) chartemp = (64 + detector_count_buff[i]) -  detector_count_bak0[i];
			else chartemp = detector_count_buff[i] -  detector_count_bak0[i];
		
			chanl_start = check_chanl_7;
			for (j=0x00;j<i%8;j++) chanl_start = chanl_start >> 1;
		
			if (chartemp > 8) {				// 4 ���ڼ����������仯������ 8������Ϊ��·�����������
//060211				chanl_start = check_chanl_7;
//060211				for (j=0x00;j<i%8;j++) chanl_start = chanl_start >> 1;
				detector_err[i / 8] = detector_err[i / 8] | chanl_start;
				
				set_error(DETECTOR_ERROR,&g_controller_status.m_error.m_hard_error);			// ����ù���
				}
			else {							// ������Ϊ��·�����û������
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
//		if (detector_time >= 248) {		// ���õļ��������� 248 * 4 ���ڣ�û�м�⵽������Ϊ��·�����������
//			detector_time = 0x00;
//			detector_err[0] = ~detector_flag[0] | detector_err[0];
//			detector_err[1] = ~detector_flag[1] | detector_err[1];
//			detector_err[2] = ~detector_flag[2] | detector_err[2];
//			detector_err[3] = ~detector_flag[3] | detector_err[3];
//			}
		}
	}

/*********************************************************************************************
*	ԭ�ͣ�void init_gm8125(void)
*	����: ������չоƬ GM8125 ��ʼ��
*	˵����
*	��������
*	���أ���
*********************************************************************************************/
void init_gm8125(void)
{
#ifdef single_mode
	chip_uart_ms = 1;				// ������չоƬ�ڵ�ͨ��ģʽ�¹���
	UART_CHANL = 0x09;				// ���� 0 �ڵ�ͨ��ģʽ�£����ͺͽ��ܶ�ͨ���Ӵ��� 1 ���
	
	TR2 = 0;						//							230400	115200	76800	38400	19200	9600	7200	4800	2400	1200��Page 213��
	RCAP2L = 0xa0;					// ��ʱ�� 2 ��׽�Ĵ������ֽ�	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
	RCAP2H = 0xff;					// ��ʱ�� 2 ��׽�Ĵ������ֽ�	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xfe	0xfd
	TR2 = 1;
#else 
	UART_CHANL = UART_CHANL & 0xc7;
	P4 = 0xc0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	chip_uart_ms = 0;				// ������չоƬ�ڶ�ͨ��ģʽ�¹���	
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	SBUF0 = 0xf3;					// ����GM8125�Ӵ��ڲ�����Ϊ19200bps��ĸ���ڲ�����Ϊ115200bps 10Bit
	ES0 = 0;
	while(TI0 == 0); 
	TI0 = 0;
	delay_200us(50);
	chip_uart_ms = 1; 				// �������� 
	while(RI0 == 0); 
	RI0 = 0;
	ReceiveBuff0 = SBUF0; 	
	while (ReceiveBuff0!=0xf3) { 	// ��֤д����������Ƿ���ȷ������ȷ������д 
		ES0 = 0;
		chip_uart_ms = 0;			// GM8125������д�����ֹ���ģʽ��
		SBUF0 = 0xf3;
		while(TI0 == 0); 
		TI0=0; 
		delay_200us(50); 
		chip_uart_ms = 1; 			// �������� 
		while(RI0 == 0); 
		RI0 = 0;
		ReceiveBuff0 = SBUF0;
		} 
	chip_uart_ms = 0; 				// ����GM8125�����ڶ�ͨ������ģʽ��
	
	comm_baud = B_9600;
	
	P2MDOUT = 0xff;					// �� P2.7��P2.6��P2.5��P2.4��P2.3 P2.2 P2.1 P2.0 ����Ϊ���췽ʽ
	P2 = P2 | 0x08;					//
	P4 = 0xff;
	TR2 = 0;						//								230400	115200	76800	38400	19200	9600	7200	4800	2400	1200��Page 213��
	RCAP2L = 0xf4;			//f4		// ��ʱ�� 2 ��׽�Ĵ������ֽ�	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
	RCAP2H = 0xff;						// ��ʱ�� 2 ��׽�Ĵ������ֽ�	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xff	0xfe	0xfd
	TR2 = 1;
	RI0 = 0;
	ES0 = 1;
#endif	
	}

/*********************************************************************************************
*	ԭ�ͣ�void delay_200us(unsigned char m)
*	����: ������ʱ�ӳ���
*	˵����
*	��������
*	���أ���
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
*	ԭ�ͣ�void decode_key(void)
*	���ܣ���������
*	˵�������� AD ֵ�����ְ�����������ң��������
*	��������
*	���أ���
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
		switch (TELECONTROL_BAK) {					// ң�ذ�������
			case 0x01:
				{
					f_run_mode = 1;						// ��ǰ����ϵ��趨����
					ctemp0 = 0x01;
					break;
				}
			case 0x02: 
				{
					f_run_mode = 1;						// ��ǰ����ϵ��趨����
					ctemp0 = 0x02;
					break;
				}
			case 0x03:
				{
					f_run_mode = 1;						// ��ǰ����ϵ��趨����
					ctemp0 = 0x03;
					break;
				}
			case 0x04:
				{
					f_run_mode = 1;						// ��ǰ����ϵ��趨����
					ctemp0 = 0x04;
					break;
				}
			case 0x05:
				{
					f_run_mode = 1;						// ��ǰ����ϵ��趨����
					ctemp0 = 0x05;
					if (!f_manual)
					{
						f_manual = 1;
						f_manual_auto = 1;				// �ֶ�/�Զ�ת��һ����Ч
					}
					manual_set_stage = 1;
				 	f_manual_step= 1;
					break;
				}
			case 0x06:
				{
					f_run_mode = 1;						// ��ǰ����ϵ��趨����
					ctemp0 = 0x06;
					if (!f_manual) 
					{
						f_manual = 1;
						f_manual_auto = 1;				// �ֶ�/�Զ�ת��һ����Ч
					}
					manual_set_stage = 2;
					f_manual_step = 1;
					break;
				}
			case 0x07: 
				{
					f_run_mode = 1;						// ��ǰ����ϵ��趨����
					ctemp0 = 0x07;
					if (!f_manual)
					{
						f_manual = 1;
						f_manual_auto = 1;				// �ֶ�/�Զ�ת��һ����Ч
					}
					manual_set_stage = 3;
					f_manual_step = 1;
					break;
				}
			case 0x08:
				{
					f_run_mode = 1;						// ��ǰ����ϵ��趨����
					ctemp0 = 0x08;
					if (!f_manual)
				  {
						f_manual = 1;
						f_manual_auto = 1;				// �ֶ�/�Զ�ת��һ����Ч
					}
					manual_set_stage = 4;
					f_manual_step = 1;
					break;
				}
			case 0x09:	{
				f_manual = 1;						// �ֶ�/�Զ�ѡ�񿪹��Ѵ򿪴����ֶ�״̬
				f_manual_auto = 1;					// �ֶ�/�Զ�ת��һ����Ч
				break;
				}
			case 0x0a:	{
				f_manual = 0;						// �ֶ�/�Զ�ѡ�񿪹��Ѵ򿪴����Զ�״̬
				f_manual_auto = 1;					// �ֶ�/�Զ�ת��һ����Ч
				break;
				}
			case 0x0b:	f_step = 1;	break;			// һ����Ч��"��������"
			case 0x0c:	{
				f_manual = 0;						// �ֶ�/�Զ�ѡ�񿪹��Ѵ򿪴����Զ�״̬
				f_manual_auto = 1;					// �ֶ�/�Զ�ת��һ����Ч
				break;
				}
			default:break;
			}
		if (ctemp0 != 0x00) {
			key0_val = ctemp0;
			f_mode_switch = 1;
			}
		}
	if (f_key0_int) {			 					// ���� 0 ����
		f_key0_int = 0;
		if (!f_key0_int_bak) {						// �ð������º�ĵ�һ�β���ֵ
			f_key0_int_bak = 1;
			key_val0 = key0_adc0;
			}
		else {										// �ð������º�ĵڶ��β���ֵ
			if (key_val0 > key0_adc0) ctemp0 = key_val0 - key0_adc0;
			else ctemp0 = key0_adc0 - key_val0;
			if (ctemp0 < 8) {					// �������µĶ��β���ֵ��ֵ�ں�����Χ����ΪΪ���ΰ�����Ч
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
					
					if (ctemp0 != 0xff) {		// һ����Ч��"���� 0 ����"
						key0_val = ctemp0;
						f_mode_switch = 1;
						f_buzz = 1;
						led_bg = 1;				// ���� LCD ����
						f_key0_ed = 1;			// ģ������ 0 һ����Ч����
						f_key1_ed = 0;
						led_bg_count = 0;
						f_run_mode = 1;			// ��ǰ����ϵ��趨����
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

	if (f_key1_int) {			 				// ���� 0 ����
		f_key1_int = 0;
		if (!f_key1_int_bak) {					// �ð������º�ĵ�һ�β���ֵ
			f_key1_int_bak = 1;
			key_val1 = key1_adc0;
			}
		else {									// �ð������º�ĵڶ��β���ֵ
			if (key_val1 > key1_adc0) ctemp0 = key_val1 - key1_adc0;
			else ctemp0 = key1_adc0 - key_val1;
			if (ctemp0 < 8) {					// �������µĶ��β���ֵ��ֵ�ں�����Χ����ΪΪ���ΰ�����Ч
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
					if (ctemp0 != 0xff) {					// һ����Ч��"���� 0 ����"
						f_buzz = 1;
						led_bg = 1;							// ���� LCD ����
						f_key0_ed = 0;
						f_key1_ed = 1;						// ģ������ 1 һ����Ч����
						led_bg_count = 0;
						key0_val = ctemp0 + 4;
						f_mode_switch = 1;
						f_run_mode = 1;						// ��ǰ����ϵ��趨����
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

//1219	if (f_manual_use) {			 						// �ֶ�/�Զ�ѡ�񿪹�
//1219		if (!f_key2_int_bak) {							// �ð������º�ĵ�һ�β���ֵ
//1219			f_key2_int_bak = 1;
//1219			key_val2 = key2_adc0;
//1219			}
//1219		else {											// �ð������º�ĵڶ��β���ֵ
//1219			if (key_val2 > key2_adc0) ctemp0 = key_val2 - key2_adc0;
//1219			else ctemp0 = key2_adc0 - key_val2;
//1219			if (ctemp0 < 8) {						// �������µĶ��β���ֵ��ֵ�ں�����Χ����ΪΪ���ΰ�����Ч		
//1219				if (!f_key2_ed) {
//1219					f_buzz = 1;
//1219					led_bg = 1;							// ���� LCD ����
//1219					f_key2_ed = 1;						// ģ������ 2 һ����Ч����
//1219					f_manual = 1;						// �ֶ�/�Զ�ѡ�񿪹��Ѵ򿪴����ֶ�״̬
//1219					led_bg_count = 0;
//1219					f_auto_buzz = 1;
//1219					f_manual_bak = 1;
//1219					
//1219					manual_m_control_mode_bak = g_controller_status.m_actual_control_mode;		// �����ֶ�ǰ��ʵ�ʿ���ģʽ
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
//1219			led_bg = 1;									// ���� LCD ����
//1219			f_auto_buzz = 0;
//1219			led_bg_count = 0;
//1219			}
//1219		}

	if (f_key2_int) {			 							// �ֶ�/�Զ�ѡ�񰴼�
		f_key2_int = 0;
		if (!f_key2_ed) {									// ��һ����Ч��"�ֶ�/�Զ�ѡ�񰴼�"�����ڸð����ͷź�
			if (!f_key2_int_bak) {							// �ð������º�ĵ�һ�β���ֵ
				f_key2_int_bak = 1;
				key_val2 = key2_adc0;
				}
			else {											// �ð������º�ĵڶ��β���ֵ
				if (key_val2 > key2_adc0) ctemp0 = key_val2 - key2_adc0;
				else ctemp0 = key2_adc0 - key_val2;
				if (ctemp0 < 8) {							// �������µĶ��β���ֵ��ֵ�ں�����Χ����ΪΪ���ΰ�����Ч
					f_buzz = 1;
					led_bg = 1;								// ���� LCD ����
					f_key2_ed = 1;							// ģ������ 2 һ����Ч����
					led_bg_count = 0;
					f_manual = !f_manual;					// �ֶ�/�Զ�ת��
					f_manual_auto = 1;						// �ֶ�/�Զ�ת��һ����Ч
					}
				else key_val2 = key2_adc0;
				}
			}
		}
	else {													// �ֶ�/�Զ�ѡ�񰴼����ͷ�
		f_key2_ed = 0;
		f_key2_int_bak = 0;
		}

	if (f_key3_int) {			 							// ��������
		f_key3_int = 0;
		if (!f_key3_ed) {									// ��һ����Ч��"��������"�����ڸð����ͷź�
			if (!f_key3_int_bak) {							// �ð������º�ĵ�һ�β���ֵ
				f_key3_int_bak = 1;
				key_val3 = key3_adc0;
				}
			else {											// �ð������º�ĵڶ��β���ֵ
				if (key_val3 > key3_adc0) ctemp0 = key_val3 - key3_adc0;
				else ctemp0 = key3_adc0 - key_val3;
				if (ctemp0 < 8) {							// �������µĶ��β���ֵ��ֵ�ں�����Χ����ΪΪ���ΰ�����Ч
					f_buzz = 1;
					led_bg = 1;								// ���� LCD ����
					f_key3_ed = 1;							// ģ������ 7 һ����Ч����
					f_step = 1;								// һ����Ч��"��������"
					led_bg_count = 0;
					}
				else key_val3 = key3_adc0;
				}
			}
		}
	else {													// �����������ͷ�
		f_key3_ed = 0;
		f_key3_int_bak = 0;
		}

	if (f_manual_auto) {									// �ֶ�/�Զ�ת���账��������
		f_manual_auto = 0;
		if (f_manual) {										// �����ֶ�ǰ��ʵ�ʿ���ģʽ
			manual_m_control_mode_bak = g_controller_status.m_actual_control_mode;		
			}
		else {												// �ָ��ֶ�ǰ��ʵ�ʿ���ģʽ
// 090610			if ((manual_m_control_mode_bak == 4) || (manual_m_control_mode_bak == 5)) g_config.m_control_mode_selection.m_control_mode = manual_m_control_mode_bak;
// 090610			else g_config.m_control_mode_selection.m_control_mode = MULTIPLE_SCHEDULE_CONTROL;
// 090625		  g_config.m_control_mode_selection.m_control_mode = manual_m_control_mode_bak;
			g_controller_status.m_designate_control_mode=manual_m_control_mode_bak;
//			save_control_mode_selection();					// ���浱ǰ�Ŀ���ģʽ
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
*	ԭ�ͣ�void scankey(void)
*	���ܣ�����ɨ�衣ֻ����������������������ϰ�����ÿ�ΰ�����Ч����ʱ������ʱ�䳤����ȷ��
*		  ���ֵ������ʱ��С�� 1.2s ʱ��Ϊ������ֵ������̧����ִ�иü��������� 1.2s ʱ��
*		  Ϊ���Ӽ�ֵ�����õȵ��ð����ͷŲ�ִ�У���������ִ�У�����һ��������������һ������
*         �ͷź��£����ܱ�ɨ�赽����ִ�С��缸��ͬʱ����ʱ��ֻ������µ��Ǹ�������Ч��
*	˵����ɨ����ͨ���޸� scan_table[i] ����ɡ�
*	��������
*	���أ���
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
			key_port = key_port & scan_table[i];		// i+1 ��ʾɨ���ߵĶ���
			ctemp = *pchar;
			j = (ctemp & 0x01) ^ 0x01;
			if (j!=0) {
				k = 0;									// k+1 ��ʾ�����ߵĶ���
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
			if (key_count>=0x04) f_buzz = 1;	// ���̻���������ݴ� 4 ���������糬�� 4 ���򲻽����µİ�����������
			if (!f_key_now) {
//1024				f_buzz = 1;
				led_bg = 1;			// ���� LCD ����
				led_bg_count = 0;
				f_key_now = 1;			// ���һ������ֵ��ֱ���ð������ͷ�
				key_val = key_table[j]; 
				}
			else {
				if (key_time >= 0x88) {		// ��ʱ�䣨���� 0.68s ʱ����ĳһ�����͸���һ��ֵ
					key_time = 0xf4;
					if (!f_key && (key_count<0x04)) {	// ���̻���������ݴ� 4 ���������糬�� 4 ���򲻽����µİ�����������
						f_key = 1;
						f_buzz = 1;
						key_count++;
						key_buf[0] = key_val + 0x10;
//						if (key_buf[0] == 0x12) f_key_val = !f_key_val;
//						if (f_key_val) {
//							f_lamp_err = 0;				// ��λ�ƹ��ϱ�־λ
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
			f_key_now = 0;				// �������ͷ�
			if (!f_key && (key_count<0x04)) {	// ���̻���������ݴ� 4 ���������糬�� 4 ���򲻽����µİ�����������
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
*	ԭ�ͣ�int set_term_status(phase_t phase)
*	����: ����������ӵ�״̬
*	˵����
*	��������                  
*	���أ���                  
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

// ����ӳ��	
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
// ����ӳ��			

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
*	ԭ�ͣ�void get_lamp_health_status(void)
*	����: ��ȡ�ƵĹ�����Ϣ
*	˵����0 û���ϣ�1��ʾ�й���
*	��������                  
*	���أ���                  
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
	for (i=0x00;i<(MAX_LAMP_NUM - 1)/8;i++) {			// һ��ƿذ��� 12 ���ƣ����ص�״̬ռ�� 12 λ
// ���ӷ�ӳ��			
		ctemp1 = (lamp_input_buffer[10][k+1] << 2) & 0x08; 						// �� 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k] << 2) & 0x04);              // �� 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+1] << 1) & 0x02);            // �� 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+1] >> 3) & 0x01);			// �� 2
		
		ctemp2 = (lamp_input_buffer[10][k] << 6) & 0x80;                         // �� 2
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+1] << 4) & 0x40);            // �� 2
		ctemp2 = ctemp2 | (lamp_input_buffer[10][k+1] & 0x20);					// �� 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k] << 2) & 0x10);				// �� 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+1] >> 1) & 0x08);			// �� 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+1] >> 5) & 0x04);			// �� 4
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k] >> 2) & 0x02);				// �� 4
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+1] >> 6) & 0x01);			// �� 4
		
		lamp_input_buffer[10][k] = ctemp1;
		lamp_input_buffer[10][k+1] = ctemp2;
		
		ctemp1 = (lamp_input_buffer[10][k+3] << 2) & 0x08; 						// �� 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+2] << 2) & 0x04);            // �� 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+3] << 1) & 0x02);            // �� 1
		ctemp1 = ctemp1 | ((lamp_input_buffer[10][k+3] >> 3) & 0x01);			// �� 2
		
		ctemp2 = (lamp_input_buffer[10][k+2] << 6) & 0x80;                       // �� 2
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+3] << 4) & 0x40);            // �� 2
		ctemp2 = ctemp2 | (lamp_input_buffer[10][k+3] & 0x20);					// �� 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+2] << 2) & 0x10);			// �� 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+3] >> 1) & 0x08);			// �� 3
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+3] >> 5) & 0x04);			// �� 4
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+2] >> 2) & 0x02);			// �� 4
		ctemp2 = ctemp2 | ((lamp_input_buffer[10][k+3] >> 6) & 0x01);			// �� 4
		
		lamp_input_buffer[10][k+2] = ctemp1;
		lamp_input_buffer[10][k+3] = ctemp2;
// ���ӷ�ӳ��

		j = lamp_input_buffer[10][k+1] >> 4;				// �� 2 ��ƿذ�ϲ�Ϊ 3 ���ֽ�
		ctemp[i] = (lamp_input_buffer[10][k] << 4) | j;
		ctemp[i + 1] = lamp_input_buffer[10][k+1] << 4 | (lamp_input_buffer[10][k+2] & 0x0f);
		ctemp[i + 2] = lamp_input_buffer[10][k+3];

//w		j = lamp_input_buffer[10][k+1] >> 4;				// �� 2 ��ƿذ�ϲ�Ϊ 3 ���ֽ�
//w		*(pp + i) = (lamp_input_buffer[10][k] << 4) | j;
//w		*(pp + i + 1) = lamp_input_buffer[10][k+1] << 4 | (lamp_input_buffer[10][k+2] & 0x0f);
//w		*(pp + i + 2) = lamp_input_buffer[10][k+3];
		i = i + 0x02;
		k = k + 0x04;
		}
//	xxxx 1111  1111 1111


//w1031				ÿ�����ӵ����״̬���� 1400ms ����������������Ϊ���й��� 
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
				if (lamp_health_count[i][j] >= 0x0d) {		// ĳ�������� 1400ms ��������⣬����Ϊ�õ��й���
					lamp_health_count1[i][j] = 200;			// ĳ����һ����⵽�й��ϣ��ͱ������� 200s ��������⣬����ȡ���ù���
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
*	ԭ�ͣ�void conflict_detect(void)
*	����: ��ͻ���
*	˵����
*	��������                  
*	���أ���                  
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
				if ((lamp_health_status[i] & ctemp2) != 0x00) {								// ��ǰ�����й���
					f_lamp_err = 1;															// ��ǰ�����й��ϱ�־λ
// 090224					f_lamp_err = 1;															// ��ǰ�����й��ϱ�־λ
					ctemp1 = 8 * i + j;
					if (is_term_connect_to_yellow(i+1) == 0x00) f_lamp_green_or_red_err = 1;// ��ƻ��̵��й���
					else f_lamp_yellow_err = 1;												// �Ƶ��й���

//					if ((lamp_output_buffer1[i] & ctemp2) != 0x00) {						// ��ǰ�ö����趨Ϊ��
//						
//						}
//					else {																	// ��ǰ�ö����趨Ϊ��
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
//		if (lamp_err_delay >= 200) {														// ����״̬���� 20s
//			f_lamp_err = 0;
//			lamp_err_delay = 0x00;
//			}
//		}
//	else lamp_err_delay = 0x00;
// 090611
	}




/*********************************************************************************************
*	ԭ�ͣ�int get_lamp_health(lamp_health_t* health)
*	����: ��ȡ�ƵĹ�����Ϣ
*	˵����0 û���ϣ�1��ʾ�й���
*	��������                  
*	���أ���                  
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
*	ԭ�ͣ�int get_detector_status(detector_status_t* status)
*	����: ��ȡ������ļ����Ϣ
*	˵����0��ʾû����1��ʾ�г�
*	��������                  
*	���أ���                  
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
*	ԭ�ͣ�int get_detector_health(detector_health_t* health)
*	����: ��ȡ������Ĺ�����Ϣ
*	˵����
*	��������                  
*	���أ���                  
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
*	ԭ�ͣ�int get_detector_counter(detector_counter_t* counter)
*	����: ��ȡ���������ֵ
*	˵����������ΧΪ 0 -- 63
*	��������                  
*	���أ���                  
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
//061016*	ԭ�ͣ�clear_detector_counter(int detector)
//061016*	����: ���ָ��������ļ���ֵ
//061016*	˵����
//061016*	��������                  
//061016*	���أ���                  
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
//061016	*pp = 0xff;								// ���������λ������
//061016	
//061016	get_detector_counter(&detector_count_bak0);
//061016	read_detector_time = 0x00;
//061016	}

/*********************************************************************************************
*	ԭ�ͣ�void in_init(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
void in_init(int port)
{
	g_in_read_p[port-1] = 0;
	g_in_write_p[port-1] = 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�int open_comm(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
void in_putchar(int port,unsigned char ch)
{
	uart0_receive_buffer[port-1][g_in_write_p[port-1]++] = ch;
	if(g_in_write_p[port-1] == uart0_trans_max)
		g_in_write_p[port-1] = 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�int open_comm(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
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
//061016*	ԭ�ͣ�int open_comm(int port)
//061016*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
//061016*	˵����
//061016*	��������                  
//061016*	���أ���                  
//061016*********************************************************************************************/
//061016int is_in_full(int port)
//061016{
//061016	if((g_in_write_p[port-1] + 1)%uart0_trans_max  == g_in_read_p[port-1])
//061016		return 1;
//061016	else
//061016		return 0;
//061016	}

/*********************************************************************************************
*	ԭ�ͣ�int open_comm(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
int is_in_empty(int port)
{
	if(g_in_read_p[port-1] == g_in_write_p[port-1])
		return 1;
	else
		return 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�int open_comm(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
void out_init(int port)
{
	g_out_read_p[port-1] = 0;
	g_out_write_p[port-1] = 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�int open_comm(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
void out_putchar(int port,unsigned char ch)
{
	uart0_trans_buffer[port-1][g_out_write_p[port-1]++] = ch;
//	uart0_trans_buffer[port-1][g_out_write_p[port-1]++] = 0xaa;
	if(g_out_write_p[port-1] == uart0_trans_max)
		g_out_write_p[port-1] = 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�int open_comm(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
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
*	ԭ�ͣ�int open_comm(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
int is_out_full(int port)
{
	if((g_out_write_p[port-1] + 1)%uart0_trans_max  == g_out_read_p[port-1])
		return 1;
	else
		return 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�int open_comm(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
int is_out_empty(int port)
{
	if(g_out_read_p[port-1] == g_out_write_p[port-1])
		return 1;
	else
		return 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�int open_comm(int port)
*	����: �� fd ��Ӧ��ͨ�Ŷ˿ڣ��ɹ����ظ� fd��ʧ�ܷ��� -1
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
int open_comm(int port)
{
unsigned char i,ctemp1,j;	
	i = (char)port;
	if (i<=6) {
		j = g_config.m_comm_param[port-1].m_baud;
		if (j == comm_baud) {				// �͵�ǰ������һ��
			in_init(port);
			out_init(port);
			ctemp1 = 0x01;
			for (i=0x01;i<port;i++) ctemp1 = ctemp1 << 1;
			flag1_uart0 = flag1_uart0 | ctemp1;							// ����Ӧ�˿����ڽ������ݱ�־��λ
			return port;
			} 
		else {															// �͵�ǰ�����ʲ�һ���������
			if (((flag0_uart0 & 0x1f) | (flag1_uart0 & 0x1f)) == 0x00) {// ��ǰ���ڶ�ûʹ�ã����Ը��Ĳ�����
				
				UART_CHANL = UART_CHANL & 0xc7;							// �� STADD0 - STADD2 ����
				P4 = 0xc0;												// �� SRADD0 - SRADD2 ����
				_nop_();
				_nop_();
				_nop_();
				_nop_();
				chip_uart_ms = 0;										// ������չоƬ�ڶ�ͨ��ģʽ�¹���
				_nop_();
				_nop_();
				_nop_();
				_nop_();
				SBUF0 = 0xf0 | (g_config.m_comm_param[port-1].m_baud - 2);	// ����GM8125�Ӵ��ڲ�����Ϊ1200 - 19200bps��ĸ���ڲ�����Ϊ7200 - 115200bps 10Bit
				ES0 = 0;
				while(TI0 == 0); 
				TI0 = 0;
				delay_200us(50);
				chip_uart_ms = 1; 							// �������� 
				while(RI0 == 0); 
				RI0 = 0;
				ReceiveBuff0 = SBUF0;
				ctemp1 = 0x00;
				while (ReceiveBuff0 != (0xf0 | (g_config.m_comm_param[port-1].m_baud - 2))) { 	// ��֤д����������Ƿ���ȷ������ȷ������д 
					ES0 = 0;
					chip_uart_ms = 0;					// GM8125������д�����ֹ���ģʽ��
					SBUF0 = 0xf0 | (g_config.m_comm_param[port-1].m_baud - 2);
					while(TI0 == 0); 
					TI0=0; 
					delay_200us(50); 
					chip_uart_ms = 1; 					// �������� 
					while(RI0 == 0); 
					RI0 = 0;
					ReceiveBuff0 = SBUF0;
					ctemp1++;
					if (ctemp1 > 0x0a) return -1;		// �޸Ĳ�����ʧ��
					} 
				chip_uart_ms = 0; 				// ����GM8125�����ڶ�ͨ������ģʽ��
    	
				P2MDOUT = 0xff;					// �� P2.7��P2.6��P2.5��P2.4��P2.3 P2.2 P2.1 P2.0 ����Ϊ���췽ʽ
//				P2 = 0x08;						// 
				P2 = P2 | 0x08;
				P4 = 0xff;
		
				ES0 = 0;		
				TR2 = 0;
				RCAP2H = 0xff;
				switch (g_config.m_comm_param[port-1].m_baud) {
					case B_1200:						//								230400	115200	76800	57600	19200	9600	7200	4800	2400	1200��Page 213��
						RCAP2L = 0xa0;					// ��ʱ�� 2 ��׽�Ĵ������ֽ�	0xfd	0xfa	0xf7	0xf4	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;
					case B_2400:
						RCAP2L = 0xd0;					// ��ʱ�� 2 ��׽�Ĵ������ֽ�	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;
					case B_4800:
						RCAP2L = 0xe8;					// ��ʱ�� 2 ��׽�Ĵ������ֽ�	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;
					case B_9600:
						RCAP2L = 0xf4;					// ��ʱ�� 2 ��׽�Ĵ������ֽ�	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;		
					case B_19200:
						RCAP2L = 0xfa;					// ��ʱ�� 2 ��׽�Ĵ������ֽ�	0xfd	0xfa	0xf7	0xee	0xdc	0xb8	0xa0	0x70	0xe0	0xc0
						break;
					default: RCAP2L = 0xf4;
					
					}								
				TR2 = 1;
				ES0 = 1;
				in_init(port);
				out_init(port);
				ctemp1 = 0x01;
				for (i=0x01;i<port;i++) ctemp1 = ctemp1 << 1;
				flag1_uart0 = flag1_uart0 | ctemp1;				// ����Ӧ�˿����ڽ������ݱ�־��λ
				comm_baud = g_config.m_comm_param[port-1].m_baud;
				return port;
				}
			else return -1;										// ��ǰ����������ʹ�õģ������Ը��Ĳ�����
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
//061016*	ԭ�ͣ�int close_comm(int fd)
//061016*	����: �ر� fd ��Ӧ��ͨ�Ŷ˿�
//061016*	˵����
//061016*	��������                  
//061016*	���أ���                  
//061016*********************************************************************************************/
//061016int close_comm(int fd)
//061016{
//061016unsigned char i,ctemp1;
//061016	i = (char)fd;
//061016	if (i<=5) {
//061016		ctemp1 = 0x01;
//061016		for (i=0x01;i<fd;i++) ctemp1 = ctemp1 << 1;
//061016		flag0_uart0 = ~ctemp1 & flag0_uart0;				// ����Ӧ�˿����ڷ������ݱ�־����
//061016		flag1_uart0 = ~ctemp1 & flag1_uart0;				// ����Ӧ�˿����ڽ������ݱ�־����		
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
*	ԭ�ͣ�int read_comm(int fd,byte*  in_buffer, int size)
*	����: �Ӵ��� x ��ȡ����
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
int read_comm(int fd,byte*  in_buffer, int size)
{
unsigned char j;
unsigned int i;
	if(size <1)	return size;
	j = (char)fd;	
	if (j <= 8) {									// 
		if (j <= 5) ES0 = 0;						// �ش��� 0 �ж�
		else {
			if (j==6) EIE2 = EIE2 & 0xbf;			// ES1 = 0  �ش��� 1 �ж�
			else EX0 = 0;
			}
		
		i=0;
		while(!is_in_empty(fd)){
			in_buffer[i++] = in_getchar(fd);
			if(i==size)
				break;
			}
		
		if (j <= 5) ES0 = 1;						// ������ 0 �ж�
		else {
			if (j==6) EIE2 = EIE2 | 0x40;			// ES1 = 1 ������ 1 �ж�
			else EX0 = 1;
			}
//		ES0 = 1;									// 
		} 
	else return -1;									// ���ն˿ڳ��� 5 ����
	return i;										// ���ض��ص��ֽ���
	}

/*********************************************************************************************
*	ԭ�ͣ�int write_comm(int fd,byte* out_buffer, int size)
*	����: �򴮿� x д����
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
int write_comm(int fd,byte* out_buffer, int size)
{
unsigned char j,ctemp1,k;
unsigned char xdata * data pchar;
unsigned int i;
	if(size < 1) return size;
	k = (char)fd;
	if (k>8) return -1;											// �˿ڳ��� 8 ����
	
	i=0;	

	EIE2 = EIE2 & 0xef;											// ���ⲿ�ж� 6
	while(!is_out_full(fd)){
		out_putchar(fd,out_buffer[i++]);
		if(i==size)	break;
		}
	EIE2 = EIE2 | 0x10;											// ���ⲿ�ж� 6


	if (k <= 5) {												// ���� 0 ��չ�� 5 ���Ӵ���
		ctemp1 = 0x01;
		for (j=0x01;j<fd;j++) ctemp1 = ctemp1 << 1;
		if ((flag0_uart0 & 0x1f) == 0x00) {						// ���Ӵ��ڶ�û���ݷ��ͣ�Ҫ��������
			uart0_chanl_sel = fd << 3;
			UART_CHANL = (UART_CHANL & 0xc7) | uart0_chanl_sel;	// ѡ��Ҫ���͵Ķ˿�
			SBUF0 = uart0_trans_buffer[fd - 1][g_out_read_p[fd-1]++];
			if(g_out_read_p[fd-1] == uart0_trans_max) g_out_read_p[fd-1] = 0;

			}
		flag0_uart0 = flag0_uart0 | ctemp1;						// ����Ӧ�˿������ݷ��ͱ�־��λ
		}
	else {
		switch (k) {
			case 6:{											// ���� 1 
				if (!f_uart1_using) {
					SBUF1 = uart0_trans_buffer[5][g_out_read_p[5]++];
					if(g_out_read_p[5] == uart0_trans_max) g_out_read_p[5] = 0;		
					f_uart1_using = 1;
					}
				break;
				}
			case 7:{											// CPLD �˿� 0
				if (!f_cpld_chanl0_using) {
					pchar = ADDRESS_PC_WR0;
					EIE2 = EIE2 & 0xef;							// ���ⲿ�ж� 6		
					f_cpld_trans_num = 1;
					cpld_trans_bak = uart0_trans_buffer[6][g_out_read_p[6]++];
					*pchar = cpld_trans_bak;
					if(g_out_read_p[6] == uart0_trans_max) g_out_read_p[6] = 0;		
					EIE2 = EIE2 | 0x10;							// ���ⲿ�ж� 6
					f_cpld_chanl0_using = 1;
					cpld_count = 0x00;
					}
				break;
				}
			default:{											// CPLD �˿� 1
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
*	ԭ�ͣ�int is_manual_control_state()
*	����: �Ƿ����ֶ�����״̬
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
int is_manual_control_state()
{
	if (f_manual) return 1;
	else return 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�unsigned char get_front_panel_control_mode()
*	����: ǰ����趨�Ŀ���ģʽ
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
unsigned char get_front_panel_control_mode()
{
unsigned char ctemp0;
//	if (!f_manual) {
		switch(key0_val) {
			case 0: ctemp0 = ONLINE_HARMONY_CONTROL;break;				// ����Э������
			case 1: ctemp0 = ALL_RED_CONTROL;break;						// ȫ�����
			case 2: ctemp0 = YELLOW_FLASH_CONTROL;break;				// ��������
			case 3: ctemp0 = MULTIPLE_SCHEDULE_CONTROL;break;			// ��ο���
			case 4: ctemp0 = ACTUATE_CONTROL;break;						// ��Ӧ����			
			case 5: ctemp0 = 0;break;									// ����
			default : ctemp0 = 0;break;
			}
// 090803 zjl			
//		if (ctemp0 != 0) g_config.m_control_mode_selection.m_control_mode = ctemp0;
		
		return ctemp0;
//		}
//	else return 0;
	}

/*********************************************************************************************
*	ԭ�ͣ�void front_panel_dis(unsigned char ch)
*	����: ǰ������ģʽ��ʾ��led ָʾ�ƣ�
*	˵����
*	��������                  
*	���أ���                  
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
			case 1: ctemp0 = 0x40;break;								// ��ƿ���
			case 2: ctemp0 = 0x04;break;								// ȫ�����
			case 3: ctemp0 = 0x08;break;								// ��������
			case 4: ctemp0 = 0x10;break;								// ��ο���			
			case 5: ctemp0 = 0x20;break;								// ��Ӧ����
//			case 6: ctemp0 = ;break;									// 
			case 7: ctemp0 = 0x02;break;								// ����Э������
			case 8: ctemp0 = 0x01;break;								// �ֶ�����
			default : ctemp0 = 0;break;
		}
	pchar = ADDRESS_FRONT_PANEL;
	*pchar = ctemp0;
	}
/*********************************************************************************************
*	ԭ�ͣ�int is_step_forward_button_pressed()
*	����: �Ƿ��˲�����ť
*	˵����
*	��������                  
*	���أ���                  
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
*	ԭ�ͣ�int clear_step_forward_button_state()
*	����: ���������ť״̬
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
int clear_step_forward_button_state()
{
	f_step = 0;
	return 0;
}
/*********************************************************************************************
*	ԭ�ͣ�int is_need_change_stage()
*	����: �Ƿ���Ҫ�ı䵽�¸�״̬
*	˵����
*	��������                  
*	���أ���                  
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
*	ԭ�ͣ�int is_need_change_stage_pressed()
*	����: 
*	˵����
*	��������                  
*	���أ���Ҫ�ı䵽����λ��                  
*********************************************************************************************/
int get_manual_next_stage_count()
{
	return manual_set_stage;
}
/*********************************************************************************************
*	ԭ�ͣ�int clear_need_change_stage_flag()
*	����: �����Ҫ�ı䵽�¸���λ�ı�־
*	˵����
*	��������                  
*	���أ���                  
*********************************************************************************************/
int clear_need_change_stage_flag()
{
	f_manual_step = 0;
	return 0;
}
///*********************************************************************************************
//*	ԭ�ͣ�unsigned int cal_crc(unsigned char *ptr, unsigned char len)
//*	����: ��� CRC У��
//*	˵����
//*	��������                  
//*	���أ���                  
//*********************************************************************************************/
//unsigned int cal_crc(unsigned char *ptr, unsigned char len) 
//{
//unsigned int crc;
//unsigned char da;
//	crc=0;
//	while(len--!=0) {
//		da = ((char)(crc/256))/16; 							// �ݴ�CRC �ĸ���λ 
//		crc <<= 4; 											// CRC ����4 λ���൱��ȡCRC �ĵ�12 λ��
//		crc ^= crc_ta[da^(*ptr/16)]; 						// CRC �ĸ�4 λ�ͱ��ֽڵ�ǰ���ֽ���Ӻ��
//															// ������CRC��Ȼ�������һ��CRC ������
//		da = ((char)(crc/256))/16; 							// �ݴ�CRC �ĸ�4 λ 
//		crc <<= 4; 											// CRC ����4 λ�� �൱��CRC �ĵ�12 λ��
//		crc ^= crc_ta[da^(*ptr&0x0f)]; 						// CRC �ĸ�4 λ�ͱ��ֽڵĺ���ֽ���Ӻ��
//															// ������CRC��Ȼ���ټ�����һ��CRC ������
//		Ptr++;
//		}
//	return(crc);
//	}

// �޸ļ�¼
// 090611 �޸ı����ƹ����ֳ���ֻ����20�룬20s�����¼��
// 090407 �޸ļ����������ʽ��ԭ��������ļ������м�������ϵ�Ӳ������������ɣ��޸�Ϊ�ɵ�Ƭ����������������������8255����
//        ������λʱ����Ӧ���Ʋ�����������
// 090224 �޸ı����ƹ����ֳ���ԭ��ֻҪ��⵽�еƹ��Ͼ�һֱͣ�������״̬���޸�Ϊ�������
// 071213 �޸��ɸ�Ӧ���Ƶ��ֶ��������ֶ�����ʱ��������������ȫ��׶Σ�����˳��ԭ����˳��
// 071210 ����ԭ����ң��������ֱ������ĳ���̵Ʒ��в�ʱ��ǰ����ϻ����������Ƶơ�ȫ��׶�
// 070301 �����źŻ����塣ÿ���ƿذ塢���������е� 74HC244 �����������е� 82C55Ƭѡ��Ҫ��һ���������裬
//        ����ϵͳ���ư�û����߱�����ϵͳ��û����ʱ����ЩоƬ��Ƭѡ���ճ��ֲ���״̬��������߳�ͻ��ʹ����
//        �Ӵ󣬳��ڻ�ʹ����оƬ�𻵡�Ŀǰ���������������ϵͳ���ϣ���ֵΪ 2K���ƿذ塢�����������������
//        ʱ��Ӧ�ý�ϵͳ���ϵ���������ȡ��������ϵͳ���ϵ���������ȡ�ϴ��ֵ��Ϊ��Ӧ�ƿذ塢���������ϵ���
//        ������ʮ�����ϣ���ƿذ塢���������ϵ���������ȡ2K��ϵͳ���ϵ���������ȡ20K ���ϣ�����ϵͳ��ͣ��
//        ʱ����������������ķ�ѹ��ϵ����ʹƬѡ�˵ĵ�ѹ���ɵ͵�ƽ
// 070203 ǰ��� a /d �����ѹ�����иı䣬R28��R7 �� 1K ��Ϊ 2.7K��R29��R8��1K ��Ϊ 4.3K
// 070202 ң����������� 244 Ƭѡ©���������裬ǰ���״̬��ʾ���� 373Ƭѡ©����������
// 061013 ����������Ƶ�ʣ�mcu.c �иģ�
// 061011 ����ң������ֱ�ӿ���ĳһ�����̵ƣ����κ�����£���һ��ң������Ӧ�İ��������Է��ж�Ӧ����ĳ���
// 061011 �˳�ʱ�谴����ֶ�������תΪ�Զ����Ƽ��ɡ�ֻ����������λ������λ������λ������������ȫ�졢�̵ơ�
// 061011 �������Ƶ�ʱ�ſ����ã�mcu.c �иģ�
// 061010 ��Ӧ����ʱ�����ж϶��Ӻ�Ϊ 9 18 27 36 ���Ƿ���̵ƣ������ɽ���Щ������ɳ��̡������Ӧ����ʱ��
// 061010 �κζ��Ӷ�������Ϊ���̣�actuate_control.c �иģ�
// 060615 �޸�ԭ����ǰ������Ϊ"�ֶ�����"ʱ�����ľ��޷��ٸı����ģʽ��smbus.c �иģ� 
// 060613 ����ÿ��ֻҪǰ�˿���ģʽ������Чʱ�������µ��øÿ���ģʽ���㷨��Ԥ����ʵ�����ж�Ϊ��Ӧ����ʱ���������е��ã�
// 060613 ��Ϊ���е��û���ȫ����֣���ԭ��ֻ���ڿ���ģʽ�ı�ʱ���ŵ��ã�mcu.c��kernel.c �иģ�
// 060612 ��ԭ�������루AD�� 0��1 ���棬��Ϊ�����棬Ӳ���� 74HC573(U4) �� LE �˹̶��Ӹߵ�ƽ������������ÿ��ģʽ�ı䣬
// 060612 ����ϴ��趨�ı�־λ���Ҵ��ڽ�������ʱ��Ҳ�����־λ��Ϊ�����������ϴ�Ԥ��ģʽ��ͬ�Ŀ���ģʽ��

// 060612 kernel /fb-linux.img ro root=/dev/hdb2 single vga=785 video=vesa:ypan init=/bin/sh
// ���� DOC �̷���
// cd /util
// /util/bin/insmod doc.o
// cp fla.img /dev/msys/fla 
// sync


// 071212
//	int fun(int someVal);
//	int main(){
//	��fun(10);
//	}
//	Ȼ���������Ҫ��ĳ��ʱ�̽�����ָ�뱣�������������Ժ����һ��ʱ�̵���������������
//	int fun(int);
//	typedef int (*func_handle)(int);
//	int main(){
//	��func_handle fh=fun;
//	��... //do something
//	��fh(10);
//	}
//	���fun��ʽΪvoid fun(int)�أ�����������fun��������������ʽ����fun��ÿһ����ʽ��typedefһ����Ӧ��func_handle
// 071212

//
//	�������뺯��ָ��                                                                                                                                                                                                            
//	                                                                                                                                                                                                                            
//	һ ͨ���ĺ�������                                                                                                                                                                                                           
//	    һ��ͨ���ĺ������õ����ӣ�                                                                                                                                                                                              
//	//���а���ͷ�ļ�                                                                                                                                                                                                            
//	void MyFun(int x);    //�˴�������Ҳ��д�ɣ�void MyFun( int );                                                                                                                                                              
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //�����ǵ���MyFun(10);����                                                                                                                                                                                
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	void MyFun(int x)  //���ﶨ��һ��MyFun����                                                                                                                                                                                  
//	{                                                                                                                                                                                                                           
//	   printf(��%d\n��,x);                                                                                                                                                                                                      
//	}                                                                                                                                                                                                                           
//	    ���MyFun������һ���޷���ֵ�ĺ��������������ʲô���顣���ֵ��ú����ĸ�ʽ��Ӧ���Ǻ���Ϥ�İɣ����������е���MyFun��������д��ʽ��                                                                                        
//	MyFun(10);                                                                                                                                                                                                                  
//	    ����һ��ʼֻ�Ǵӹ����ϻ���˵����ѧ����������MyFun���������֪��MyFun��������������һ�����ܣ�����˵һ�δ��룩��                                                                                                          
//	    ֱ������                                                                                                                                                                                                                
//	    ѧϰ������ָ�����ʱ���ҲŲ��ò���˼������������������ʲô�����أ�                                                                                                                                                      
//	    ����Ҫ��Ϊ����û��ʲô��������ޣ��Ǻǣ��������¿����֪���ˡ���                                                                                                                                                        
//	                                                                                                                                                                                                                            
//	�� ����ָ�����������                                                                                                                                                                                                       
//	    ����ĳһ���ݱ������ڴ��ַ���Դ洢����Ӧ��ָ�������һ�����������׵�ַҲ�Դ洢��ĳ������ָ�������ġ��������ҾͿ���ͨ���������ָ�������������ָ��ĺ����ˡ�                                                          
//	    ��Cϵ�������У��κ�һ������������Ҫ��������֮�����ʹ�õġ���ô������ָ�����ҲӦ��Ҫ�������ɣ�����������������أ������������Ϊ������������һ������ָ��MyFun�����ĺ���ָ�����FunP�������������FunP�����ķ�����      
//	void (*FunP)(int) ;   //Ҳ��д��void (*FunP)(int x);                                                                                                                                                                        
//	    �㿴����������ָ�������������ʽ��ͬ����MyFun��������һ����ֻ�����������ǰ�MyFun�ĳɣ�*FunP�����ѣ�����������һ����ָ��MyFun������ָ��FunP�ˡ�����Ȼ�����FunPָ�����Ҳ����ָ����������������ͬ����������ֵ�ĺ����ˡ���
//	                                                                                                                                                                                                                            
//	�� ͨ������ָ��������ú���                                                                                                                                                                                                 
//	    ����FunPָ����������ǾͿ��Զ�����ֵָ��MyFun��Ȼ��ͨ��FunP������MyFun�����ˡ��������ͨ��FunPָ�����������MyFun�����ģ�                                                                                             
//	//���а���ͷ�ļ�                                                                                                                                                                                                            
//	void MyFun(int x);    //�������Ҳ��д�ɣ�void MyFun( int );                                                                                                                                                                
//	void (*FunP)(int );   //Ҳ��������void(*FunP)(int x)����ϰ����һ�㲻������                                                                                                                                                  
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //����ֱ�ӵ���MyFun����                                                                                                                                                                                   
//	   FunP=&MyFun;  //��MyFun�����ĵ�ַ����FunP����                                                                                                                                                                            
//	   (*FunP)(20);    //����ͨ������ָ�����FunP������MyFun�����ġ�                                                                                                                                                            
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	void MyFun(int x)  //���ﶨ��һ��MyFun����                                                                                                                                                                                  
//	{                                                                                                                                                                                                                           
//	   printf(��%d\n��,x);                                                                                                                                                                                                      
//	}                                                                                                                                                                                                                           
//	    �뿴�����ֲ��ֵĴ��뼰ע�͡�                                                                                                                                                                                            
//	    ���п������ţ��������������еúܺá�                                                                                                                                                                                    
//	    Ŷ���ҵĸо��ǣ�MyFun��FunP�����͹�ϵ������int ��int *�Ĺ�ϵ������MyFun������һ����int�ı���������������FunP����һ����int *һ����ָ�������                                                                           
//	int i,*pi;                                                                                                                                                                                                                  
//	pi=&i;    //��FunP=&MyFun�Ƚϡ�                                                                                                                                                                                             
//	    ����ĸо��أ���                                                                                                                                                                                                        
//	    �Ǻǣ���ʵ��Ȼ����                                                                                                                                                                                                      
//	                                                                                                                                                                                                                            
//	�� ���ú�����������д��ʽ                                                                                                                                                                                                   
//	����ָ��Ҳ������ʹ�ã������ͬ�������飺                                                                                                                                                                                    
//	//���а���ͷ�ļ�                                                                                                                                                                                                            
//	void MyFun(int x);                                                                                                                                                                                                          
//	void (*FunP)(int );    //����һ������ָ��ͬ������������ֵ������ָ�������                                                                                                                                                   
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //�����ǵ���MyFun(10);����                                                                                                                                                                                
//	   FunP=MyFun;  //��MyFun�����ĵ�ַ����FunP����                                                                                                                                                                             
//	   FunP(20);    //����ͨ������ָ�����������MyFun�����ġ�                                                                                                                                                                   
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	void MyFun(int x)  //���ﶨ��һ��MyFun����                                                                                                                                                                                  
//	{                                                                                                                                                                                                                           
//	   printf(��%d\n��,x);                                                                                                                                                                                                      
//	}                                                                                                                                                                                                                           
//	�Ҹ��˺����ֲ��֣���������֮ǰ�Ĵ���Ƚ�һ�£���                                                                                                                                                                            
//	�������ԣ�����һ���سɹ���                                                                                                                                                                                                  
//	�ף�                                                                                                                                                                                                                        
//	FunP=MyFun;                                                                                                                                                                                                                 
//	����������MyFunֵͬ��ֵ��FunP���ѵ�MyFun��FunP��ͬһ�������ͣ�����ͬ��int ��int�Ĺ�ϵ������������ͬint ��int*�Ĺ�ϵ�ˣ�����û��һ���ĺ�Ϳ�ˣ���                                                                           
//	������֮ǰ�Ĵ����е�ì���ˣ��ǰɣ�������˵�                                                                                                                                                                              
//	���������ݲ�������ͣ����������¼����������Щ�ɶ��ǿ�����ȷ���еĴ���Ӵ������                                                                                                                                              
//	����֮����                                                                                                                                                                                                                  
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //�����ǵ���MyFun(10);����                                                                                                                                                                                
//	   FunP=&MyFun;  //��MyFun�����ĵ�ַ����FunP����                                                                                                                                                                            
//	   FunP(20);    //����ͨ������ָ�����������MyFun�����ġ�                                                                                                                                                                   
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	����֮�ģ�                                                                                                                                                                                                                  
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   MyFun(10);     //�����ǵ���MyFun(10);����                                                                                                                                                                                
//	   FunP=MyFun;  //��MyFun�����ĵ�ַ����FunP����                                                                                                                                                                             
//	   (*FunP)(20);    //����ͨ������ָ�����������MyFun�����ġ�                                                                                                                                                                
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	����ǿ����������ޣ�                                                                                                                                                                                                        
//	���ۣ�����Ҫ�ε��ˣ���                                                                                                                                                                                                      
//	�����ţ�������                                                                                                                                                                                                              
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   ��*MyFun��(10);     //����������MyFunҲ�����������ĵ��ø�ʽ                                                                                                                                                              
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	��Ҳ����һ�μ����ɣ�����������Ҳ����������д�İ�����ֻ��������ƽ��û��������д���ˡ���                                                                                                                                      
//	��ô����Щ��˵����ʲô�أ�                                                                                                                                                                                                  
//	�Ǻǣ���ʹ���ǡ�����Ħ˹��������������֪ʶ�;�����������ƪ�ġ��·��֡����ض����ɴ˷������ƶϳ����µĽ��ۣ�                                                                                                                  
//	1. ��ʵ��MyFun�ĺ�������FunP����ָ�붼��һ���ģ������Ǻ���ָ�롣MyFun��������һ������ָ�볣������FunP��һ��������ָ��������������ǵĹ�ϵ��                                                                                 
//	2. ���������������������(*MyFun)(10)������������д����������ǲ�����Ͳ�ϰ�ߵġ�����C���Ե�������ǲŻ���Ƴ��ֿ�����MyFun(10);������ʽ�ص��ã�����������˲�����ѧ�еĺ�����ʽһ���������𣿣���                          
//	3. Ϊͳһ�����FunP����ָ�����Ҳ����FunP(10)����ʽ�����á�                                                                                                                                                                 
//	4. ��ֵʱ������FunP=&MyFun��ʽ��Ҳ��FunP=MyFun��                                                                                                                                                                            
//	���������д��������㰮��ô�ţ�                                                                                                                                                                                            
//	����������ɣ��������������Ժ���ָ���Ӧ��ඣ�                                                                                                                                                                            
//	��󡪡�                                                                                                                                                                                                                    
//	����˵��һ�㣺�ں�������������                                                                                                                                                                                              
//	void MyFun(int );    //����д��void (*MyFun)(int )��                                                                                                                                                                        
//	void (*FunP)(int );   //����д��void FunP(int )��                                                                                                                                                                           
//	���뿴ע�ͣ���һ����Ҫע��ġ�                                                                                                                                                                                              
//	                                                                                                                                                                                                                            
//	�� ����ĳһ������ָ�����ͣ�                                                                                                                                                                                                 
//	�����Զ�����������һ��������Ҳ�����ȶ���һ������ָ�����ͣ�Ȼ�����������������������ָ�������                                                                                                                              
//	���ȸ���һ���Զ����������͵����ӡ�                                                                                                                                                                                          
//	typedef int* PINT;    //Ϊint* ���Ͷ�����һ��PINT�ı���                                                                                                                                                                     
//	int main()                                                                                                                                                                                                                  
//	{                                                                                                                                                                                                                           
//	  int x;                                                                                                                                                                                                                    
//	  PINT px=&x;   //��int * px=&x;�ǵȼ۵ġ�PINT������ʵ����int * ����                                                                                                                                                        
//	  *px=10;       //px����int*���͵ı���                                                                                                                                                                                      
//	  return 0;                                                                                                                                                                                                                 
//	}                                                                                                                                                                                                                           
//	����ע�ͣ�Ӧ�ò��ѿ����ɣ�����Ȼ����ܺ�����������ʹ�ã����Ժ�ѧϰWin32���ʱ�ᾭ�������ġ���                                                                                                                               
//	������������һ�º���ָ�����͵Ķ��弰ʹ�ã��������϶��գ���                                                                                                                                                                  
//	//���а���ͷ�ļ�                                                                                                                                                                                                            
//	void MyFun(int x);    //�˴�������Ҳ��д�ɣ�void MyFun( int );                                                                                                                                                              
//	typedef void (*FunType)(int );   //����ֻ�Ƕ���һ������ָ������                                                                                                                                                             
//	FunType FunP;              //Ȼ����FunType����������ȫ��FunP����                                                                                                                                                            
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	//FunType FunP;    //����ָ�������ȻҲ�ǿ����Ǿֲ��� ���Ǿ��������������ˡ�                                                                                                                                                
//	   MyFun(10);                                                                                                                                                                                                               
//	   FunP=&MyFun;                                                                                                                                                                                                             
//	   (*FunP)(20);                                                                                                                                                                                                             
//	                                                                                                                                                                                                                            
//	      return 0;                                                                                                                                                                                                             
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	void MyFun(int x)                                                                                                                                                                                                           
//	{                                                                                                                                                                                                                           
//	   printf(��%d\n��,x);                                                                                                                                                                                                      
//	}                                                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	�����岿�֣�                                                                                                                                                                                                                
//	���ȣ���void (*FunType)(int ); ǰ����һ��typedef ������ֻ�Ƕ���һ����ΪFunType����ָ�����ͣ�������һ��FunType������                                                                                                         
//	Ȼ��FunType FunP;  ������PINT px;һ��������һ��FunP������                                                                                                                                                               
//	������ͬ�����������������ͬ���¡�                                                                                                                                                                                          
//	���������ĺô��ǣ�                                                                                                                                                                                                          
//	����FunType���ͺ����ǾͿ���ͬ���ء��ܷ������FunType�������������ͬ���͵ĺ���ָ������ˡ����£�                                                                                                                          
//	FunType FunP2;                                                                                                                                                                                                              
//	FunType FunP3;                                                                                                                                                                                                              
//	//����                                                                                                                                                                                                                      
//	                                                                                                                                                                                                                            
//	�� ����ָ����Ϊĳ�������Ĳ���                                                                                                                                                                                               
//	��Ȼ����ָ�������һ����������ȻҲ������Ϊĳ�������Ĳ�����ʹ�õġ����ԣ��㻹Ӧ֪������ָ���������Ϊĳ�������Ĳ���������ʹ�õġ�                                                                                            
//	����һ��ʵ����                                                                                                                                                                                                              
//	Ҫ����Ҫ���һ��CallMyFun�����������������ͨ�������еĺ���ָ��ֵ��ͬ���ֱ����MyFun1��MyFun2��MyFun3������������ע�������������Ķ����ʽӦ��ͬ����                                                                       
//	ʵ�֣��������£�                                                                                                                                                                                                            
//	//���а���ͷ�ļ�                                                                                                                                                                                                            
//	void MyFun1(int x);                                                                                                                                                                                                         
//	void MyFun2(int x);                                                                                                                                                                                                         
//	void MyFun3(int x);                                                                                                                                                                                                         
//	typedef void (*FunType)(int ); //��. ����һ������ָ������FunType,��ٺ�������һ��                                                                                                                                           
//	void CallMyFun(FunType fp,int x);                                                                                                                                                                                           
//	                                                                                                                                                                                                                            
//	int main(int argc, char* argv[])                                                                                                                                                                                            
//	{                                                                                                                                                                                                                           
//	   CallMyFun(MyFun1,10);   //��. ͨ��CallMyFun�����ֱ����������ͬ�ĺ���                                                                                                                                                    
//	   CallMyFun(MyFun2,20);                                                                                                                                                                                                    
//	   CallMyFun(MyFun3,30);                                                                                                                                                                                                    
//	}                                                                                                                                                                                                                           
//	void CallMyFun(FunType fp,int x) //��. ����fp��������FunType��                                                                                                                                                              
//	{                                                                                                                                                                                                                           
//	  fp(x);//��. ͨ��fp��ָ��ִ�д��ݽ����ĺ�����ע��fp��ָ�ĺ�������һ��������                                                                                                                                                
//	}                                                                                                                                                                                                                           
//	void MyFun1(int x) // ��. ���Ǹ���һ�������ĺ�����������������Ҳ��ͬ                                                                                                                                                        
//	{                                                                                                                                                                                                                           
//	   printf(������MyFun1�������%d\n��,x);                                                                                                                                                                                    
//	}                                                                                                                                                                                                                           
//	void MyFun2(int x)                                                                                                                                                                                                          
//	{                                                                                                                                                                                                                           
//	   printf(������MyFun2�������%d\n��,x);                                                                                                                                                                                    
//	}                                                                                                                                                                                                                           
//	void MyFun3(int x)                                                                                                                                                                                                          
//	{                                                                                                                                                                                                                           
//	   printf(������MyFun3�������%d\n��,x);                                                                                                                                                                                    
//	}

//                                                                                                                                                                                                                           