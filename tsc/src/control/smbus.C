#include "mcu.h"
#include "datastruct.h"
#include "kernel.h"
#include "smbus.h"

char bdata flag_eeprom0,flag_eeprom1;
sbit f_save_system_param						= flag_eeprom0^0;		// 
sbit f_save_lamp_group_setting 					= flag_eeprom0^1;		// 
sbit f_save_detector_setting 					= flag_eeprom0^2;		// 
sbit f_save_phase_plan 							= flag_eeprom0^3;		// 
sbit f_save_timming_plan 						= flag_eeprom0^4;		// 
sbit f_save_schedule_plan 						= flag_eeprom0^5;		// 
sbit f_save_special_day 						= flag_eeprom0^6;		// 
sbit f_save_multiple_schedule_control_param 	= flag_eeprom0^7;		// 

sbit f_save_actuate_control_param				= flag_eeprom1^0;		// 
sbit f_save_cordless_coordinate_control_param 	= flag_eeprom1^1;		// 
sbit f_save_comm_param 							= flag_eeprom1^2;		// 
sbit f_save_control_mode_selection 				= flag_eeprom1^3;		// 
sbit f_save_config_data							= flag_eeprom1^4;		//
sbit f_save_green_conflict						= flag_eeprom1^5;
sbit f_save_lamp_setting              = flag_eeprom1^6;

bit f_load_config_data;
bit f_set_time;									// 有数据要写入 RTC 标志
bit f_rd_rtc_now;								// 正在读系统时间标志
bit f_page_wr;									// 页写标志
bit f_eeprom_rd;								// 有数据要从 EEPROM 读标志
bit f_rd_eeprom_now;							// 正在读 EEPROM 标志 
bit f_step_all;									// 准备读 EEPROM 中的某相位表总步数标志位

bit f_save_err;
extern bit f_manual;

unsigned char write_rtc_buff[7];				// 修改系统时间数据缓冲

unsigned int wrt_num;
unsigned int rdt_num,rdt_num_bak;
unsigned char xdata *start_addr_ram;
unsigned int start_addr_eeprom,start_addr_phase_plan_no;
//unsigned char save_phase_plane_table[6][12];
unsigned char phase_plan_no,phase_plan_no_step,timming_plan_no,schedule_plan_no,comm_param_no,step_all_ram;

unsigned int err_count;
unsigned char err_buf[6],m_soft_error_bak,m_hard_error_bak;

void save_err(void);
void err_data(unsigned char ctemp);

void fun_smbus(void);
void fun_write_rtc(void);
void get_system_time(void);
void fun_write_EEPROM(void);
int set_system_time(date_time_t *dt);
void display_status(unsigned char ctemp1,unsigned char ctemp2); 
void SM_Receive(unsigned char chip_select, unsigned int byte_address,unsigned char rd_count,bit f_addr);
void SM_Send(unsigned char chip_select, unsigned int byte_address, unsigned char wr_count,bit f_addr);

