/**
 * 信号机控制程序的主程序
 *
 * Company: Xiamen Uni-Creative Information Technology Corporation
 * Author: Lin Jingxian(林景贤)
 * E-Mail: cckk_lin@yahoo.ie
 * Date:  2005-06-28 09:40:03(中国标准时间) 
 *
 * $log$
 */
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include <string.h>
#include "io.h"
#include "kernel.h"
#include "datastruct.h"
void validateconfigparam()
{
	int i=0;
	int j=0;
	int k=0;
	//验证感应参数
	if(g_config.m_actuate_control_param.m_minmum_green<3)
		g_config.m_actuate_control_param.m_minmum_green=3;
	if(g_config.m_actuate_control_param.m_maximum_green<g_config.m_actuate_control_param.m_minmum_green)
		g_config.m_actuate_control_param.m_maximum_green = g_config.m_actuate_control_param.m_minmum_green+20;
	if(g_config.m_actuate_control_param.m_adjust_quantum<2)
		g_config.m_actuate_control_param.m_adjust_quantum=2;
	if(g_config.m_actuate_control_param.m_strategy!=1&&g_config.m_actuate_control_param.m_strategy!=2)
	{
		g_config.m_actuate_control_param.m_strategy=2;
	}
	//无线协调控制模式
	if (g_config.m_cordless_coordinate_control_param.m_phase>MAX_PHASE_PLAN_COUNT||g_config.m_cordless_coordinate_control_param.m_phase<=0) {
		g_config.m_cordless_coordinate_control_param.m_phase=1;
	}
	if (g_config.m_cordless_coordinate_control_param.m_timming>MAX_TIMMING_PLAN_COUNT||g_config.m_cordless_coordinate_control_param.m_timming<=0) {
		g_config.m_cordless_coordinate_control_param.m_timming=1;
	}
	//多时段
	if(g_config.m_multiple_schedule_control_param.m_schedule_plan>MAX_SCHEDULE_PLAN_COUNT||g_config.m_multiple_schedule_control_param.m_schedule_plan<=0)
		g_config.m_multiple_schedule_control_param.m_schedule_plan=1;
	//控制模式
	if(g_config.m_control_mode_selection.m_control_mode>9||g_config.m_control_mode_selection.m_control_mode<1)
		g_config.m_control_mode_selection.m_control_mode=MULTIPLE_SCHEDULE_CONTROL;
	//系统参数
	if(g_config.m_system_param.m_initial_all_red_time<=0||g_config.m_system_param.m_initial_all_red_time>99)
		g_config.m_system_param.m_initial_all_red_time=4;
	if(g_config.m_system_param.m_initial_yellow_flash_time<=0||g_config.m_system_param.m_initial_yellow_flash_time>99)
		g_config.m_system_param.m_initial_yellow_flash_time=4;
	if(g_config.m_system_param.m_run_degraded_time>255||g_config.m_system_param.m_run_degraded_time<=0)
		g_config.m_system_param.m_run_degraded_time=10;
	//相位表
	for(i=0;i<MAX_PHASE_PLAN_COUNT;i++)
	{
		if(g_config.m_phase_table[i].m_stage_count>MAX_STAGE_COUNT)
			g_config.m_phase_table[i].m_stage_count=MAX_STAGE_COUNT;
		else if(g_config.m_phase_table[i].m_stage_count<0)
			g_config.m_phase_table[i].m_stage_count=0;
		for (j=0;j<MAX_STAGE_COUNT;j++)
		{
			for(k=0;k<MAX_PHASE_COUNT;k++)
			{
				if(g_config.m_phase_table[i].m_stage[j].m_phase[k]>MAX_LAMP_GROUP_COUNT||g_config.m_phase_table[i].m_stage[j].m_phase[k]<0)
					g_config.m_phase_table[i].m_stage[j].m_phase[k]=0;
			}
		}
	}
	//配时表
	for(i=0;i<MAX_TIMMING_PLAN_COUNT;i++)
	{
		if(g_config.m_timming_table[i].m_step>MAX_STAGE_COUNT)
			g_config.m_timming_table[i].m_step=MAX_STAGE_COUNT;
		else if(g_config.m_timming_table[i].m_step<0)
			g_config.m_timming_table[i].m_step=0;
		for(j=0;j<MAX_STAGE_COUNT;j++)
		{
			if(g_config.m_timming_table[i].m_greenflashtimming[j]>255||g_config.m_timming_table[i].m_greenflashtimming[j]<0)
				g_config.m_timming_table[i].m_greenflashtimming[j]=0;
			if(g_config.m_timming_table[i].m_greentimming[j]>255||g_config.m_timming_table[i].m_greentimming[j]<0)
				g_config.m_timming_table[i].m_greentimming[j]=0;
			if(g_config.m_timming_table[i].m_yellowtimming[j]>255||g_config.m_timming_table[i].m_yellowtimming[j]<0)
				g_config.m_timming_table[i].m_yellowtimming[j]=0;
			if(g_config.m_timming_table[i].m_redtimming[j]>255||g_config.m_timming_table[i].m_redtimming[j]<0)
				g_config.m_timming_table[i].m_redtimming[j]=0;
		}
	}
	//时段表
	for (i=0;i<MAX_SCHEDULE_PLAN_COUNT;i++) 
	{
		for(j=0;j<MAX_TIME_SEGMENT_COUNT;j++)
		{
			if(/*g_config.m_schedule_table[i].m_phase[j]>MAX_PHASE_PLAN_COUNT||*/g_config.m_schedule_table[i].m_phase[j]<0)
				g_config.m_schedule_table[i].m_phase[j]=0;
			if(/*g_config.m_schedule_table[i].m_timming[j]>MAX_TIMMING_PLAN_COUNT||*/g_config.m_schedule_table[i].m_timming[j]<0)
				g_config.m_schedule_table[i].m_timming[j]=0;
		}
	}
	//特殊日
	for (i=0;i<MAX_SPECIAL_DAY_COUNT;i++)
	{
		if(g_config.m_special_day_table.m_day[i].m_schedule>MAX_SCHEDULE_PLAN_COUNT||g_config.m_special_day_table.m_day[i].m_schedule<=0)
			g_config.m_special_day_table.m_day[i].m_schedule=1;
	}
	//信号灯设置
//	for(i=0;i<MAX_TERMINAL_COUNT/8;i++)
//		g_config.m_lamp_setting.m_use[i]=0;
	//灯组设置
	if(g_config.m_lamp_group_setting.m_lamp_group_count>MAX_LAMP_GROUP_COUNT||g_config.m_lamp_group_setting.m_lamp_group_count<0)
		g_config.m_lamp_group_setting.m_lamp_group_count=0;
	for(i=0;i<MAX_LAMP_GROUP_COUNT;i++)
	{
		for (j=0;j<3;j++)
		{
			if(g_config.m_lamp_group_setting.m_lamp_group[i][j]>MAX_TERMINAL_COUNT||g_config.m_lamp_group_setting.m_lamp_group[i][j]<0)
				g_config.m_lamp_group_setting.m_lamp_group[i][j]=0;
		}
	}
	//检测器参数
	for (i=0;i<MAX_DETECTOR_COUNT;i++)
	{
		if(g_config.m_detector_setting.m_lamp_group[i]>MAX_LAMP_GROUP_COUNT||g_config.m_detector_setting.m_lamp_group[i]<0)
			g_config.m_detector_setting.m_lamp_group[i]=0;
	}
	//初始化车流量
	memset(&g_volume_occupy,0,sizeof(volume_occupy_group_t));
	//设置系统启动的运行模式
	g_controller_status.m_designate_control_mode=g_config.m_control_mode_selection.m_control_mode;

}
void main(void)
{	
	//初始化
	initialize();
	//参数的有效性
	validateconfigparam();
	//程序主流程
	while(1)	
	{
		controller_main();	
	}
}
