/**
 * kernel.c
 * 
 * 主控制循环实现文件
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-06-25 10:16:00 中国标准时间
 * 
 * $log$
 *
 */
//#include <intrins.h>
#include <assert.h>
#include <string.h>
#include "datastruct.h"
#include "io.h"
#include "kernel.h"
#include "util.h"
#include "vm.h"
#include "multiple_schedule_control.h"
#include "actuate_control.h"
#include "cordless_coordinate_control.h"
#include "executive_machine_control.h"
#include "manual_control.h"
#include "command_reactor.h"

// 定义要用到的全局数据结构
system_config_t g_config;	// 配置数据

volume_occupy_group_t g_volume_occupy;

stage_plan_t g_special_phase_plan;
lamp_status_t g_special_phase;
timming_plan_t g_special_phase_timming_plan;

// 信号机的全局状态
controller_status_t g_controller_status;
#ifndef __C51__
typedef byte bit;
#endif
command_reactor_t g_command_reactor2,g_command_reactor7,g_command_reactor8;	
extern void wait_for_10ms(void);

/**
 *  初始化特殊多段参数
 *
 */
//0921 int init_special_schedule_data()

int init_special_schedule_data(unsigned char ctemp)
{
	int i;

	// 把三个特殊相位清零
	for(i=0; i<MAX_TERMINAL_COUNT; i++)
	{
		g_special_phase.m_output[i] = 0;
	}
	// 设置相位值
	g_special_phase_plan.m_stage_count=1;
	for(i=0;i<MAX_STAGE_COUNT;i++)
	{
		g_special_phase_plan.m_stage[0].m_phase[i]=0;
	}
	// 初始化特殊配时
	g_special_phase_timming_plan.m_step = 1; 			// 认为特殊相位只有1步
	for(i=0; i<MAX_STAGE_COUNT; i++)
	{
		if (ctemp == ALL_RED_CONTROL) 				// 初始化全红相位
		{
			g_special_phase_timming_plan.m_greentimming[i] = 3; // 每步只有3秒
			g_special_phase_timming_plan.m_greenflashtimming[i]=0;
			g_special_phase_timming_plan.m_yellowtimming[i]=0;
			g_special_phase_timming_plan.m_redtimming[i]=0;
		}
		else  if (ctemp == YELLOW_FLASH_CONTROL)
		{
			g_special_phase_timming_plan.m_greentimming[i] = 3; // 每步只有3秒
			g_special_phase_timming_plan.m_greenflashtimming[i]=0;
			g_special_phase_timming_plan.m_yellowtimming[i]=0;
			g_special_phase_timming_plan.m_redtimming[i]=0;
		}
		else if(ctemp ==ALL_OFF_CONTROL)
		{
			g_special_phase_timming_plan.m_greentimming[i] = 0; // 每步只有3秒
			g_special_phase_timming_plan.m_greenflashtimming[i]=0;
			g_special_phase_timming_plan.m_yellowtimming[i]=0;
			g_special_phase_timming_plan.m_redtimming[i]=0;
		}
	}

	for(i=0;i<MAX_LAMP_GROUP_COUNT;i++){
		int r,y;
		if (ctemp == YELLOW_FLASH_CONTROL) {					// 初始化黄闪相位
			y = g_config.m_lamp_group_setting.m_lamp_group[i][YELLOW];
			if(y > 0){
				// 将该端子输出设为闪
				int p = (y-1)/4;		// 端子所在的字节
				int offset = (y-1)%4;		// 字节里面的偏移0-3
				//0921				byte* b = &g_yellow_flash_phase_plan.m_phase[0].m_output[p];
				byte* b = &g_special_phase.m_output[p];
				byte flash = 0x02 << 2*(3 - offset);
				*b &= ~(0x03 << 2*(3 - offset)); //先清零
				*b |= flash;			// 后赋值
			}
		}
		if (ctemp == ALL_RED_CONTROL) {					// 初始化全红相位
			r = g_config.m_lamp_group_setting.m_lamp_group[i][RED];
			if(r > 0){
				// 将该端子设为亮
				int p = (r-1)/4;
				int offset = (r-1)%4;
				//0921				byte* b = &g_all_red_phase_plan.m_phase[0].m_output[p];
				byte* b = &g_special_phase.m_output[p];
				byte on = 0x01 << 2*(3 - offset);
				*b &= ~(0x03 << 2*(3- offset));	// 先清零
				*b |= on;			// 后赋值
			}
		}
	}
	return 0;
}


/**
 * 判断今日是否为特殊日
 * 如果是的话,返回该日应当运行的时段方案
 */