/*********************************************************************************************
*	原型：void SM_Send(unsigned char chip_select, unsigned int byte_address, unsigned char wr_count,bit f_addr)
*	功能: SMBUS(IIC) 发送子程序
*	说明：发送 wr_count 个字节，f_addr 为 0 表示字节地址为 8 位，为 1 则为 16 位地址
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void SM_Send(unsigned char chip_select, unsigned int byte_address, unsigned char wr_count,bit f_addr)
{                             
	SMB0CN = 0x44;									// SMBus enabled,ACK on acknowledge cycle
	SMB_WR_NUM_C = 0x00;         
	SMB_WR_NUM_BAK = wr_count;   
	COMMAND = (chip_select | WRITE);				// Chip select + WRITE
	
	if (f_addr) {
		ADDR_NUMBER = 2;							// 2 address bytes
		HIGH_ADD = ((byte_address >> 8) & 0x00FF);	// Upper 8 address bits
		LOW_ADD = (byte_address & 0x00FF);			// Lower 8 address bits
		}
	else {
		ADDR_NUMBER = 1;							// 1 address bytes
		HIGH_ADD = (byte_address & 0x00FF);			// 8 address bits
		}
	STO = 0;
	STA = 1;										// Start transfer
	if (COMMAND == SLAVE_LCD) f_lcd = 1;
//0705（送到 LCD 显示）			if (COMMAND == SLAVE_LCD) f_lcd = 1;								
	SM_BUSY = 1;									// Occupy SMBus (set to busy)
	}

/*********************************************************************************************
*	原型：void SM_Receive (unsigned char chip_select, unsigned int byte_address,unsigned char rd_count,bit f_addr)
*	功能: SMBUS(IIC) 接收子程序
*	说明：接收 rd_count 个字节，f_addr 为 0 表示字节地址为 8 位，为 1 则为 16 位地址
*	参数：无
*	返回：无
*********************************************************************************************/
void SM_Receive(unsigned char chip_select, unsigned int byte_address,unsigned char rd_count,bit f_addr)
{
	SMB0CN = 0x44;									// SMBus enabled, ACK on acknowledge cycle
	SMB_RD_NUM_C = 0x00;
	SMB_RD_NUM_BAK = rd_count;                                			
	COMMAND = (chip_select | READ);					// Chip select + READ
	
	if (f_addr) {
		ADDR_NUMBER = 2;							// 2 address bytes
		HIGH_ADD = ((byte_address >> 8) & 0x00FF);	// Upper 8 address bits
		LOW_ADD = (byte_address & 0x00FF);			// Lower 8 address bits
		}
	else {
		ADDR_NUMBER = 1;							// 1 address bytes
		HIGH_ADD = (byte_address & 0x00FF);			// 8 address bits
		}
	STO = 0;
	STA = 1;										// Start transfer
	SM_BUSY = 1;									// Occupy SMBus (set to busy)
	}
	
//061016/*********************************************************************************************
//061016*	原型：void display_status(unsigned char ctemp)
//061016*	功能：
//061016*	说明：
//061016*	参数：无
//061016*	返回：无
//061016*********************************************************************************************/
//061016void display_status(unsigned char ctemp1,unsigned char ctemp2) 
//061016{
//061016	if (!SM_BUSY) {
//061016		BUFF_SMB_TR[0] = (ctemp1 / 100) % 10;
//061016		BUFF_SMB_TR[1] = (ctemp1 / 10) % 10; 
//061016		BUFF_SMB_TR[2] = ctemp1 % 10; 
//061016		BUFF_SMB_TR[3] = (ctemp2 / 100) % 10;
//061016		BUFF_SMB_TR[4] = (ctemp2 / 10) % 10; 
//061016		BUFF_SMB_TR[5] = ctemp2 % 10;  
//061016		SM_Send(SLAVE_LCD,0xe01a,0x06,1);
//061016		}
//061016	}

