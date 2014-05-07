/**
 * exective_machine_control.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-14 14:58:10 中国标准时间
 * 
 * $log$
 *
 */
#include "io.h"
#include "datastruct.h"
#include "kernel.h"
#include "error_code.h"
#include "executive_machine_control.h"

/**
 *  在指定的时间内没有收到命令，信号机将退出该状态
 */
static int active_guard_timer;

//进入联机模式，引导联机要求的相位和配时
int online_get_schedule(byte p,byte t)
{
	// 检查当前相位和配时是否合法
	if(!is_schedule_valid(p,t))
	{
		// 时段数据不合法
		set_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
		set_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
		return -1;
	}
	else
	{
		clear_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
		clear_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
	} 
	load_current_schedule(0,p,t);
	g_controller_status.m_step=1;
	g_controller_status.m_stage = 1;
	if(g_controller_status.m_current_phase_plan_p->m_stage_count>1)
		g_controller_status.m_next_stage=2;
	else
		g_controller_status.m_next_stage=1;
	g_controller_status.m_assign_time = 0;
	g_controller_status.m_remain_time = 0;
	if ((p == PHASE_YELLOW_FLASH) || (p == PHASE_ALL_RED) || (p == PHASE_ALL_OFF))
	{
		// 更新时间
		g_controller_status.m_elapse_time = 0;
		// 确定新步的配时
		set_phase_time();
		// 设置新的灯状态
		set_term_status(&g_controller_status.m_current_lamp_status);
	}
	else
	{
		run_current_step();
		g_controller_status.m_assign_time =g_controller_status.m_remain_time= 0;
	}
	return 1;
}
int online_change_step_dispose()
{
	if(g_controller_status.m_online_command.m_param1==g_controller_status.m_stage)
		return 1;
	//要求转换到特殊阶段
	if(g_controller_status.m_online_command.m_param1==PHASE_YELLOW_FLASH||g_controller_status.m_online_command.m_param1==PHASE_ALL_RED||g_controller_status.m_online_command.m_param1==PHASE_ALL_OFF)
	{
		g_controller_status.m_stage=g_controller_status.m_online_command.m_param1;
		//引导特定阶段
		if(g_controller_status.m_stage==PHASE_YELLOW_FLASH)
			init_special_schedule_data(YELLOW_FLASH_CONTROL);
		else if(g_controller_status.m_stage==PHASE_ALL_RED)
			init_special_schedule_data(ALL_RED_CONTROL);
		else if(g_controller_status.m_stage==PHASE_ALL_OFF)
			init_special_schedule_data(ALL_OFF_CONTROL);
		g_controller_status.m_current_lamp_status=g_special_phase;
		//强制灯色
		set_term_status(&g_special_phase);
		g_controller_status.m_remain_time = g_controller_status.m_assign_time =g_controller_status.m_elapse_time=0;
		return 1;
	}
	//现在阶段是特殊阶段
	if(g_controller_status.m_stage==PHASE_YELLOW_FLASH||g_controller_status.m_stage==PHASE_ALL_OFF||g_controller_status.m_stage==PHASE_ALL_RED)
	{
		if(g_controller_status.m_current_phase_plan_p->m_stage_count==1)
		{
			//本身是特殊的相位
			g_controller_status.m_stage=1;
			g_controller_status.m_step=1;
			g_controller_status.m_next_stage=1;
			if(g_controller_status.m_phase==PHASE_YELLOW_FLASH)
				init_special_schedule_data(YELLOW_FLASH_CONTROL);
			else if(g_controller_status.m_phase==PHASE_ALL_RED)
				init_special_schedule_data(ALL_RED_CONTROL);
			else if(g_controller_status.m_phase==PHASE_ALL_OFF)
				init_special_schedule_data(ALL_OFF_CONTROL);
			g_controller_status.m_current_lamp_status=g_special_phase;
			set_term_status(&g_controller_status.m_current_lamp_status);
			g_controller_status.m_remain_time = g_controller_status.m_assign_time =g_controller_status.m_elapse_time=0;
			return 1;
		}
		if(g_controller_status.m_online_command.m_param1>g_controller_status.m_current_phase_plan_p->m_stage_count)
		{
			//错误的阶段号
			return 1;
		}
		//在特定阶段中接收到跳转到下一阶段指令，则跳转到第一阶段
		if(g_controller_status.m_online_command.m_param1==0)
			g_controller_status.m_online_command.m_param1=1;
		g_controller_status.m_stage=g_controller_status.m_online_command.m_param1;
		if(g_controller_status.m_current_phase_plan_p->m_stage_count>g_controller_status.m_stage)
			g_controller_status.m_next_stage++;
		else
			g_controller_status.m_next_stage=1;
		g_controller_status.m_step=1;
		run_current_step();
		g_controller_status.m_remain_time = g_controller_status.m_assign_time =g_controller_status.m_elapse_time=0;
		return 1;
	}
	//一般阶段之间的转换
	if(g_controller_status.m_online_command.m_param1>g_controller_status.m_current_phase_plan_p->m_stage_count||g_controller_status.m_current_phase_plan_p->m_stage_count==1)
	{
		//错误的阶段号
		return 1;
	}
	//设置下一阶段号
	if(g_controller_status.m_online_command.m_param1>0)
	{
		g_controller_status.m_next_stage=g_controller_status.m_online_command.m_param1;
	}
	return online_run_over_stage();
}
int online_stage_forward_dispose()
{
	if(g_controller_status.m_current_phase_plan_p->m_stage_count==1)
	{
		//本身是特殊的相位
		g_controller_status.m_stage=1;
		g_controller_status.m_step=1;
		g_controller_status.m_next_stage=1;
		if(g_controller_status.m_phase==PHASE_YELLOW_FLASH)
			init_special_schedule_data(YELLOW_FLASH_CONTROL);
		else if(g_controller_status.m_phase==PHASE_ALL_RED)
			init_special_schedule_data(ALL_RED_CONTROL);
		else if(g_controller_status.m_phase==PHASE_ALL_OFF)
			init_special_schedule_data(ALL_OFF_CONTROL);
		g_controller_status.m_current_lamp_status=g_special_phase;
		set_term_status(&g_controller_status.m_current_lamp_status);
		g_controller_status.m_remain_time = g_controller_status.m_assign_time = 0;
		return 1;
	}
	//现在阶段是特殊阶段
	if(g_controller_status.m_stage==PHASE_YELLOW_FLASH||g_controller_status.m_stage==PHASE_ALL_RED||g_controller_status.m_stage==PHASE_ALL_OFF)
	{
		//在特定阶段中接收到跳转到下一阶段指令，则跳转到第一阶段
		g_controller_status.m_stage=1;
		if(g_controller_status.m_current_phase_plan_p->m_stage_count>g_controller_status.m_stage)
			g_controller_status.m_next_stage++;
		else
			g_controller_status.m_next_stage=1;
		g_controller_status.m_step=1;
		run_current_step();
		g_controller_status.m_remain_time = g_controller_status.m_assign_time = 0;
		return 1;
	}
	return online_run_over_stage();
}
//把当前阶段的剩余步走完
int online_run_over_stage()
{
	int findish=0;
	if(is_step_end())
	{
		if(g_controller_status.m_step<4)
		{
			g_controller_status.m_step++;
		}
		else
		{
			g_controller_status.m_stage=g_controller_status.m_next_stage;
			if(g_controller_status.m_current_phase_plan_p->m_stage_count>g_controller_status.m_stage)
				g_controller_status.m_next_stage++;
			else
				g_controller_status.m_next_stage=1;
			g_controller_status.m_step=1;
			findish=1;
		}
		run_current_step();
		if(g_controller_status.m_step==1)
		{
			g_controller_status.m_remain_time = g_controller_status.m_assign_time = 0;
		}
	}
	return findish;
}
int online_command_dispose(int switch_mode,int timer)
{
	if(g_controller_status.m_online_command.m_param1==0)//保持原来的相位
	{
		if(g_controller_status.m_online_command.m_param2!=0)//配时已经发生变化
		{
			// 检查当前相位和配时是否合法
			if(!is_schedule_valid(g_controller_status.m_phase,g_controller_status.m_online_command.m_param2))
			{
				// 时段数据不合法
				set_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
				set_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
				return -1;
			}
			else
			{
				clear_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
				clear_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
			} 
			load_current_schedule(0,g_controller_status.m_phase,g_controller_status.m_online_command.m_param2);
			g_controller_status.m_online_command.m_param2=0;
		}
		if(g_controller_status.m_step==1)
		{
			g_controller_status.m_elapse_time=g_controller_status.m_remain_time=g_controller_status.m_actual_control_mode=0;
			return 1;
		}
		return online_run_over_stage();
	}
	else  if(switching_with_yellow_flash_all_red(switch_mode,2,2,timer))//转换完成，进行设置
	{
		return online_get_schedule(g_controller_status.m_online_command.m_param1,g_controller_status.m_online_command.m_param2);
	}
	return 0;
}
int executive_machine_control(int switch_mode,int timer)
{
	int nRet=0;
	if(switch_mode){
		if(g_controller_status.m_online_command.m_cmd!=ONLINE)
			return -1;
	}
	if(g_controller_status.m_online_command.m_cmd==ONLINE)
	{
		g_controller_status.m_bNoRevCommand=1;//不接收中心的控制指令
		active_guard_timer = 3;
		nRet=online_command_dispose(switch_mode,timer);
		if(nRet!=0)
		{
			g_controller_status.m_online_command.m_cmd=0;
			g_controller_status.m_bNoRevCommand=0;
		}
		if(nRet==1) nRet=0;
		return nRet;
	}
	if(timer!=0) 	return 0;
	if(g_controller_status.m_online_command.m_cmd==STAGE_JUMP)
	{
		active_guard_timer = 3;
		g_controller_status.m_bNoRevCommand=1;
		if(online_change_step_dispose()==1)
		{
			g_controller_status.m_online_command.m_cmd=0;
			g_controller_status.m_bNoRevCommand=0;
		}
		return 0;
	}
	if(g_controller_status.m_online_command.m_cmd==OFFLINE)
	{
		//脱机,回到默认控制模式（启动后进入的控制模式）
		g_controller_status.m_designate_control_mode=g_config.m_control_mode_selection.m_control_mode;
		g_controller_status.m_online_command.m_cmd=0;
		return 0;
	}
	if(g_controller_status.m_online_command.m_cmd==STAGE_FORWARD)
	{
		active_guard_timer = 3;
		g_controller_status.m_bNoRevCommand=1;
		if(online_stage_forward_dispose()==1)
		{
			g_controller_status.m_online_command.m_cmd=0;
			g_controller_status.m_bNoRevCommand=0;
		}
	}
	if(g_controller_status.m_online_command .m_cmd== DIRECT_PHASE){
		// 设置强制灯色
		set_term_status(&g_controller_status.m_direct_phase);
		g_controller_status.m_elapse_time = 0;
		g_controller_status.m_online_command.m_cmd= 0;
		active_guard_timer = 3;
	}
	else if(g_controller_status.m_online_command.m_cmd == STAGE_KEEP){
		// 保持
		active_guard_timer = 3;
		g_controller_status.m_online_command.m_cmd = 0;
	}

	if(timer == 0){
		active_guard_timer--;
		if(active_guard_timer <= 0){
			// 3秒钟之类没有保持指令
			// 放弃控制
			return -1;
		}
	}

	return 0;
}