static int is_today_special_day()
{
	int i;
	for(i=0; i<MAX_SPECIAL_DAY_COUNT; i++){
		special_day_t sday;
		
		int month = g_controller_status.m_month;
		int day = g_controller_status.m_day;
 		sday = g_config.m_special_day_table.m_day[i];
		if(sday.m_start_month > month)
			continue;
		if((sday.m_start_month == month) && (sday.m_start_day > day))
			continue;
		if(sday.m_end_month < month)
			continue;
		if((month == sday.m_end_month) && (sday.m_end_day < day))
			continue;

		return sday.m_schedule;
	}

	return 0;
}
// 确定多时段状态下面应该走的相位方案和配时方案
int get_next_schedule(byte* schedule,byte* phase,byte* timming,byte second)
{
	int time,count;
	int i,plan;
	schedule_plan_t schedule_plan;

	// 首先判断要使用哪个时段方案
	// 1. 是否使用特定的时段方案
	if(g_config.m_multiple_schedule_control_param.m_schedule_plan > 7){
		int p = g_config.m_multiple_schedule_control_param.m_schedule_plan;
		schedule_plan = g_config.m_schedule_table[p-1];
		*schedule = p;
		// 2. 是否启用特殊日控制
	}else if(g_config.m_multiple_schedule_control_param.m_enable_special &&
		(plan = is_today_special_day()) > 0){
			*schedule = plan;
			schedule_plan = g_config.m_schedule_table[plan-1]; 
		}
		// 3. 按缺省方式选择时段方案
	else{
		int t = g_controller_status.m_week;
		if(t == 0)
			t = 7;		// 星期日使用时段方案7

		schedule_plan = g_config.m_schedule_table[t-1];
		*schedule = t;
	}

	// 根据确定时段，选择相位以及配时 
	if(g_controller_status.m_second+second>59)
		time = g_controller_status.m_hour*60 + g_controller_status.m_minute+1;
	else
		time = g_controller_status.m_hour*60 + g_controller_status.m_minute;
	count = 0;
	for(i=0; i<MAX_TIME_SEGMENT_COUNT; i++){
		if(schedule_plan.m_phase[i] == 0)
			break;
		count++;

	}
	*phase   = 0;
	*timming = 0;   

	if(count == 0)//表示该时段内容为空，不合法的数据
		return 0;

	*phase = schedule_plan.m_phase[count-1];
	*timming = schedule_plan.m_timming[count-1];

	for(i=0; i<count-1 ;i++){

		int t1 = schedule_plan.m_hour[i]*60 + schedule_plan.m_minute[i];
		int t2 = schedule_plan.m_hour[i+1]*60 + schedule_plan.m_minute[i+1];
		if( time >= t1 && time < t2 ){
			*phase   = schedule_plan.m_phase[i];
			*timming = schedule_plan.m_timming[i];
			break;
		}
	}

	return 0;
}
// 确定多时段状态下面应该走的相位方案和配时方案
int get_current_schedule(byte* schedule,byte* phase,byte* timming)
{
	int time,count;
	int i,plan;
	schedule_plan_t schedule_plan;

	// 首先判断要使用哪个时段方案
	// 1. 是否使用特定的时段方案
	if(g_config.m_multiple_schedule_control_param.m_schedule_plan > 7){
		int p = g_config.m_multiple_schedule_control_param.m_schedule_plan;
		schedule_plan = g_config.m_schedule_table[p-1];
		*schedule = p;
	// 2. 是否启用特殊日控制
	}else if(g_config.m_multiple_schedule_control_param.m_enable_special &&
			(plan = is_today_special_day()) > 0){
		*schedule = plan;
		schedule_plan = g_config.m_schedule_table[plan-1]; 
	}
	// 3. 按缺省方式选择时段方案
	else{
		int t = g_controller_status.m_week;
		if(t == 0)
			t = 7;		// 星期日使用时段方案7

		schedule_plan = g_config.m_schedule_table[t-1];
		*schedule = t;
	}

	// 根据确定时段，选择相位以及配时 
	
	time = g_controller_status.m_hour*60 + g_controller_status.m_minute;

    count = 0;
    for(i=0; i<MAX_TIME_SEGMENT_COUNT; i++){
    	if(schedule_plan.m_phase[i] == 0)
    		break;
    	count++;
    		
    }
	*phase   = 0;
	*timming = 0;   

    if(count == 0)//表示该时段内容为空，不合法的数据
		return 0;

	*phase = schedule_plan.m_phase[count-1];
	*timming = schedule_plan.m_timming[count-1];

	for(i=0; i<count-1 ;i++){
		
		int t1 = schedule_plan.m_hour[i]*60 + schedule_plan.m_minute[i];
		int t2 = schedule_plan.m_hour[i+1]*60 + schedule_plan.m_minute[i+1];
	   	if( time >= t1 && time < t2 ){
			*phase   = schedule_plan.m_phase[i];
			*timming = schedule_plan.m_timming[i];
			break;
		}
	}

	return 0;
}
///*
// *	转换到特定相位和配时
// */
//int switching_with_schedule(byte schedule,byte phase, byte timming)
//{
//	// 检查当前相位和配时是否合法
//	if(!is_schedule_valid(phase,timming)){
//		// 时段数据不合法
//		set_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
//		set_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
//		return -1;
//	}
//	else {
//		clear_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
//		clear_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
//	} 
//
//	load_current_schedule(schedule,phase,timming);
//	g_controller_status.m_step=1;
//	g_controller_status.m_stage = 1;
//	if(g_controller_status.m_current_phase_plan_p->m_stage_count>1)
//		g_controller_status.m_next_stage=2;
//	else
//		g_controller_status.m_next_stage=1;
//	if ((phase == PHASE_YELLOW_FLASH) || (phase == PHASE_ALL_RED) || (phase == PHASE_ALL_OFF))
//	{
//		// 更新时间
//		g_controller_status.m_elapse_time = 0;
//		// 确定新步的配时
//		set_phase_time();
//		// 设置新的灯状态
//		set_term_status(&g_controller_status.m_current_lamp_status);
//	}
//	else
//		run_current_step();
//	return 0;
//}
/**
 *  载入当前使用的相位方案和配时方案
 */