/*********************************************************************************************
*	原型：int set_system_time(date_time_t *dt)
*	功能: 将设定的系统时间存入缓冲区
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int set_system_time(date_time_t *dt)
{
	f_set_time = 1;
	write_rtc_buff[0] = (((*dt).m_second / 10) << 4) | ((*dt).m_second % 10);
	write_rtc_buff[1] = (((*dt).m_minute / 10) << 4) | ((*dt).m_minute % 10);
	write_rtc_buff[2] = (((*dt).m_hour / 10) << 4) | ((*dt).m_hour % 10);
	if ((*dt).m_weekday == 0x00) write_rtc_buff[3] = 0x07;
	write_rtc_buff[3] = (*dt).m_weekday;
	write_rtc_buff[4] = (((*dt).m_day / 10) << 4) | ((*dt).m_day % 10);
	write_rtc_buff[5] = (((*dt).m_month / 10) << 4) | ((*dt).m_month % 10);
	write_rtc_buff[6] = (((*dt).m_year[1] / 10) << 4) | ((*dt).m_year[1] % 10);
	return 0;
	}

/*********************************************************************************************
*	原型：void fun_write_rtc(void)
*	功能: 将缓冲区的系统时间写入 RTC
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void fun_write_rtc(void) 
{
	f_set_time = 0;
	BUFF_SMB_TR[0] = write_rtc_buff[0];
	BUFF_SMB_TR[1] = write_rtc_buff[1];
	BUFF_SMB_TR[2] = write_rtc_buff[2];
	BUFF_SMB_TR[3] = write_rtc_buff[3];
	BUFF_SMB_TR[4] = write_rtc_buff[4];
	BUFF_SMB_TR[5] = write_rtc_buff[5];
	BUFF_SMB_TR[6] = write_rtc_buff[6];
	SM_Send(CHIP_RTC,0x0001,0x07,0);
	}

/*********************************************************************************************
*	原型：int save_config_data()
*	功能: 保存系统配置参数
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_config_data()
{
	f_save_config_data = 1;
	return 0;
	}

/*********************************************************************************************
*	原型：int save_system_param()
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_system_param()
{
	f_save_system_param = 1;
	}

/*********************************************************************************************
*	原型：int save_lamp_group_setting()
*	功能: 保存灯组设置信息
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_lamp_group_setting()
{
	f_save_lamp_group_setting = 1;
}
/*********************************************************************************************
*	原型：int save_lamp_group_setting()
*	功能: 保存信号灯设置信息
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_lamp_setting()
{
	f_save_lamp_setting=1;
}
/*********************************************************************************************
*	原型：int save_detector_setting()
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_detector_setting()
{
	f_save_detector_setting = 1;
	}

/*********************************************************************************************
*	原型：int save_phase_plan(int plan_no,int step)
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_phase_plan(int plan_no,int step)
{
/*unsigned char ctemp1,ctemp2,i,j;
	ctemp2 = (char)step;
	j = (ctemp2-1) % 8;
	ctemp1 = 0x01;
	for (i=0x00;i<j;i++) {
		ctemp1 = ctemp1 << 1;
		}
	i = (ctemp2 - 1) / 8;
	ctemp2 = (char)plan_no;	
	ctemp2--;
	save_phase_plane_table[ctemp2][i] = save_phase_plane_table[ctemp2][i] | ctemp1;
	
//	f_save_phase_plan = 1;
*/
	phase_plan_no = (char)plan_no;
	f_save_phase_plan = 1;
}

/*********************************************************************************************
*	原型：void get_save_phase_plan(void)
*	功能: 
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
/*void get_save_phase_plan(void)
{
unsigned char i,j,k,ctemp;
	if (!(f_save_phase_plan | f_step_all | f_step_all_ok)) {
		for (i=0x00;i<6;i++) {
			for (j=0x00;j<12;j++) {
				if (save_phase_plane_table[i][j] != 0x00) {
					ctemp = 0x01;
					for (k=0x00;k<8;k++) {
						if ((save_phase_plane_table[i][j] & ctemp) != 0x00) {
							save_phase_plane_table[i][j] = save_phase_plane_table[i][j] & (~ctemp);
							phase_plan_no = i + 1;
							phase_plan_no_step = 1 + j * 8 + k;
							f_save_phase_plan = 1;							
							return;
							}
						ctemp = ctemp << 1;
						}
					} 
				}
			}			
		}
	}*/


