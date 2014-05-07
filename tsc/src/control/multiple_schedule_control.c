/**
 *  ��ο���
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
 *  ��ο����㷨
 *
 */
int multiple_schedule_control(int mode_switch, int timer)
{
	//����Ǹ��л����ÿ���ģʽ
	if(mode_switch){
		s_is_mode_switching = 1;
	}

	// ����Ǵ��ڹ���״̬
	if(s_is_mode_switching){
		// ���й����㷨
		int ret = switching_with_multiple_schedule(mode_switch,timer);
		if(ret == 1){
			// ���ɽ���
			s_is_mode_switching = 0;
			return 0;
		}
	}
	else  if(timer == 0)
	{
		if(is_step_end() && is_last_step()){
			byte s,p,t;
			// һ�������Ѿ��������
			// ������Ҫ�����µ�ʱ��
			get_current_schedule(&s,&p,&t);
			// ��鵱ǰ��λ����ʱ�Ƿ�Ϸ�
			if(!is_schedule_valid(p,t)){
				// ʱ�����ݲ��Ϸ�
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
				// ����ʱ��
				g_controller_status.m_elapse_time = 0;
				// ȷ���²�����ʱ
				set_phase_time();
				// �����µĵ�״̬
				set_term_status(&g_controller_status.m_current_lamp_status);
			}
			else
				run_current_step();
		}
		// ������ǰ���������
		else if(is_step_end()){
			if(is_last_stage_first_step())
			{
				byte s,p,t;
				// ������Ҫ�����µ�ʱ��
				byte times;
				times=g_controller_status.m_current_timming_plan_p->m_greenflashtimming[g_controller_status.m_stage]+
					  g_controller_status.m_current_timming_plan_p->m_yellowtimming[g_controller_status.m_stage]+
					  g_controller_status.m_current_timming_plan_p->m_redtimming[g_controller_status.m_stage];
				get_next_schedule(&s,&p,&t,times);
				g_controller_status.m_next_phase_plan_p=&g_config.m_phase_table[p-1];
			}
			// ���²���
			set_current_stage();
			if((g_controller_status.m_phase!= PHASE_YELLOW_FLASH) && (g_controller_status.m_phase != PHASE_ALL_RED)
				&& (g_controller_status.m_phase != PHASE_ALL_OFF))
				run_current_step();
		}
	}
	if(timer == 9){
		// ���ϵͳ�Ĵ���״̬
		// ��������²����������б�����ģʽ����Ҫ�󽵼�
		// �ж��źŵ��Ƿ��й��ϣ��й��ϣ��򽵼�
		if(!is_detect_health_flag())//Ҫ����
		{
			if(is_lamp_state()==-1)
				return -1;
		}
	}
	// ������������
	return 0;

}
