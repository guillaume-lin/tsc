/**
 * actuate_control.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-14 15:26:10 �й���׼ʱ��
 * 
 * $log$
 *
 */
#include "kernel.h"
#include "io.h"
#include "util.h"
#include "detector_set.h"
#include "pedestrian_control.h"
#include "error_code.h"
#ifndef __C51__
typedef byte bit;
#endif
/**
 * �жϵ�ǰ���ǲ�����λ��
 */
int is_phase_step()
{
	if(g_controller_status.m_step==1)//�׶εĵ�һ��Ϊ��λ��
		return 1;
	return 0;
}
// �������
//detector_set_t set;

// 
// �򵥵�ȫ��Ӧ����
// ����Ӧ����λһ����С�̣���������г��Ļ�����������λʱ�䣬һֱ�������
//
int pedestrian_control(int mode_switch,int timer)
{
	static byte min_green;
	static byte max_green;
	static byte adjustment;
	static byte is_in_phase_step;
	static detector_set_t set;
	static byte phase;
	static byte timming;
	
	if(mode_switch)//ģʽת����ʱ�򣬳�ʼ������
	{
		byte ps,ts;

		phase = g_config.m_pedestrian_control_param.m_phase;
		timming = g_config.m_pedestrian_control_param.m_timming;
		ps = g_config.m_phase_table[phase-1].m_step;                                                                                      
		ts = g_config.m_timming_table[timming-1].m_step;                                                                                  
		if(ps <= 0 || ts <= 0 || ps != ts) return -1;				// �����õ�����λ��������ʱ�����ŵ��ܲ�����һ�»�Ϊ 0 ʱ����ʾ���Ϸ���ֱ�ӷ���

		if(g_config.m_pedestrian_setting.m_minmum_green<3)
			min_green=3;
		else
			min_green = g_config.m_pedestrian_control_param.m_minmum_green;
		if(g_config.m_pedestrian_control_param.m_maximum_green>min_green)
			max_green = g_config.m_pedestrian_control_param.m_maximum_green;
		else
			max_green = min_green+20;
		adjustment = g_config.m_pedestrian_control_param.m_adjust_quantum;
		if(adjustment<2) adjustment=2;
		is_in_phase_step = 0;
		empty_set(&set);
	}

	if(timer == 0&&is_step_end())
	{
		if(is_last_step())
		{
			byte s,p,t;
			// һ�������Ѿ��������
			//�ض���Ӧ����
			if(g_config.m_actuate_control_param.m_minmum_green<3)
				min_green=3;
			else
				min_green = g_config.m_actuate_control_param.m_minmum_green;
			if(g_config.m_actuate_control_param.m_maximum_green>min_green)
				max_green = g_config.m_actuate_control_param.m_maximum_green;
			else
				max_green = min_green+20;
			adjustment = g_config.m_actuate_control_param.m_adjust_quantum;
			if(adjustment<2) adjustment=2;
			vehicle_gap = g_config.m_actuate_control_param.m_vehicle_gap;
			// ������Ҫ�����µ�ʱ��
			get_current_schedule(&s,&p,&t);
			// ��鵱ǰ��λ����ʱ�Ƿ�Ϸ�
			if(!is_schedule_valid(p,t))
			{
				// ʱ�����ݲ��Ϸ�
				set_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
				set_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
				return -1;
			}
			else
			{
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
		}
		else
		{
			// ���²���
			set_current_stage();
		}
		if ((g_controller_status.m_phase == PHASE_YELLOW_FLASH) || (g_controller_status.m_phase == PHASE_ALL_RED) || (g_controller_status.m_phase == PHASE_ALL_OFF))
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
		if(is_phase_step())//����λ��
		{
			no_car_arrive_time = 0;
			is_in_phase_step = 1;
			// ����λ��, ��Ҫ�������������ʼ��ʱ
			// ���ݴ�ǰ����ĸ���λ����ı��賵����������һ����ʼ�̵�ʱ��
			g_controller_status.m_remain_time = 
				g_controller_status.m_assign_time = min_green;
			// ��ȡ��λ��ؼ����
			empty_set(&set);
			get_phase_detector(&g_controller_status.m_current_lamp_status,&set);

		}
		else
			is_in_phase_step = 0;
		is_actuate_control = 0;
	}
	if(timer == 5 && is_in_phase_step)
	{
		if(!is_detect_health_flag())//Ҫ����
		{
			// �ж��Ƿ��м��������
			if(is_detect_state(&set)==-1) 
				return -1;
			// �ж��Ƿ����źŵ��Ƿ��й���
			if(is_lamp_state()==-1)
				return -1;
		}
	}

	if(timer == 8 && is_in_phase_step)//��鵱ǰ�Ƿ��г�ͨ��
	{
		int i;
		// �������״̬
		// ֻ�����λ��ؼ����
		detector_status_t st;
		get_detector_status(&st);
		no_car_arrive_time++;

		for(i=0; i<MAX_DETECTOR_COUNT; i++)
		{
			int v;
			if(!is_in_set(i+1,&set))
				continue;
			is_actuate_control = 1;											// ����λ���ж�Ӧ�ļ����
			//�жϸü������Ӧ����λ��һ���׶��Ƿ����̵ƣ����̵ƣ����г�ͨ��Ҳ����Ҫ����ʱ��
			if(find_lamp_group(g_config.m_detector_setting.m_lamp_group[i])==0)
			{
				v = get_bit_value(st.m_status,(i+1),1);
				if(v > 0)
				{														// ��ǰ��λ��Ӧ�ļ�����������ʾ���г�ͨ��
					no_car_arrive_time = 0;										
					break;
				}
			}
		}
		//��λ������ǰ 5 ��,������������ļ���ֵ
		if (is_actuate_control && (g_controller_status.m_remain_time == 5))
			get_detector_counter(&detector_count_bak2);
	}
	if(timer == 9 && is_in_phase_step)
	{
		// �ж��ǲ�����λ������ǰ�����ҵ�ǰ���ǲ���ͨ�в�
		// ����ǵĻ������ݼ�����������Ҫ��Ҫ��ʱ
		int i;
		byte is_car_pass = 0;
		if(g_controller_status.m_remain_time != 3 )	
			return 0;	
		if (!is_actuate_control)// ����λ���޶�Ӧ�����ֱ�ӷ���
			return 0;									
		get_detector_counter(&detector_count_bak1);
		for(i=0; i<MAX_DETECTOR_COUNT; i++) 
		{
			if(!is_in_set(i+1,&set))
				continue;
			if (detector_count_bak1.m_counter[i] != detector_count_bak2.m_counter[i])// ��Ӧ������ļ���ֵ��һ������ʾ��ǰ���г�ͨ��������ʱ
			{
				//�жϸü������Ӧ����λ��һ���׶��Ƿ����̵ƣ����̵ƣ����г�ͨ��Ҳ����Ҫ����ʱ��
				if(find_lamp_group(g_config.m_detector_setting.m_lamp_group[i])==0)
				{
//					detector_count_bak2=detector_count_bak1;
					is_car_pass = 1;												
					break;
				}
			}
		}
		if (!is_car_pass && (no_car_arrive_time > vehicle_gap)) //���2����û��ͨ�������ҿ���ʱ����ڳ�ͷʱ�࣬�򲻼�ʱ��
			return 0;
		if(g_controller_status.m_assign_time+adjustment > max_green)
			return 0;														// �Ѿ������������
		// ��ʱ
		g_controller_status.m_assign_time += adjustment;
		g_controller_status.m_remain_time += adjustment;
	}
	return 0;
}

/**
 * ���㵱ǰ���е���λ�������λ��
 */
unsigned char phase_count()
{
//	unsigned char  i,j;
//	stage_t* p = &g_controller_status.m_current_phase_plan_p->m_stage[g_controller_status.m_stage-1];
//	j = 0x00;
//	for(i=0; i<MAX_PHASE_COUNT; i++)
//	{
////		if(is_term_connect_to_green(i+1) && decode_term_status(p, i+1) == ON) j++;
//		if(p->m_phase[i])
//			j++;
//	}
//	return j;
	return 0;
}

/**
 *  ���Ӧ����
 *  ֻ�ڴ���λ�ķ����ϰ�װ�����
 */
int simple_half_actuate_control(int mode_switch, int timer)
{
	static byte min_green;
	static byte max_green;
	static byte adjustment;
	static byte vehicle_gap;
	detector_counter_t detector_count_bak1;
	static detector_counter_t detector_count_bak2; 
	static byte no_car_arrive_time;
	static byte is_in_phase_step;
	static detector_set_t set;
	static byte is_actuate_control;//�Ƿ�Ҫ�ø�Ӧ���ƣ��޶�Ӧ��Ȧ����û�а취ʹ�ø�Ӧ���ƣ�
	if(mode_switch)
	{
		// ��ʼ�� 
		if(g_config.m_actuate_control_param.m_minmum_green<3)
			min_green=3;
		else
			min_green = g_config.m_actuate_control_param.m_minmum_green;
		if(g_config.m_actuate_control_param.m_maximum_green>min_green)
			max_green = g_config.m_actuate_control_param.m_maximum_green;
		else
			max_green = min_green+20;
		adjustment = g_config.m_actuate_control_param.m_adjust_quantum;
		if(adjustment<2) adjustment=2;
		vehicle_gap = g_config.m_actuate_control_param.m_vehicle_gap;
		no_car_arrive_time = 0;
		is_in_phase_step = 0;
		empty_set(&set);
	}
	no_car_arrive_time++;
	if(timer == 0 && is_step_end())
	{
		// ���²���
		int i;
		if(is_last_step())
		{
			byte s,p,t;
			// һ�������Ѿ��������
			if(g_config.m_actuate_control_param.m_minmum_green<3)
				min_green=3;
			else
				min_green = g_config.m_actuate_control_param.m_minmum_green;
			if(g_config.m_actuate_control_param.m_maximum_green>min_green)
				max_green = g_config.m_actuate_control_param.m_maximum_green;
			else
				max_green = min_green+20;
			adjustment = g_config.m_actuate_control_param.m_adjust_quantum;
			if(adjustment<2) adjustment=2;
			vehicle_gap = g_config.m_actuate_control_param.m_vehicle_gap;
			// ������Ҫ�����µ�ʱ��
			get_current_schedule(&s,&p,&t);
			// ��鵱ǰ��λ����ʱ�Ƿ�Ϸ�
			if(!is_schedule_valid(p,t))
			{
				// ʱ�����ݲ��Ϸ�
				set_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
				set_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
				return -1;
			}
			else
			{
				clear_error(INVALID_PHASE,&g_controller_status.m_error.m_soft_error);
				clear_error(INVALID_TIMMING,&g_controller_status.m_error.m_soft_error);
			} 
			load_current_schedule(s,p,t);
			if(g_controller_status.m_current_phase_plan_p->m_stage_count!=2)
				return -1;
			g_controller_status.m_step=1;
			g_controller_status.m_stage = 1;
			if(g_controller_status.m_current_phase_plan_p->m_stage_count>1)
				g_controller_status.m_next_stage=2;
			else
				g_controller_status.m_next_stage=1;
		}
		else
		{
			// ���²���
			set_current_stage();
		}
		run_current_step();		
		if(is_phase_step())
		{
			no_car_arrive_time = 0;
			is_in_phase_step = 1;
			// ��ȡ��λ��ؼ����
			empty_set(&set);
			get_phase_detector(&g_controller_status.m_current_lamp_status,&set);
			
			is_actuate_control = 0;
			for(i=0; i<MAX_DETECTOR_COUNT; i++) 
			{
				if(!is_in_set(i+1,&set))
					continue;
				is_actuate_control = 1;							// ����λ���ж�Ӧ�ļ������Ϊ����λ
				break;
			}
			// ����λ��, ��Ҫ�������������ʼ��ʱ
			// ���ݴ�ǰ����ĸ���λ����ı��賵����������һ����ʼ�̵�ʱ��
			if (is_actuate_control) 							// ��ʼ������λ�����Сʱ��
			{
				g_controller_status.m_remain_time = g_controller_status.m_assign_time = min_green;
			}
			else	// ��ʼ������λ�����Сʱ��
			{												
				max_green = max_green + 20;
				g_controller_status.m_remain_time = g_controller_status.m_assign_time = min_green + 10;
			}
		}
		else
			is_in_phase_step = 0;
	}
	
	if(timer == 5 && is_in_phase_step) {
		if(!is_detect_health_flag())//Ҫ����
		{
			// �ж��Ƿ��м��������
			if(is_detect_state(&set)==-1) 
				return -1;
			// �ж��Ƿ����źŵ��Ƿ��й���
			if(is_lamp_state()==-1)
				return -1;
		}
	}

	if(timer == 8 && is_in_phase_step) {
		int i;
		// �������״̬
		// ������м����
		detector_status_t st;
		get_detector_status(&st);

		for(i=0; i<MAX_DETECTOR_COUNT; i++) {
			int v;
			v = get_bit_value(st.m_status,(i+1),1);							// ��ȡ��ǰ��λ��Ӧ�ĳ��������״̬
			//�жϸü������Ӧ����λ��һ���׶��Ƿ����̵ƣ����̵ƣ����г�ͨ��Ҳ����Ҫ����ʱ��
			if(find_lamp_group(g_config.m_detector_setting.m_lamp_group[i])==0)
			{

				if(v > 0){														// �г�
					no_car_arrive_time = 0;
					break;
						}
					}
			}
//			��λ������ǰ 5 ��,������������ļ���ֵ
		if (g_controller_status.m_remain_time == 5) 
			get_detector_counter(&detector_count_bak2);
	}

	if(timer == 9 && is_in_phase_step)
	{
		// �ж��ǲ�����λ������ǰ�����ҵ�ǰ���ǲ���ͨ�в�
		// ����ǵĻ������ݼ�����������Ҫ��Ҫ��ʱ
		int i;
		byte is_current_car_pass=0;
		if(g_controller_status.m_remain_time != 3 )
			return 0;	
		get_detector_counter(&detector_count_bak1);
		for(i=0; i<MAX_DETECTOR_COUNT; i++)
		{
			if (detector_count_bak1.m_counter[i] != detector_count_bak2.m_counter[i]) 			// ��Ӧ������ļ���ֵ��һ������ʾ��ǰ
			{
				//�жϸü������Ӧ����λ��һ���׶��Ƿ����̵ƣ����̵ƣ����г�ͨ��Ҳ����Ҫ����ʱ��
				if(find_lamp_group(g_config.m_detector_setting.m_lamp_group[i])==0)
				{
					is_current_car_pass = 1;						// ���г�ͨ��������ʱ						
					break;
				}
			}
		}
		if (!is_actuate_control)											// ����λ��δװ�����������
		{
			if (is_current_car_pass | (no_car_arrive_time < vehicle_gap)) return 0; 		// �����������⵽����λ�г�
		}									
		else																// ����λ��װ�г����������
		{
			if (!is_current_car_pass && (no_car_arrive_time > vehicle_gap)) return 0;	// ���������û��⵽����λ�г� 
		}
		if(g_controller_status.m_assign_time +adjustment> max_green) return 0;														// �Ѿ������������
		// ��ʱ
		g_controller_status.m_assign_time += adjustment;
		g_controller_status.m_remain_time += adjustment;
	}
	return 0;
}