/*********************************************************************************************
*	原型：int save_timming_plan(int plan_no)
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_timming_plan(int plan_no)
{
	timming_plan_no = (char)plan_no;
	f_save_timming_plan = 1;
}

/*********************************************************************************************
*	原型：int save_schedule_plan(int plan_no)
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_schedule_plan(int plan_no)
{
	schedule_plan_no = (char)plan_no;
	f_save_schedule_plan = 1;
}

/*********************************************************************************************
*	原型：int save_special_day()
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_special_day()
{
	f_save_special_day = 1;
	}

/*********************************************************************************************
*	原型：int save_multiple_schedule_control_param()
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_multiple_schedule_control_param()
{
	f_save_multiple_schedule_control_param = 1;
	}

/*********************************************************************************************
*	原型：int save_actuate_control_param()
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_actuate_control_param()
{
	f_save_actuate_control_param = 1;
	}

/*********************************************************************************************
*	原型：int save_cordless_coordinate_control_param()
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_cordless_coordinate_control_param()
{
	f_save_cordless_coordinate_control_param = 1;
	}

/*********************************************************************************************
*	原型：int save_comm_param(int param_no)
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_comm_param(int param_no)
{
	comm_param_no = (char)param_no;
	f_save_comm_param = 1;
	}

/*********************************************************************************************
*	原型：int save_control_mode_selection()
*	功能: 
*	说明：占用 1 + 3 * 32  共 97 个字节
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_control_mode_selection()
{
	f_run_mode = 0;										// 以中心设定模式运行
	f_save_control_mode_selection = 1;
	}

/*********************************************************************************************
*	原型：int save_green_conflict()
*	功能: 
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int save_green_conflict()
{
	f_save_green_conflict = 1;
	}

/*********************************************************************************************
*	原型：void save_err(void)
*	功能: 保存故障的类型及发生、解除时间
*	说明：故障类型的最高位(B7)为 1 表示故障发生，为 0 表示故障解除
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void save_err(void)
{
unsigned char ctemp0,ctemp1,i;
	if (f_wr_eeprom_now | SM_BUSY) return;	
	if (err_count >= err_count_max) return;										// EEPROM 空间已满不能再存放数据
	if (m_hard_error_bak != g_controller_status.m_error.m_hard_error) {			// 硬件故障
		ctemp0 = m_hard_error_bak ^ g_controller_status.m_error.m_hard_error;
		ctemp1 = 0x01;
		for (i=0x00;i<8;i++) {
			if ((ctemp0 & ctemp1) != 0x00) break;
			ctemp1 = ctemp1 << 1;
			}
		if ((m_hard_error_bak & ctemp1) != 0x00) m_hard_error_bak = m_hard_error_bak & (~ctemp1);	// 故障解除
		else { 																	// 故障发生
			i = i + 0x80;
			m_hard_error_bak = m_hard_error_bak | ctemp1;
			}
		err_data(i+1);
		return;
		} 
	if (m_soft_error_bak != g_controller_status.m_error.m_soft_error) {			// 软件故障
		ctemp0 = m_soft_error_bak ^ g_controller_status.m_error.m_soft_error;
		ctemp1 = 0x01;
		for (i=0x00;i<8;i++) {
			if ((ctemp0 & ctemp1) != 0x00) break;
			ctemp1 = ctemp1 << 1;
			}
		if ((m_soft_error_bak & ctemp1) != 0x00) m_soft_error_bak = m_soft_error_bak & (~ctemp1);	// 故障解除
		else {																	// 故障发生
			i = i + 0x80;
			m_soft_error_bak = m_soft_error_bak | ctemp1;
			}
		err_data(i+17);
		} 
	}

/*********************************************************************************************
*	原型：void err_data(unsigned char ctemp)
*	功能: 故障内容
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void err_data(unsigned char ctemp)
{
	f_save_err = 1;
	err_buf[0] = ctemp;
	err_buf[1] = g_controller_status.m_month;
	err_buf[2] = g_controller_status.m_day;
	err_buf[3] = g_controller_status.m_hour;
	err_buf[4] = g_controller_status.m_minute;	
	err_buf[5] = g_controller_status.m_second;
	}
	
/*********************************************************************************************
*	原型：void fun_smbus(void)
*	功能: 判断是否有数据要写入 EEPROM，并在 SMBUS 空闲时启动写入
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void fun_smbus(void)
{
unsigned int intemp,intemp1;
	
	if (SM_BUSY) return;											// SMBUS 忙

	if (f_set_time) {
		fun_write_rtc();											// 写 RTC 
		return;
		}
	if (!f_wr_eeprom_now) {
		
		if (f_step_all_ok) {										// 保存某相位表的总步数
			f_step_all_ok = 0;
			if (step_all_ram != step_all_flash) {
				BUFF_SMB_TR[0] = step_all_ram;
				SM_Send(CHIP_EEPROM,start_addr_phase_plan_no,0x01,1);
				return;
				}
			}
		
		
		if (f_page_wr) fun_write_EEPROM();							// 后序页写
		else {
			if (f_eeprom_rd) {										// 有数据要从 EEPROM 读 
				if (!f_rd_eeprom_now) {	
//					f_eeprom_rd = 0;
					if (f_load_config_data) {
						f_rd_eeprom_now = 1;
						f_load_config_data = 0;
						rdt_num = 0x00;
						rdt_num_bak = ee_size0;	
						start_addr_ram = &g_config.m_system_param;
						SM_Receive(CHIP_EEPROM,g_config_addr,0x80,1);
						return;		
						}
					if (f_step_all) {
						rdt_num_bak = 0x01;	
						f_rd_eeprom_now = 1;
						SM_Receive(CHIP_EEPROM,start_addr_phase_plan_no,0x01,1);
						return;	
						}
					}
				}
			else {
				if (flag_eeprom0 | flag_eeprom1) {					// 有数据要写入 EEPROM
					if (f_save_config_data) {
						flag_eeprom0 = 0x00;
						flag_eeprom1 = 0x00;
						start_addr_eeprom = g_config_addr;
						wrt_num = ee_size0;	
						start_addr_ram = &g_config.m_system_param;
						fun_write_EEPROM();		
						return;		
						}			
					if (f_save_system_param) {
						f_save_system_param = 0;
						start_addr_eeprom = m_system_param_addr;
						wrt_num = ee_size1;	
						start_addr_ram = &g_config.m_system_param;
						fun_write_EEPROM();
						return;
						}
					if (f_save_lamp_group_setting) {
						f_save_lamp_group_setting = 0;
						start_addr_eeprom = m_lamp_group_setting_addr;
						wrt_num = ee_size2;	
						start_addr_ram = &g_config.m_lamp_group_setting;
						fun_write_EEPROM();
						return;
						}
					if(f_save_lamp_setting){
						f_save_lamp_setting = 0;
						start_addr_eeprom = m_lamp_setting_addr;
						wrt_num = ee_size14;	
						start_addr_ram = &g_config.m_lamp_setting;
						fun_write_EEPROM();
						return;
						}
					if (f_save_detector_setting) {
						f_save_detector_setting = 0;
						start_addr_eeprom = m_detector_setting_addr;
						wrt_num = ee_size3;	
						start_addr_ram = &g_config.m_detector_setting;
						fun_write_EEPROM();
						return;
						}
					if (f_save_phase_plan) {
/*						f_save_phase_plan = 0;
						intemp = (phase_plan_no - 1) * ee_size4a;
						intemp1 = (phase_plan_no_step - 1) * ee_size4b;
						start_addr_phase_plan_no = m_phase_table_addr + intemp;			// 某相位表在 FLASH 中的首地址
						start_addr_eeprom = start_addr_phase_plan_no + intemp1 + 1;		// 某相位表的第 phase_plan_no_step 步在 FLASH 中的首地址
						wrt_num = ee_size4b;	
						start_addr_ram = &g_config.m_phase_table;
						start_addr_ram = start_addr_ram + intemp;						// 某相位表在内存中的首地址
						step_all_ram = *start_addr_ram;									// 某相位表在内存中保存的总步数
						start_addr_ram = start_addr_ram + intemp1 + 1;					// 某相位表的第 phase_plan_no_step 步在内存中的首地址
						fun_write_EEPROM();
						f_step_all = 1;
						f_eeprom_rd = 1;*/
						f_save_phase_plan = 0;
						intemp = (phase_plan_no - 1) * ee_size4a;
						start_addr_eeprom = m_phase_table_addr + intemp;
						wrt_num = ee_size4a;	
						start_addr_ram = &g_config.m_phase_table;
						start_addr_ram = start_addr_ram + intemp;
						fun_write_EEPROM();						
						return;
						}
					if (f_save_timming_plan) {
						f_save_timming_plan = 0;
						intemp = (timming_plan_no - 1) * ee_size5a;
						start_addr_eeprom = m_timming_table_addr + intemp;
						wrt_num = ee_size5a;	
						start_addr_ram = &g_config.m_timming_table;
						start_addr_ram = start_addr_ram + intemp;
						fun_write_EEPROM();
						return;
						}
					if (f_save_schedule_plan) {
						f_save_schedule_plan = 0;
						intemp = (schedule_plan_no - 1) * ee_size6a;
						start_addr_eeprom = m_schedule_table_addr + intemp;
						wrt_num = ee_size6a;	
						start_addr_ram = g_config.m_schedule_table;
						start_addr_ram = start_addr_ram + intemp;
						fun_write_EEPROM();
						return;
						}
					if (f_save_special_day) {
						f_save_special_day = 0;
						start_addr_eeprom = m_special_day_table_addr;
						wrt_num = ee_size7;	
						start_addr_ram = &g_config.m_special_day_table;
						fun_write_EEPROM();
						return;
						}
					if (f_save_multiple_schedule_control_param) {
						f_save_multiple_schedule_control_param = 0;
						start_addr_eeprom = m_multiple_schedule_control_param_addr;
						wrt_num = ee_size8;	
						start_addr_ram = &g_config.m_multiple_schedule_control_param;
						fun_write_EEPROM();
						return;
						}
					if (f_save_actuate_control_param) {
						f_save_actuate_control_param = 0;
						start_addr_eeprom = m_actuate_control_param_addr;
						wrt_num = ee_size9;	
						start_addr_ram = &g_config.m_actuate_control_param;
						fun_write_EEPROM();
						return;
						}
					if (f_save_cordless_coordinate_control_param) {
						f_save_cordless_coordinate_control_param = 0;
						start_addr_eeprom = m_cordless_coordinate_control_param_addr;
						wrt_num = ee_size10;	
						start_addr_ram = &g_config.m_cordless_coordinate_control_param;
						fun_write_EEPROM();
						return;
						}
					if (f_save_comm_param) {
						f_save_comm_param = 0;
						intemp = (comm_param_no - 1) * ee_size11a;
						start_addr_eeprom = m_comm_param_addr + intemp;
						wrt_num = ee_size11a;	
						start_addr_ram = &g_config.m_comm_param;
						start_addr_ram = start_addr_ram + intemp;
						fun_write_EEPROM();
						return;
						}
					if (f_save_control_mode_selection) {
						f_save_control_mode_selection = 0;
						start_addr_eeprom = m_control_mode_selection_addr;
						wrt_num = ee_size12;	
						start_addr_ram = &g_config.m_control_mode_selection;			//0x44dc
// 060615
						if (g_config.m_control_mode_selection.m_control_mode != 0x08) f_manual = 0;
// 060615
						fun_write_EEPROM();
						return;
						}
					if (f_save_green_conflict) {
						f_save_green_conflict = 0;
						start_addr_eeprom = m_green_conflict_addr;
						wrt_num = ee_size13;	
						start_addr_ram = &g_config.m_green_conflict;
						fun_write_EEPROM();
						return;
						}
					
					if (f_save_err) {
						f_save_err = 0;
						start_addr_eeprom = err_save_start_addr + err_count * err_long;
						wrt_num = err_long;	
						start_addr_ram = &err_buf;
						fun_write_EEPROM();
						err_count++;
						return;
						}
					}
				}
			}
		}
	if (wr_eeprom_count == 0x60) get_system_time();							// 读取系统时间
	}