int load_current_schedule(byte schedule,byte phase, byte timming)
{
	g_controller_status.m_schedule = schedule;
	g_controller_status.m_phase = phase;
	g_controller_status.m_timming = timming;

//	g_controller_status.m_current_timming_plan_p = &g_special_phase_timming_plan;
	if ((phase == PHASE_YELLOW_FLASH) || (phase == PHASE_ALL_RED) || (phase == PHASE_ALL_OFF))
	{
			if(phase==PHASE_ALL_OFF)
				init_special_schedule_data(ALL_OFF_CONTROL);
			else if(phase==PHASE_ALL_RED)
				init_special_schedule_data(ALL_RED_CONTROL);
			else if(phase==PHASE_YELLOW_FLASH)
				init_special_schedule_data(YELLOW_FLASH_CONTROL);
			g_controller_status.m_current_phase_plan_p=&g_special_phase_plan;
			g_controller_status.m_current_timming_plan_p=&g_special_phase_timming_plan;
			g_controller_status.m_current_lamp_status = g_special_phase;
			g_controller_status.m_next_phase_plan_p=g_controller_status.m_next_phase_plan_p;
	}
	else 
	{
		g_controller_status.m_current_phase_plan_p = &g_config.m_phase_table[g_controller_status.m_phase - 1];
		g_controller_status.m_next_phase_plan_p=g_controller_status.m_next_phase_plan_p;
		g_controller_status.m_current_timming_plan_p = &g_config.m_timming_table[g_controller_status.m_timming - 1];
	}

	return 0;
}
int is_schedule_valid(byte phase,byte timming)
{
	byte ps,ts;
	if(phase == PHASE_YELLOW_FLASH ||
	   phase == PHASE_ALL_RED ||
	   phase == PHASE_ALL_OFF)//特殊相位，使用默认配时。
	{
		return 1;
	}

	if(phase == 0 || timming == 0)
		return 0;// 若查表得到的相位方案或配时方案号为 0 时，表示不合法
	else if(phase > MAX_PHASE_PLAN_COUNT || timming > MAX_TIMMING_PLAN_COUNT)
		return 0;//大于最大配置，不合法，返回0
	
	ps = g_config.m_phase_table[phase-1].m_stage_count;
	ts = g_config.m_timming_table[timming-1].m_step;
	if(ps <= 0 || ts <= 0 || ps != ts)// 若查表得到的相位方案或配时方案号的总步数不一致或为 0 时，表示不合法
		return 0;				
	return 1;
}
// 判断当前步是否是最后一步
int is_last_step()
{
	// 查找当前相位的步伐数，判断是否已经运行到最大步伐数
	if(g_controller_status.m_stage >= g_controller_status.m_current_phase_plan_p->m_stage_count)
		if(g_controller_status.m_step>=4)
			return 1;
	return 0;
}
//判断当前步是否是最后阶段的第2步，在最后阶段的第2步，要获取下个时间要走的相位方案和配时方案
int is_last_stage_first_step()
{
	if(g_controller_status.m_stage >= g_controller_status.m_current_phase_plan_p->m_stage_count)
		if(g_controller_status.m_step==1)
			return 1;
	return 0;
}
// 判断当前步是否已经走完
int is_step_end()
{
	if(g_controller_status.m_remain_time > 0)
		return 0;
	else
		return 1;

}
//判断无线协调时间段是否已经到达
int  is_cordless_coordinate_time()
{
	int hour,minute;
	unsigned char phase;
	unsigned char timming;
	//判断时间是否到了
	hour = g_config.m_cordless_coordinate_control_param.m_start_hour;
	minute = g_config.m_cordless_coordinate_control_param.m_start_minute;


	if((g_controller_status.m_hour-hour)*(long)3600+
		(g_controller_status.m_minute-minute)*60+
		g_controller_status.m_second < 0)
	{
		// 时间还没到
		return -1;
	}
	//判断相位，配时是否有效
	phase = g_config.m_cordless_coordinate_control_param.m_phase;
	timming = g_config.m_cordless_coordinate_control_param.m_timming;
	if(!is_schedule_valid(phase,timming))
		return -1;

	return 0;
}
//获取在使用的线圈检测器，判断线圈的状态好坏
//有个线圈坏则返回-1，否则返回0
int judge_detector_state()
{
	int i=0;
	detector_set_t set;
	empty_set(&set);
	//获取当前的相位方案中用到的线圈检测器
	for(i=0;i<g_config.m_lamp_group_setting.m_lamp_group_count;i++)
	{
		get_lamp_group_detector(i,&set);
	}
	// 判断是否有检测器故障
	if(is_detect_state(&set)==-1) 
		return -1;
	return 0;
}
//判断检测器的状态
int  is_detect_state(detector_set_t *pset)
{
	int i,v;
	get_detector_health(&g_controller_status.m_detector_health);
	//判断线圈检测器是否有故障
	for(i=0; i<MAX_DETECTOR_COUNT; i++)
	{
		if(!is_in_set(i+1,pset))
			continue;
		v = get_bit_value(g_controller_status.m_detector_health.m_health,(i+1),1);
		if(v > 0)
			return -1;
	}
	return 0;
}
 //判断信号灯状态
