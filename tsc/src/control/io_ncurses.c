/**
 * io_ncurses.c
 * 
 * 实现底层通信
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-27 16:40:24 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "datastruct.h"
#include "command_reactor.h"

system_config_t g_config;
controller_status_t g_controller_status;
stage_plan_t g_yellow_flash_phase_plan;
stage_plan_t g_all_red_phase_plan;
stage_plan_t g_all_off_phase_plan;
timming_plan_t g_special_phase_timming_plan;
volume_occupy_t g_volume_occupy;
unsigned char key_buf[4],key_count;
// 获取温度，单位为0.1摄氏度
int get_temperature()
{
	return 200;
}
// 门是否非法打开
int is_door_illegal_open()
{
	return 1;
}

int initialize()
{
	return 0;
}
/**
 *  IO 相关函数
 */
// 设置端子状态
int set_term_status(lamp_status_t* phase)
{
	return 0;
}
// 获取灯的故障信息
int get_lamp_health(lamp_health_t* health)
{
	if(access("lamp_failed",0x00) == 0)
		return -1;
	return 0;
}
// 获取检测器的检测信息
int get_detector_status(detector_status_t* status)
{
	if(access("car_on",0x00) == 0)
		memset(status,0xff,sizeof(detector_status_t));
	else
		memset(status,0x00,sizeof(detector_status_t));
	return 0;
}
// 获取检测器的故障信息
int get_detector_health(detector_health_t* health)
{
	memset(health,0,sizeof(detector_health_t));
	return 0;
}
// 获取检测器计数值
int get_detector_counter(detector_counter_t* counter)
{
	return 0;
}
int set_system_time(date_time_t* dt)
{
	return 0;
}

// 载入系统配置参数
int load_config_data()
{
	int i;
	timming_plan_t tm;
	stage_plan_t ph;
	// 测试多时段控制使用
	g_config.m_control_mode_selection.m_control_mode = CONTROL_MODE_FRONT_PANEL_SELECT;

	// 设定一个时段方案
	schedule_plan_t sp;
	sp.m_time_segment_count = 2;
	sp.m_hour[0]      = 0;
	sp.m_minute[0]    = 0;
	sp.m_phase[0]     = 1;
	sp.m_timming[0]   = 1;

	sp.m_hour[1]      = 10;
	sp.m_minute[1]      = 54;
	sp.m_phase[1]     = 2;
	sp.m_timming[1]   = 2;

	for(i=0; i<MAX_SCHEDULE_PLAN_COUNT; i++)
		g_config.m_schedule_table[i] = sp;
	
	//配时方案
	tm.m_step = 4;
	for(i=0; i<MAX_STAGE_COUNT; i++)
	{
			tm.m_greentimming[i] = 3;
			tm.m_greenflashtimming[i]=3;
			tm.m_yellowtimming[i]=3;
			tm.m_redtimming[i]=3;
	}
	g_config.m_timming_table[0] = tm;
	g_config.m_timming_table[1] = tm;
	//相位表
	ph.m_stage_count = 4;
	g_config.m_phase_table[0] = ph;
	g_config.m_phase_table[1] = ph;
	// 多段控制参数
	g_config.m_multiple_schedule_control_param.m_schedule_plan = 7;
	g_config.m_control_mode_selection.m_control_mode = CORDLESS_COORDINATE_CONTROL;
	g_config.m_cordless_coordinate_control_param.m_start_hour = 18;
	g_config.m_cordless_coordinate_control_param.m_start_minute = 10;
	g_config.m_cordless_coordinate_control_param.m_offset = 13;
	g_config.m_cordless_coordinate_control_param.m_phase = 2;
	g_config.m_cordless_coordinate_control_param.m_timming = 2;
	return 0;
}
// 保存系统配置参数
int save_config_data()
{
	return 0;
}
int save_system_param()
{
	return 0;
}
int save_lamp_group_setting()
{
	return 0;
}
int save_detector_setting()
{
	return 0;
}
int save_phase_plan(int plan_no)
{
	return 0;
}
int save_timming_plan(int plan_no)
{
	return 0;
}
int save_schedule_plan(int plan_no)
{
	return 0;
}
int save_special_day()
{
	return 0;
}
int save_multiple_schedule_control_param()
{
	return 0;
}
int save_actuate_control_param()
{
	return 0;
}
int save_cordless_coordinate_control_param()
{
	return 0;
}
int save_comm_param(int param_no)
{
	return 0;
}
int save_control_mode_selection()
{
	return 0;
}