/*********************************************************************************************
*	原型：void fun_write_EEPROM(void)
*	功能: 将缓冲区的数据写入 EEPROM
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void fun_write_EEPROM(void)
{
unsigned char chartemp,i;
unsigned int intemp0,intemp1;
	if (f_page_wr) {
		intemp1 = start_addr_eeprom;
		if (wrt_num >= 0x80) {											// 本次仍不能将待写的字节一次性写完
			chartemp = 0x80;											// 页长度为 128
			start_addr_eeprom = start_addr_eeprom + chartemp;	
			wrt_num = wrt_num - chartemp;
			}
		else {															// 本次可将待写的字节一次性写完
			f_page_wr = 0;
			chartemp = (char)wrt_num;
			}
		}	
	else {	
		intemp0 = (start_addr_eeprom + wrt_num) & 0xff80; 
		intemp0 = intemp0 ^ (start_addr_eeprom & 0xff80); 
		intemp1 = start_addr_eeprom;									// 本次写 EEPROM 的起始地址
		if (intemp0 != 0) {												// 要写的数据跨页
			f_page_wr = 1;												// 一次写不完的标志位
			i = (char)start_addr_eeprom;	
			chartemp = 0x80 - (i & 0x7f);								// 当前页可写的字节数，也就是本次写入 EEPROM 的字节数 
			start_addr_eeprom = start_addr_eeprom + chartemp;			// 下一次写 EEPROM 的起始地址
			wrt_num = wrt_num - chartemp;								// 剩下待写的字节数
			}
		else {
			chartemp = (char)wrt_num;
			}
		}
	for (i=0x00;i<chartemp;i++) {										// 从 RAM 中读出数据放入 SMBUS 发送缓冲区
		BUFF_SMB_TR[i] = *start_addr_ram; 
		start_addr_ram++;
		}
	SM_Send(CHIP_EEPROM,intemp1,chartemp,1);
	}

/*********************************************************************************************
*	原型：void get_system_time(void)
*	功能: 读取系统时间
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
void get_system_time(void)
{
unsigned char ctemp1;
	
	if (!(f_rd_rtc_now | f_rd_smbus_ok)) {
		f_rd_rtc_now = 1;
		SM_Receive(CHIP_RTC,0x0001,0x07,0);                          
		}
	if (f_rd_rtc_now | f_rd_smbus_ok) {
		f_rd_rtc_now = 0;
		f_rd_smbus_ok = 0;
		ctemp1 = ((BUFF_SMB_RD[0] & 0x7f) >> 4) * 10;
		g_controller_status.m_second = ctemp1 + (BUFF_SMB_RD[0] & 0x0f);
		ctemp1 = (BUFF_SMB_RD[1] >> 4) * 10;
		g_controller_status.m_minute = ctemp1 + (BUFF_SMB_RD[1] & 0x0f);
		ctemp1 = ((BUFF_SMB_RD[2] & 0x3f) >> 4) * 10;
		g_controller_status.m_hour = ctemp1 + (BUFF_SMB_RD[2] & 0x0f);
		if (BUFF_SMB_RD[3] == 0x07) g_controller_status.m_week = 0x00;
		else g_controller_status.m_week = BUFF_SMB_RD[3];
		ctemp1 = (BUFF_SMB_RD[4] >> 4) * 10;
		g_controller_status.m_day = ctemp1 + (BUFF_SMB_RD[4] & 0x0f);
		ctemp1 = (BUFF_SMB_RD[5] >> 4) * 10;
		g_controller_status.m_month = ctemp1 + (BUFF_SMB_RD[5] & 0x0f);
		ctemp1 = (BUFF_SMB_RD[6] >> 4) * 10;
		g_controller_status.m_year[1] = ctemp1 + (BUFF_SMB_RD[6] & 0x0f);
		g_controller_status.m_year[0] = 20;
		}
	}



/*********************************************************************************************
*	原型：int load_config_data()
*	功能: 刚开机时载入系统配置参数
*	说明：
*	参数：无                  
*	返回：无                  
*********************************************************************************************/
int load_config_data()
{
	f_eeprom_rd = 1;
	f_load_config_data = 1;
	
//	int i,j;
//	timming_plan_t tm;
//	phase_plan_t ph;
//	// 测试多时段控制使用
//
//	// 设定一个时段方案
//	schedule_plan_t sp;
//
//	g_config.m_control_mode_selection.m_control_mode = CONTROL_MODE_FRONT_PANEL_SELECT;
//	
//	sp.m_time_segment_count = 2;
//	sp.m_hour[0]      = 0;
//	sp.m_minute[0]    = 0;
//	sp.m_phase[0]     = 1;
//	sp.m_timming[0]   = 1;
//
//	sp.m_hour[1]      = 10;
//	sp.m_minute[1]    = 54;
//	sp.m_phase[1]     = 1;
//	sp.m_timming[1]   = 1;
//
//	for(i=0; i<MAX_SCHEDULE_PLAN_COUNT; i++)
//		g_config.m_schedule_table[i] = sp;
//	
//	//
//	tm.m_step = 72;
//	for(i=0; i<MAX_STEP_COUNT; i++)
//		tm.m_timming[i] = 5;
//	g_config.m_timming_table[0] = tm;
//	g_config.m_timming_table[1] = tm;
//	//
//	ph.m_step = 72;
//	for(i=0;i<MAX_STEP_COUNT; i++){
//		unsigned char v;
//		switch(i%4){
//				case 0:
//					v = 0x40;
//					break;
//				case 1:
//					v = 0x10;
//					break;
//				case 2:
//					v = 0x04;
//					break;
//				case 3:
//					v = 0x01;
//					break;
//				default:
//					v = 0x00;
//			}
//	
//		for(j=0;j<MAX_OUTPUT_COUNT;j++){
//			if(j == i/4)
//				ph.m_phase[i].m_output[j] = v;
//			else
//				ph.m_phase[i].m_output[j] = 0;
//		}
//	}
//	g_config.m_phase_table[0] = ph;
//	g_config.m_phase_table[1] = ph;
//	// 多段控制参数
//	g_config.m_multiple_schedule_control_param.m_schedule_plan = 7;
//
//	// 初始化波特率
//	for(i=0; i<MAX_COMM_COUNT; i++)
//		g_config.m_comm_param[i].m_baud = B_9600;

	return 0;

	}