int  is_lamp_state()
{
	int i,u,v;
	get_lamp_health(&g_controller_status.m_lamp_health);
	// 判断是否有灯组是否有故障
	for(i=0; i<MAX_TERMINAL_COUNT; i++)
	{
		u = get_bit_value(g_config.m_lamp_setting.m_use,(i+1),1);
		if(u > 0)//该信号灯启用
		{
			v = get_bit_value(g_controller_status.m_lamp_health.m_health,(i+1),1);
			if(v > 0) return -1;
		}
	}
	return  0;
}
//设置当前的阶段数和对应的相位数
int  set_current_stage()
{
	if(is_last_step())
	{
		g_controller_status.m_step = 1;
		g_controller_status.m_stage=1;
		if(g_controller_status.m_current_phase_plan_p->m_stage_count==1)
			g_controller_status.m_next_stage=1;
		else
			g_controller_status.m_next_stage=2;
	}
	else
	{
		if(g_controller_status.m_step<4)
		{

			g_controller_status.m_step++;
		}
		else
		{
			g_controller_status.m_stage++;
			g_controller_status.m_step=1;
			if(g_controller_status.m_current_phase_plan_p->m_stage_count==g_controller_status.m_stage)
				g_controller_status.m_next_stage=1;
			else
				g_controller_status.m_next_stage++;
		}
	}
	return 0;
}

// 判断是否一个周期运行结束
//w	int is_cycle_end()
//w	{
//w		return is_last_step() && is_step_end();
//w	}

// 根据全局状态中的步号, 输出当前步
int run_current_step()
{
	memset(&g_controller_status.m_current_lamp_status,0,sizeof(lamp_status_t));
	// 更新时间
	g_controller_status.m_elapse_time = 0;
	// 确定新步的配时
	set_phase_time();
	if(g_controller_status.m_remain_time>0)
	{
		// 确定新的相位
		set_phase_state(&g_controller_status.m_current_lamp_status);	
		// 设置新的灯状态
		set_term_status(&g_controller_status.m_current_lamp_status);
	}
	return 0;
}
/**
 * 黄闪控制
 */
int yellow_flash_control(int mode_switch, int timer)
{
	// 设置灯色为黄闪
	if(mode_switch){
		init_special_schedule_data(YELLOW_FLASH_CONTROL);
		load_current_schedule(0,PHASE_YELLOW_FLASH,TIMMING_SPECIAL_PHASE);
		g_controller_status.m_step = 1;
		g_controller_status.m_stage=1;
		g_controller_status.m_next_stage=1;
		// 更新时间
		g_controller_status.m_elapse_time = 0;
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = timer;
		// 设置新的灯状态
		set_term_status(&g_controller_status.m_current_lamp_status);
	}
	return 0;
}
/**
 * 四面红控制
 */
int all_red_control(int mode_switch, int timer)
{
	// 设置灯色为四面红
	if(mode_switch){
		init_special_schedule_data(ALL_RED_CONTROL);
		load_current_schedule(0,PHASE_ALL_RED,TIMMING_SPECIAL_PHASE);
		g_controller_status.m_step = 1;
		g_controller_status.m_stage=1;
		g_controller_status.m_next_stage=1;
		// 更新时间
		g_controller_status.m_elapse_time = 0;
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = timer;
		// 设置新的灯状态
		set_term_status(&g_controller_status.m_current_lamp_status);
	}
	return 0;
}
/**
 * 灭灯控制
 */
int all_off_control(int mode_switch, int timer)
{
	if(mode_switch){
		init_special_schedule_data(ALL_OFF_CONTROL);
		load_current_schedule(0,PHASE_ALL_OFF,TIMMING_SPECIAL_PHASE);
		g_controller_status.m_step = 1;
		g_controller_status.m_stage=1;
		g_controller_status.m_next_stage=1;
		// 更新时间
		g_controller_status.m_elapse_time = 0;
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = timer;
		// 设置新的灯状态
		set_term_status(&g_controller_status.m_current_lamp_status);
	}
	return 0;
}

/**
 * 根据当前时段的配时，运行完当前周期
 * 返回 1 表示过渡完毕
 * 
 */
int switching_with_multiple_schedule(int mode_switch, int timer)
{
	if(timer == 0){
		if(is_step_end() && is_last_step()){
			// 一个周期已经运行完毕
			// 根据需要运行新的时段
			return 1;
		}
		// 仅仅当前步运行完毕
		else if(is_step_end()){
			// 更新步号
			if(is_last_step())
				return 1;
			else {
				set_current_stage();
				run_current_step();
			}
		}

	}
	return 0;
}
/**
 * 采用先黄闪后四面红的方式进行过渡
 * 返回1表示过渡完毕
 */
int switching_with_yellow_flash_all_red(int mode_switch, int yellow, int red, int timer)
{
	static int stage = 0;
	static int yellow_time = 0;
	static int red_time = 0;

	if(mode_switch){
		yellow_time = yellow;
		red_time = red;
		yellow_flash_control(1,0);	
		stage = 1;	// 进入黄闪
	}


	switch(stage){
		case 1:	// 黄闪
			if(yellow_time == 0){
				// 进入四面红
				all_red_control(1,0);
				stage = 2;
			}
			if(timer == 0)
				yellow_time--;
			break;
		case 2: // 四面红

			if(red_time == 0){
				// 结束四面红
				return 1;
			}
			if(timer == 0)
				red_time--;
			break;
		default:
			break;
	}
	return 0;
}

