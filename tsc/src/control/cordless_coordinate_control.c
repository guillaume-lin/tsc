/**
 * cordless_coordinate_control.c
 * 
 * ������Э������
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-05 11:48:58 �й���׼ʱ��
 * 
 * $log$
 *
 */
#include "io.h"
#include "kernel.h"

// ������ʱ����
static int get_cycle(int timming)
{
	int i,cycle;
	cycle = 0;
	if(timming < 1 || timming > MAX_TIMMING_PLAN_COUNT)
		return -1;
	for(i=0; i<g_config.m_timming_table[timming-1].m_step; i++)
	{
		cycle += g_config.m_timming_table[timming-1].m_greentimming[i];
		cycle += g_config.m_timming_table[timming-1].m_greenflashtimming[i];
		cycle += g_config.m_timming_table[timming-1].m_yellowtimming[i];
		cycle += g_config.m_timming_table[timming-1].m_redtimming[i];
	}
	return cycle;
}
/**
 * ������Э������
 */
int cordless_coordinate_control(int mode_switch, int timer)
{
	static unsigned char is_switching;
	static unsigned char phase;
	static unsigned char timming;

	if(mode_switch){
		int yellow,red,cycle;
		int hour,minute,offset;
		//�жϽ�������Э����ʱ���Ƿ���
		hour = g_config.m_cordless_coordinate_control_param.m_start_hour;
		minute = g_config.m_cordless_coordinate_control_param.m_start_minute;
		offset = g_config.m_cordless_coordinate_control_param.m_offset;

		
		if((g_controller_status.m_hour-hour)*(long)3600+
		   (g_controller_status.m_minute-minute)*60+
		    g_controller_status.m_second < 0){
			// ʱ�仹û��
			return -1;
		}
		//������λ����ʱ
		phase = g_config.m_cordless_coordinate_control_param.m_phase;
		timming = g_config.m_cordless_coordinate_control_param.m_timming;
		if(!is_schedule_valid(phase,timming))
			return -1;

		cycle = get_cycle(timming);
		// ������ɽ׶�����Ҫ��ʱ�䳤��
		yellow = ((g_controller_status.m_hour-hour)*(long)3600+
			 (g_controller_status.m_minute-minute)*60+
			 g_controller_status.m_second-offset) % cycle;
		
		yellow = cycle - yellow;
		if(yellow < 15)		
			yellow += cycle;	//ȷ����������10��
		yellow -= 5;
		red = 5;	// ���������

		// ������ɽ׶�
		is_switching = 1;
		switching_with_yellow_flash_all_red(1,yellow,red,timer);
	}

	if(is_switching){
		int ret = switching_with_yellow_flash_all_red(0,0,0,timer);
		if(ret == 1){
			is_switching = 0;
			// ����������Э�����Ƶ���ʱ����λ
			load_current_schedule(0,phase,timming);
			g_controller_status.m_step=1;
			g_controller_status.m_stage = 1;
			if(g_controller_status.m_current_phase_plan_p->m_stage_count>1)
				g_controller_status.m_next_stage=2;
			else
				g_controller_status.m_next_stage=1;
			run_current_step();
		}
		return 0;
	}

	if(timer == 0 && is_step_end()){
		// ���²���
		set_current_stage();
		run_current_step();
	}
	return 0;
}
