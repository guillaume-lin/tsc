/**
 *  多段控制
 *
 *  Company: Xiamen Uni-Creative Information Technology Corporation
 *  Author:  Lin Jingxian
 *  E-Mail:  cckk_lin@yahoo.ie
 *  Date:    2005-06-30 
 * 
 *  $log$
 *
 */
#include "kernel.h"
#include "error_code.h"
#include "multiple_schedule_control.h"
#include "io.h"
#include "util.h"


static int s_is_mode_switching;
/**
 *  多段控制算法
 *
 */
int multiple_schedule_control(int mode_switch, int timer)
{
	//如果是刚切换到该控制模式
	if(mode_switch){
		s_is_mode_switching = 1;
	}

	// 如果是处于过渡状态
	if(s_is_mode_switching){
		// 运行过渡算法
		int ret = switching_with_multiple_schedule(mode_switch,timer);
		if(ret == 1){
			// 过渡结束
			s_is_mode_switching = 0;
			return 0;
		}
	}
	else  if(timer == 0)
	{
		if(is_step_end() && is_last_step()){
			byte s,p,t;
			// 一个周期已经运行完毕
			// 根据需要运行新的时段
			get_current_schedule(&s,&p,&t);
			// 检查当前相位和配时是否合法
			if(!is_schedule_valid(p,t)){
				// 时段数据不合法
				set_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
				set_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
						
				return -1;
			}
			else {
				clear_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
				clear_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
				} 

			load_current_schedule(s,p,t);
			g_controller_status.m_step=1;
			g_controller_status.m_stage = 1;
			if(g_controller_status.m_current_phase_plan_p->m_stage_count>1)
				g_controller_status.m_next_stage=2;
			else
				g_controller_status.m_next_stage=1;
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
				run_current_step();
		}
		// 仅仅当前步运行完毕
		else if(is_step_end()){
			if(is_last_stage_first_step())
			{
				byte s,p,t;
				// 根据需要运行新的时段
				byte times;
				times=g_controller_status.m_current_timming_plan_p->m_greenflashtimming[g_controller_status.m_stage]+
					  g_controller_status.m_current_timming_plan_p->m_yellowtimming[g_controller_status.m_stage]+
					  g_controller_status.m_current_timming_plan_p->m_redtimming[g_controller_status.m_stage];
				get_next_schedule(&s,&p,&t,times);
				g_controller_status.m_next_phase_plan_p=&g_config.m_phase_table[p-1];
			}
			// 更新步号
			set_current_stage();
			if((g_controller_status.m_phase!= PHASE_YELLOW_FLASH) && (g_controller_status.m_phase != PHASE_ALL_RED)
				&& (g_controller_status.m_phase != PHASE_ALL_OFF))
				run_current_step();
		}
	}
	if(timer == 9){
		// 检查系统的错误状态
		// 如果错误导致不能正常运行本控制模式，则要求降级
		// 判断信号灯是否有故障，有故障，则降级
		if(!is_detect_health_flag())//要求检测
		{
			if(is_lamp_state()==-1)
				return -1;
		}
	}
	// 否则正常返回
	return 0;

}