typedef struct {
	// 控制模式代码
	byte m_mode;
	// 控制模式
	control_mode_t m_control_mode;
	// 降级用的控制模式
	control_mode_t m_backup_control_mode;
}control_mode_entry_t;

/**
 * 控制策略集合
 */
static 
#ifdef __C51__	// C51编译器希望表格用code来修饰
code
#endif
control_mode_entry_t control_mode_table[] = {
	// 灭灯
	{ALL_OFF_CONTROL,all_off_control,all_off_control,},
	// 全红
	{ALL_RED_CONTROL,all_red_control,all_off_control,},
	// 黄闪
	{YELLOW_FLASH_CONTROL,yellow_flash_control,all_off_control,},
	// 多段
	{MULTIPLE_SCHEDULE_CONTROL,multiple_schedule_control,yellow_flash_control,},
	// 感应
	{ACTUATE_CONTROL,actuate_control,multiple_schedule_control},
	
	// 无线缆协调控制
	{CORDLESS_COORDINATE_CONTROL,cordless_coordinate_control,actuate_control,},

	// 联机协调控制
	// {ONLINE_HARMONY_CONTROL,actuate_control,},
	// 执行机控制
	{EXECUTIVE_MACHINE_CONTROL,executive_machine_control,actuate_control,},

	// 手动控制
	{MANUAL_CONTROL,manual_control,multiple_schedule_control,},
};
static int get_mode(control_mode_t control_mode)
{
//	int i;
	unsigned char i;
//	int count = sizeof(control_mode_table)/sizeof(control_mode_entry_t);
	unsigned char count = sizeof(control_mode_table)/sizeof(control_mode_entry_t);
	int mode = -1;
	for(i=0; i<count; i++){
		if(control_mode_table[i].m_control_mode == control_mode){
			mode = control_mode_table[i].m_mode;
			break;
		}
	}
	return mode;
}
/**
 *  获取指定控制模式的控制子程序
 *
 */
static control_mode_t get_control_mode(int mode)
{
	int i;
	int count = sizeof(control_mode_table)/sizeof(control_mode_entry_t);
	control_mode_t control_mode = 0;
	for(i=0; i<count; i++){
		if(control_mode_table[i].m_mode == mode){
			control_mode = control_mode_table[i].m_control_mode;
			break;
		}
	}
	
	if(control_mode == 0)
		control_mode = multiple_schedule_control;

	return control_mode;
}
/**
 * 获取指定控制模式的降级控制子程序
 *
 */
static control_mode_t get_backup_control_mode(int mode)
{
	int i;
	int count = sizeof(control_mode_table)/sizeof(control_mode_entry_t);
	
	control_mode_t control_mode = 0;
	for(i=0;i<count;i++){
		if(control_mode_table[i].m_mode == mode){
			control_mode = control_mode_table[i].m_backup_control_mode;
			break;
		}
	}

	if(control_mode == 0)
		control_mode = yellow_flash_control;

	return control_mode;
}
//判断当前的控制模式的条件是否满足
int judge_ctrol_mode_condition(int nMode)
{
	switch(nMode) {
	case EXECUTIVE_MACHINE_CONTROL:// 执行机控制
		{
			if(g_controller_status.m_online_command.m_cmd!=ONLINE)//收到联机命令
					return -1;
			if(!is_detect_health_flag())//要求检测
			{
				// 判断是否有检测器故障
//				if(judge_detector_state()==-1)
//					return -1;
				// 判断是否有信号灯是否有故障
				if(is_lamp_state()==-1)
					return 0;
			}
			return -1;
		}
		break;
	case ONLINE_HARMONY_CONTROL:// 联机协调控制
		break;
	case CORDLESS_COORDINATE_CONTROL:// 无线缆协调控制
		{
			//判断时段是否到了
			if(is_cordless_coordinate_time()==-1)
				return -1;
			if(!is_detect_health_flag())//要求检测
			{
				// 判断是否有检测器故障
//				if(judge_detector_state()==-1)
//					return -1;
				// 判断是否有信号灯是否有故障
				if(is_lamp_state()==-1)
					return -1;
				return 0;
			}
			return -1;
		}
		break;
	case ACTUATE_CONTROL:// 感应控制
		{
			if(!is_detect_health_flag())//要求检测
			{
				// 判断是否有检测器故障
				if(judge_detector_state()==-1)
					return -1;
				// 判断是否有信号灯是否有故障
				if(is_lamp_state()==-1)
					return -1;
				return 0;
			}
			return -1;
		}
		break;
	case MULTIPLE_SCHEDULE_CONTROL:// 多段控制
		{
			if(!is_detect_health_flag())//要求检测
			{
				// 判断是否有信号灯是否有故障
				if(is_lamp_state()==-1)
					return -1;
				return 0;
			}
			return -1;
		}
		break;
	case YELLOW_FLASH_CONTROL:// 黄闪控制
		return 0;
	case ALL_RED_CONTROL:// 全红控制
		return 0;
	case ALL_OFF_CONTROL:// 灭灯控制
		return 0;
	default:
		break;
	}
	return -1;
}
control_mode_t get_current_ctrol_mode(int nNeedMode,int nCurrentMode)
{
	int nMode=nNeedMode;
	int nFirst=1;
	while(nMode!=nCurrentMode)
	{
		if(judge_ctrol_mode_condition(nMode)==0)//该模式要求的条件已经达到
		{
			if(nFirst)
			{
				g_controller_status.m_is_degraded=0;
				g_controller_status.m_degraded_time=0;
			}
			return get_control_mode(nMode);
		}
		else
		{
			control_mode_t control_mode=0;
			nFirst=0;
			control_mode=get_backup_control_mode(nMode);
			nMode=get_mode(control_mode);
			if(nMode==-1)//不存在的方式
				return multiple_schedule_control;
		}
	}
	return get_control_mode(nMode);
}
/**
 * 确定下一个时钟嘀嗒要运行的控制模式
 *
 */