// 是否处于手动控制状态
int is_manual_control_state()
{
	if(access("manual",0x00) == 0)
		return 1;
	return 0;
}
// 前面板设定的控制模式
int get_front_panel_control_mode()
{
	if(access("yellow_flash",0x00) == 0)
		return YELLOW_FLASH_CONTROL;
	else if(access("all_red",0x00) == 0)
		return ALL_RED_CONTROL;
	else if(access("all_off",0x00) == 0)
		return ALL_OFF_CONTROL;
	else
		return MULTIPLE_SCHEDULE_CONTROL;
}
// 是否按了步进按钮
int is_step_forward_button_pressed()
{
	if(access("forward",0x00) == 0){
		g_controller_status.m_step_forward_button_state = 1;
	}
	return g_controller_status.m_step_forward_button_state;
}
// 清除步进按钮状态
int clear_step_forward_button_state()
{
	//DeleteFile("forward");
	g_controller_status.m_step_forward_button_state = 0;
	return 0;
}
// 显示信号机运行状态
void display_status(unsigned char ctemp1,unsigned char ctemp2)
{
	//SYSTEMTIME systime;

	if(ctemp1 != 0)
		return;

	// 更新信号机时间
	/*
	GetLocalTime(&systime);
	g_controller_status.m_year[0] = systime.wYear/100;
	g_controller_status.m_year[1] = systime.wYear%100;
	g_controller_status.m_month = systime.wMonth;
	g_controller_status.m_week  = systime.wDayOfWeek;
	g_controller_status.m_day   = systime.wDay;
	g_controller_status.m_hour  = systime.wHour;
	g_controller_status.m_minute = systime.wMinute;
	g_controller_status.m_second = systime.wSecond;
	*/
	printf("\t==================\n");
	controller_status_t* st = &g_controller_status;
	printf("\t%04d-%02d-%02d %02d:%02d:%02d %02d\n",
			st->m_year[0]*100+st->m_year[1],st->m_month,st->m_day,
			st->m_hour,st->m_minute,st->m_second,
			st->m_week);
	printf("\tm_is_degraded : %d\n",st->m_is_degraded);
	printf("\tm_desire_mode : %d\n",st->m_designate_control_mode);
	printf("\tm_control_mode: %d\n",st->m_actual_control_mode);
	printf("\tm_schedule    : %d\n",st->m_schedule);
	printf("\tm_phase       : %d\n",st->m_phase);
	printf("\tm_timming     : %d\n",st->m_timming);
	printf("\tm_step        : %d\n",st->m_step);
	printf("\tm_assign_time : %d\n",st->m_assign_time);
	printf("\tm_remain_time : %d\n",st->m_remain_time);
	printf("\tm_elapse_time : %d\n",st->m_elapse_time);

}
int get_char()
{
	return getch();
}
/**
 * 初始化串口
 */
static int init(int fd)
{
	struct termios ios;
	speed_t baud = B9600;
	// 设置串口属性
	ios.c_iflag = 0;
	ios.c_oflag = 0;
	ios.c_cflag = 0;
	ios.c_lflag = 0;

	ios.c_iflag |= (IGNBRK | IGNPAR) ;
	// ios.c_oflag |= OPOST;
	ios.c_cflag |= (CS8 | CREAD | CLOCAL) ;
	// ios.lflag
	
	if(cfsetispeed(&ios,baud))
		return -1;
	if(cfsetospeed(&ios,baud))
		return -1;

	return tcsetattr(fd,TCSANOW,&ios);
}
/**
 *  打开指定的通信端口
 */
int open_comm(int port)
{
	char name[20];
	sprintf(name,"/dev/cpld%d",port-1);

	return open(name,O_RDWR | O_NOCTTY | O_NONBLOCK);
	/*
	if(init(fd) != 0)
		return -1;
	else
		return fd;
	*/
}

int read_comm(int fd, byte* buf, int len)
{
	return read(fd,buf,len);	
}

int write_comm(int fd, byte* buf, int len)
{
	int data_to_write = 0;
	while(data_to_write < len){
		int ret = write(fd,buf+data_to_write,len-data_to_write);
		if(ret > 0)
			data_to_write += ret;
		else{
			return -1;
		}
	}
	return len;
}

int close_comm(int fd)
{
	return close(fd);
}

int wait_for_100ms()
{
	usleep(100000);
	return 0;
}
