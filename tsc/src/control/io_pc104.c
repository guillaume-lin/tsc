/**
 * io_pc104.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-02 11:10:24 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <windows.h>
#include "io.h"
#include "io_pc104.h"
#include "command_reactor.h"
#include "kernel.h"

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
int set_term_status(phase_t* phase)
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
// 等待100ms
int wait_for_100ms()
{
	Sleep(100);
	/*
	Sleep(50);
	reactor_handle_command(&g_command_reactor);
	Sleep(50);
	reactor_handle_command(&g_command_reactor);
	*/
	return 0;
}

// 载入系统配置参数
int load_config_data()
{
	int i;
	timming_plan_t tm;
	phase_plan_t ph;
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
	
	//
	tm.m_step = 10;
	for(i=0; i<MAX_STEP_COUNT; i++)
		tm.m_timming[i] = 3;
	g_config.m_timming_table[0] = tm;
	g_config.m_timming_table[1] = tm;
	//
	ph.m_step = 10;
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
int save_phase_plan(int plan_no,int step)
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
	DeleteFile("forward");
	g_controller_status.m_step_forward_button_state = 0;
	return 0;
}
// 显示信号机运行状态
void display_status(unsigned char ctemp1,unsigned char ctemp2)
{
	SYSTEMTIME systime;


	// 更新信号机时间
	GetLocalTime(&systime);
	g_controller_status.m_year[0] = systime.wYear/100;
	g_controller_status.m_year[1] = systime.wYear%100;
	g_controller_status.m_month = systime.wMonth;
	g_controller_status.m_week  = systime.wDayOfWeek;
	g_controller_status.m_day   = systime.wDay;
	g_controller_status.m_hour  = systime.wHour;
	g_controller_status.m_minute = systime.wMinute;
	g_controller_status.m_second = systime.wSecond;

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

static int Init(HANDLE handle)
{
	DCB dcb;
	BOOL b1 = GetCommState(
			handle, // HANDLE hFile,
			&dcb	// LPDCB lpDCB
			);
	if(!b1)
		return -1;
	dcb.DCBlength = sizeof(DCB);
	dcb.fParity  = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX  = FALSE;
	dcb.fNull = FALSE;
	dcb.fAbortOnError = FALSE;

	dcb.ByteSize = 8;		// 字节为8位
	dcb.Parity   = NOPARITY;	// 没有奇偶校验
	dcb.StopBits = ONESTOPBIT;	// 一个停止位
	dcb.BaudRate = 9600;		// 9600 波特率

	BOOL b2 = SetCommState(
			handle, // HANDLE hFile,
			&dcb	// LPDCB lpDCB
			);

	if(!b2)
		return -1;

	COMMTIMEOUTS ct;
//	typedef struct _COMMTIMEOUTS {  
//		DWORD ReadIntervalTimeout;  
//		DWORD ReadTotalTimeoutMultiplier;  
//		DWORD ReadTotalTimeoutConstant;  
//		DWORD WriteTotalTimeoutMultiplier;  
//		DWORD WriteTotalTimeoutConstant;
//	} COMMTIMEOUTS, *LPCOMMTIMEOUTS;

//	设定串口立即返回
	ct.ReadIntervalTimeout = MAXDWORD;
	ct.ReadTotalTimeoutMultiplier = 0;
	ct.ReadTotalTimeoutConstant = 0;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;

	BOOL b3 = SetCommTimeouts(
			handle, // HANDLE hFile,
			&ct	// LPCOMMTIMEOUTS lpCommTimeouts
			);

	if(!b3)
		return -1;

	return 0;
}

HANDLE s_handle1;
HANDLE s_handle2;
/**
 *  打开指定的通信端口
 */
int open_comm(int port)
{
	HANDLE handle;
	char* name;
	if(port == 1 )
		name = "com1";
	else if(port == 2)
		name = "com2";
	else
		return -1;
	
	handle = CreateFile(
			name,	// LPCTSTR lpFileName,
			GENERIC_READ | GENERIC_WRITE ,// DWORD dwDesiredAccess,
			0,	// DWORD dwShareMode,
			NULL,	// LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			OPEN_EXISTING, // DWORD dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL,// DWORD dwFlagsAndAttributes,
			NULL	// HANDLE hTemplateFile
			);

	if(handle != INVALID_HANDLE_VALUE)
	{
		if(Init(handle)!= 0)
			return -1;
	}
	else
		return -1;

	if(port == 1){
		s_handle1 = handle;
		return 1;
	}
	else {
		s_handle2 = handle;
		return 2;
	}
}

int read_comm(int fd, byte* buf, int len)
{
	HANDLE handle;
	if(fd == 1)
		handle = s_handle1;
	else if(fd == 2)
		handle = s_handle2;
	else
		return -1;

	DWORD bytesRead = 0;
	BOOL b = ReadFile(
			handle, // HANDLE hFile,
			buf,		// LPVOID lpBuffer,
			len,	// DWORD nNumberOfBytesToRead,
			&bytesRead, // LPDWORD lpNumberOfBytesRead,
			NULL	// LPOVERLAPPED lpOverlapped
			);

	if(!b)
		return -1;
	else
		return bytesRead;
}

int write_comm(int fd, byte* buf, int len)
{
	HANDLE handle;
	if(fd == 1)
		handle = s_handle1;
	else if(fd == 2)
		handle = s_handle2;
	else
		return -1;

	DWORD bytesWrite = 0;
	BOOL b = WriteFile(
			handle, // HANDLE hFile,
			buf, 	// LPCVOID lpBuffer,
			len,	// DWORD nNumberOfBytesToWrite,
			&bytesWrite,	// LPDWORD lpNumberOfBytesWritten,
			NULL	// LPOVERLAPPED lpOverlapped
			);

	if(!b)
		return -1;
	else
		return bytesWrite;
}

int close_comm(int fd)
{
	if(fd == 1)
		CloseHandle(s_handle1);
	else if(fd == 2)
		CloseHandle(s_handle2);
	else
		return -1;

	return 0;
}