static control_mode_t do_schedule(int control_result,control_mode_t last_control)
{
	control_mode_t control_mode;
	int mode;
	unsigned char i;
	bit run_mode;
	mode = g_controller_status.m_designate_control_mode;

	if(is_manual_control_state()){																	// 先判断是否是手动控制
		g_controller_status.m_designate_control_mode = MANUAL_CONTROL;
	} 												
	else {
		run_mode=get_run_mode();                                                                //是否按照信号机前面板设定的工作模式，1－是
		if (run_mode) {																			// 按照信号机前面板上的设定运行
			clear_run_mode();
			i = get_front_panel_control_mode();
			if (i!=0x00) // 信号机前面板上的设定有效时，以前面板上的设定运行
			{
				if(g_controller_status.m_is_degraded==0)
					g_controller_status.m_designate_control_mode = i;
				else 
				{
					if(i!=g_controller_status.m_actual_control_mode&&i!=g_controller_status.m_designate_control_mode)
					{
						g_controller_status.m_designate_control_mode = i;
						g_controller_status.m_is_degraded = 0;
						g_controller_status.m_degraded_time=0;
					}
				}
			}
//			else 
//				g_controller_status.m_designate_control_mode = g_config.m_control_mode_selection.m_control_mode;
		}
//		else 
//			g_controller_status.m_designate_control_mode = g_config.m_control_mode_selection.m_control_mode;
	}
	//if(mode != g_controller_status.m_designate_control_mode)
	//{
	//	// 控制模式改变，清空降级标志
	//	g_controller_status.m_is_degraded = 0;
	//	g_controller_status.m_degraded_time=0;
	//}
	control_mode = last_control;
	mode = get_mode(last_control);
	if(mode != g_controller_status.m_designate_control_mode && 
		!g_controller_status.m_is_degraded)
	{

		// 新的控制模式
		control_mode = get_control_mode(g_controller_status.m_designate_control_mode);
		g_controller_status.m_is_degraded = 0;
		g_controller_status.m_degraded_time=0;
	}
	else if(mode != g_controller_status.m_designate_control_mode &&g_controller_status.m_designate_control_mode==MANUAL_CONTROL)
	{
		// 新的控制模式
		control_mode = get_control_mode(g_controller_status.m_designate_control_mode);
		g_controller_status.m_is_degraded = 0;
		g_controller_status.m_degraded_time=0;
	}
	else if(control_result == -1){

		// 降级运行
		control_mode = get_backup_control_mode(mode);
		g_controller_status.m_is_degraded = 1;
	}
	else if(g_controller_status.m_is_degraded&&
		    g_controller_status.m_degraded_time>g_config.m_system_param.m_run_degraded_time)
	{
		//对于降级的，要判断外界设备是否已经恢复
		control_mode=get_current_ctrol_mode(g_controller_status.m_designate_control_mode,mode);
	}
	return control_mode;
}

/**
 * @desc 等待指定的秒数
 * @param sec 要等待的秒数
 */
static void do_wait(int sec)
{
	while(sec-->0){
		int t=10;
		while(t-->0)
			wait_for_100ms();
		g_controller_status.m_elapse_time++;
	}
}

/**
 * 主控制程序
 *
 */
