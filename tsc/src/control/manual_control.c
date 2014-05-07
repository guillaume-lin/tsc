/**
 * manual_control.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-02 09:38:34 中国标准时间
 * 
 * $log$
 *
 */
#include "kernel.h"
#include "io.h"
#include "manual_control.h"
/**
 *  手动控制
 *
 */
void manual_load_multiple_schedule()
{
	unsigned char s,p,t;
	get_current_schedule(&s,&p,&t);
	// 检查当前相位和配时是否合法
	if(is_schedule_valid(p,t))									// 时段数据合法
	{
		load_current_schedule(s,p,t);							// 当前相位和配时合法则载入，否则不载
		g_controller_status.m_stage=1;
		if(g_controller_status.m_current_phase_plan_p->m_stage_count==1)
			g_controller_status.m_next_stage=1;
		else
			g_controller_status.m_next_stage=2;
		g_controller_status.m_step = 1;							// 从多时段相位表的第一步开始运行
		run_current_step();
		g_controller_status.m_remain_time = g_controller_status.m_assign_time = 0;

	}
	else  // 时段数据不合法，则运行黄闪
	{														
		load_current_schedule(0,PHASE_YELLOW_FLASH,TIMMING_SPECIAL_PHASE);
		g_controller_status.m_stage=1;
		g_controller_status.m_step = 1;
		g_controller_status.m_next_stage=1;
		run_current_step();
	}
}
//按下步进键以后的处理
void manual_step_dispose()
{
	if(g_controller_status.m_current_phase_plan_p->m_stage_count==1)
	{
		clear_step_forward_button_state();
		return;
	}
	if(is_step_end())
	{
		set_current_stage();
		run_current_step();
		if(g_controller_status.m_step==1)//当前相位完成
		{
			g_controller_status.m_remain_time = g_controller_status.m_assign_time =0;
			clear_step_forward_button_state();
			g_controller_status.m_step_forward_button_state=0;
		}
	}
}
void manual_change_step_dispose()
{
	if(g_controller_status.m_current_phase_plan_p->m_stage_count==1)
	{
		clear_step_forward_button_state();
		return;
	}
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
			g_controller_status.m_need_change_stage_flag=0;
			clear_need_change_stage_flag();
		}
		run_current_step();
		if(g_controller_status.m_step==1)
			g_controller_status.m_remain_time = g_controller_status.m_assign_time = 0;
	}
}
int manual_control(int mode_switch,int timer)
{
	static unsigned char time_count = 0x00; 
	int  need_change_to_stage_count=0;
	if(mode_switch)
	{
		if(g_controller_status.m_step!=1)
		{
			g_controller_status.m_step_forward_button_state = 1;
		}
	}
	//if (g_controller_status.m_current_phase_plan_p->m_stage_count == 1) 	// 手动控制时若相位表总步数为 1，则调用多时段相位表
	//{
	//	manual_load_multiple_schedule();
	//	clear_step_forward_button_state();
	//	return 0;
	//}
	if(g_controller_status.m_need_change_stage_flag)//在执行改变状态的过程中，步进键按键无效
	{
		clear_step_forward_button_state();			
		clear_need_change_stage_flag();
	}
	else
	{
		if(is_step_forward_button_pressed()) {								// 有步进键按下
			g_controller_status.m_step_forward_button_state = 1;
		}
	}
	if(g_controller_status.m_step_forward_button_state)//按下步进键
	{
		manual_step_dispose();
		clear_need_change_stage_flag();//在执行步进过程中，按下转换相位按钮无效
		return 0;
	}
	if (is_need_change_stage())
	{
		g_controller_status.m_need_change_stage_flag=1;	
		need_change_to_stage_count=get_manual_next_stage_count();
		if(need_change_to_stage_count>g_controller_status.m_current_phase_plan_p->m_stage_count)
			need_change_to_stage_count=need_change_to_stage_count-g_controller_status.m_current_phase_plan_p->m_stage_count;
		g_controller_status.m_next_stage=need_change_to_stage_count;
	}
	if(g_controller_status.m_need_change_stage_flag)
	{
		manual_change_step_dispose();
		clear_step_forward_button_state();//在执行转换相位过程中，按下转换步进按钮无效
		return 0;
	}
	return 0;
}