int controller_main()
{
	unsigned char timer = 0;
	int control_result = 0;
	int mode_switch = 0;
	int vm_status=0;
	int detector_occupy[MAX_DETECTOR_COUNT];
	detector_counter_t detector_volume;
	control_mode_t current_control = 0;
	
	get_detector_counter(&detector_volume);
	memset(detector_occupy,0,sizeof(int)*MAX_DETECTOR_COUNT);
	
	//开机的初始状态，先黄闪再全红
	set_init_state();

	// 初始化通信
	init_command_reactor(&g_command_reactor2);
	g_command_reactor2.m_fd = open_comm(2);

	init_command_reactor(&g_command_reactor7);
	g_command_reactor7.m_fd = open_comm(7);

	init_command_reactor(&g_command_reactor8);
	g_command_reactor8.m_fd = open_comm(8);

	g_controller_status.m_timer = 0;
	g_controller_status.m_restart = 0;
	g_controller_status.m_designate_control_mode=g_config.m_control_mode_selection.m_control_mode;
	// 进入预定的控制模式
	while(!g_controller_status.m_restart){
		// 根据上次的运行结果以及其它信息判断是否要切换控制模式
		control_mode_t new_control_mode = do_schedule(control_result,current_control);
		if(new_control_mode == current_control)
		{
			mode_switch = 0;
		}
		else
		{
			mode_switch=1;
			//if(current_control==0)
			//{
			//	if(new_control_mode==all_off_control||new_control_mode==yellow_flash_control||new_control_mode==all_off_control)
			//		mode_switch = 1;
			//	else 
			//		mode_switch=0;
			//}
			//else
			//{
			//	mode_switch = 1;
			//}
			current_control = new_control_mode;
		}
		g_controller_status.m_actual_control_mode = get_mode(current_control);

		// 运行当前的控制模式
		control_result = current_control(mode_switch,timer);
		//通信
		reactor_handle_command(&g_command_reactor2);					// 通信处理 uart0 的子通道 2
		reactor_handle_command(&g_command_reactor8);					// 通信处理 cpld 通道 1
		reactor_handle_command(&g_command_reactor7);					// 通信处理 cpld 通道 1		        
		// 等待下一次运行
		wait_for_100ms();
		timer++;
		if(timer >= 10)
			timer = 0;

		g_controller_status.m_timer = timer;
		//0.1秒更新一次车流量和占有率
		get_volume_occupy(&detector_volume,detector_occupy);
		if(timer == 0){
			// 更新流逝时间
			g_controller_status.m_elapse_time++;
			// 更新剩余时间
			if(g_controller_status.m_remain_time > 0)
				g_controller_status.m_remain_time--;
			//更新强制时间
			if(g_controller_status.m_is_degraded)
				g_controller_status.m_degraded_time++;
			if(g_controller_status.m_is_config_updated){
				save_config_data();
				g_controller_status.m_is_config_updated = 0;
			}

			// 一秒钟更新一次温度
			g_controller_status.m_temperature = get_temperature();

			if(is_door_illegal_open())
				g_controller_status.m_error.m_is_door_illegal_open = 1;
			else
				g_controller_status.m_error.m_is_door_illegal_open = 0;

		}
	}
	return 0;
}
//得到车流量和占有率
int get_volume_occupy(detector_counter_t  *pdetector_volume,int detector_occupy[])
{
	static byte minute=0;
	detector_set_t set;
	detector_counter_t detector_count;
	int i=0;
	int v=0;
	detector_status_t st;

	get_phase_detector(&g_controller_status.m_current_lamp_status,&set);
	get_detector_status(&st);
	for(i=0; i<MAX_DETECTOR_COUNT; i++)
	{
		if(!is_in_set(i+1,&set))
			continue;
		v = get_bit_value(st.m_status,(i+1),1);
		if(v > 0)
		{														// 当前相位对应的检测器有输出表示正有车通过
			detector_occupy[i]++;
		}
	}
	if(minute==0)  minute=g_controller_status.m_minute;
	if(minute!=g_controller_status.m_minute)//已经经过一分钟，保存1分钟车流量和1分钟车流信息
	{
		get_detector_counter(&detector_count);
		//获取存放位置
		g_volume_occupy.m_pointer=(g_volume_occupy.m_pointer+1)%MAX_VOLUME_OCCCUPY_NUM;
		//设置时间
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_year[0]=g_controller_status.m_year[0];
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_year[1]=g_controller_status.m_year[1];
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_month=g_controller_status.m_month;
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_day=g_controller_status.m_day;
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_hour=g_controller_status.m_hour;
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_minute=minute;
		for(i=0;i<MAX_DETECTOR_COUNT;i++)
		{
			//车流量
			if(detector_count.m_counter[i]>=pdetector_volume->m_counter[i])
				g_volume_occupy.vo[g_volume_occupy.m_pointer].m_volume[i]=detector_count.m_counter[i]-pdetector_volume->m_counter[i];
			else
				g_volume_occupy.vo[g_volume_occupy.m_pointer].m_volume[i]=detector_count.m_counter[i]+255-pdetector_volume->m_counter[i];
			//占有率
			g_volume_occupy.vo[g_volume_occupy.m_pointer].m_occupy[i]=detector_occupy[i]/6;
		}
		//初始化车流量和占有率变量
		get_detector_counter(pdetector_volume);
		memset(detector_occupy,0,sizeof(int)*MAX_DETECTOR_COUNT);
		minute=g_controller_status.m_minute;
	}
	return 0;
}
//设置初始状态
//先黄闪n秒,再全红n秒
void set_init_state()
{
	//黄闪
	init_special_schedule_data(YELLOW_FLASH_CONTROL);
	// 设置新的灯状态
	set_term_status(&g_special_phase);
	do_wait(g_config.m_system_param.m_initial_yellow_flash_time);
	// 全红
	init_special_schedule_data(ALL_RED_CONTROL);
	// 设置新的灯状态
	load_current_schedule(0,PHASE_ALL_RED,TIMMING_SPECIAL_PHASE);
	g_controller_status.m_step = 1;
	g_controller_status.m_stage=1;
	g_controller_status.m_next_stage=1;
	// 更新时间
	g_controller_status.m_elapse_time = 0;
	g_controller_status.m_remain_time = 
		g_controller_status.m_assign_time = g_config.m_system_param.m_initial_all_red_time;
	// 设置新的灯状态
	set_term_status(&g_controller_status.m_current_lamp_status);
	return;
}
//设置灯的状态  state 0- 灭，1－亮 2－闪
void set_lamp_state(lamp_status_t *phase,int termial,int state)
{
	//int u;
	//u = get_bit_value(g_config.m_lamp_setting.m_use,termial,1);
	//if(u==0) //该信号灯不启用，不设置他的状态（亮，灭，闪）
	//	return;
	if(termial> 0)
	{
		// 将该端子输出设为亮
		int p = (termial-1)/4;		// 端子所在的字节
		int offset = (termial-1)%4;		// 字节里面的偏移0-3
		byte* b = &phase->m_output[p];
		byte on = state << 2*(3 - offset);
//		*b &= ~(0x03 << 2*(3 - offset)); //先清零
		*b |= on;			// 后赋值
	}
}
//判断灯组的设置状态，是否要工作在长红，长绿，长黄状态哦 
int judge_lamp_group_work_mode(int nRed,int nYellow,int nGreen)
{
	if(nRed==0&&nGreen==0&&nYellow)//只有黄灯存在
		return YELLOW;//长时间亮黄闪
	else if(nYellow==0&&nRed&&nGreen==0)//只有红灯存在
		return RED;//长时间亮红灯
	else if(nGreen&&nRed==0&&nYellow==0)//只有绿灯存在
		return GREEN;
	else 
		return -1;
}
//设置新的相位
int set_phase_state(lamp_status_t* phase)
{
	int i=0;
	int lamp_group=0;
	int m_lamp_group[MAX_LAMP_GROUP_COUNT];
	memset(&m_lamp_group,0,sizeof(int)*MAX_LAMP_GROUP_COUNT);
	for(i=0;i<MAX_PHASE_COUNT;i++)
	{
		lamp_group=g_controller_status.m_current_phase_plan_p->m_stage[g_controller_status.m_stage-1].m_phase[i];
		if(lamp_group<=g_config.m_lamp_group_setting.m_lamp_group_count&&lamp_group>0)//当前灯组大于总组数
			m_lamp_group[lamp_group-1]=1;		
	}
	for(i=0;i<g_config.m_lamp_group_setting.m_lamp_group_count;i++)
	{
		int nWorkMode=judge_lamp_group_work_mode(g_config.m_lamp_group_setting.m_lamp_group[i][RED],
						g_config.m_lamp_group_setting.m_lamp_group[i][YELLOW],
						g_config.m_lamp_group_setting.m_lamp_group[i][GREEN]);
		if(nWorkMode==RED)
		{
			set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][RED],ON);
			continue;
		}
		else if(nWorkMode==YELLOW)
		{
			set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][YELLOW],FLASH);
			continue;
		}
		else if(nWorkMode==GREEN)
		{
			set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][GREEN],ON);
			continue;
		}
		if(m_lamp_group[i])
		{
			if(g_controller_status.m_step==1)//第一步，亮绿灯
			{
				set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][GREEN],ON);
			}
			else
			{
				if(find_lamp_group(i+1))//下个阶段继续亮绿灯
				{
					set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][GREEN],ON);
				}
				else //下个阶段亮红灯
				{
					if(g_controller_status.m_step==2)//第二步，亮绿闪
					{
						set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][GREEN],FLASH);
					}
					else if(g_controller_status.m_step==3)//第三步，亮黄灯
					{
						if(g_config.m_lamp_group_setting.m_lamp_group[i][YELLOW])//黄灯存在
							set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][YELLOW],ON);
						else
							set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][RED],ON);					
					}
					else if(g_controller_status.m_step==4)//第四步，亮红灯
					{
						set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][RED],ON);					
					}
				}

			}
		}
		else
			set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][RED],ON);
	}
	return 0;
}
void set_phase_time()
{
	if(g_controller_status.m_step==1)
	{
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = 
			g_controller_status.m_current_timming_plan_p->m_greentimming[g_controller_status.m_stage-1];
	}
	else if(g_controller_status.m_step==2)
	{
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = 
			g_controller_status.m_current_timming_plan_p->m_greenflashtimming[g_controller_status.m_stage-1];
	}
	else if(g_controller_status.m_step==3)
	{
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = 
			g_controller_status.m_current_timming_plan_p->m_yellowtimming[g_controller_status.m_stage-1];
	}
	else if(g_controller_status.m_step==4)
	{
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = 
			g_controller_status.m_current_timming_plan_p->m_redtimming[g_controller_status.m_stage-1];
	}
}
//查找当前灯组在下一个阶段中是否亮绿灯
int find_lamp_group(int lamp_group)
{
	int i=0;
	int m_stage=0;
	////判断下一阶段是那个阶段
	//if(g_controller_status.m_stage==g_controller_status.m_current_phase_plan_p->m_stage_count)
	//	m_stage=1;
	//else
	//	m_stage=g_controller_status.m_stage+1;
	m_stage=g_controller_status.m_next_stage;
	if(m_stage==1)//最后阶段
	{
		//查找当前灯组是否在下一阶段中
		for(i=0;i<MAX_PHASE_COUNT;i++)
		{
			if(g_controller_status.m_next_phase_plan_p->m_stage[0].m_phase[i]==lamp_group)
				return 1;
		}
	}
	else
	{
		//查找当前灯组是否在下一阶段中
		for(i=0;i<MAX_PHASE_COUNT;i++)
		{
			if(g_controller_status.m_current_phase_plan_p->m_stage[m_stage-1].m_phase[i]==lamp_group)
				return 1;
		}
	}
	return 0;
}

