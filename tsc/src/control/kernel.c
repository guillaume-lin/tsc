/**
 * kernel.c
 * 
 * ������ѭ��ʵ���ļ�
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-06-25 10:16:00 �й���׼ʱ��
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

// ����Ҫ�õ���ȫ�����ݽṹ
system_config_t g_config;	// ��������

volume_occupy_group_t g_volume_occupy;

stage_plan_t g_special_phase_plan;
lamp_status_t g_special_phase;
timming_plan_t g_special_phase_timming_plan;

// �źŻ���ȫ��״̬
controller_status_t g_controller_status;
#ifndef __C51__
typedef byte bit;
#endif
command_reactor_t g_command_reactor2,g_command_reactor7,g_command_reactor8;	
extern void wait_for_10ms(void);

/**
 *  ��ʼ�������β���
 *
 */
//0921 int init_special_schedule_data()

int init_special_schedule_data(unsigned char ctemp)
{
	int i;

	// ������������λ����
	for(i=0; i<MAX_TERMINAL_COUNT; i++)
	{
		g_special_phase.m_output[i] = 0;
	}
	// ������λֵ
	g_special_phase_plan.m_stage_count=1;
	for(i=0;i<MAX_STAGE_COUNT;i++)
	{
		g_special_phase_plan.m_stage[0].m_phase[i]=0;
	}
	// ��ʼ��������ʱ
	g_special_phase_timming_plan.m_step = 1; 			// ��Ϊ������λֻ��1��
	for(i=0; i<MAX_STAGE_COUNT; i++)
	{
		if (ctemp == ALL_RED_CONTROL) 				// ��ʼ��ȫ����λ
		{
			g_special_phase_timming_plan.m_greentimming[i] = 3; // ÿ��ֻ��3��
			g_special_phase_timming_plan.m_greenflashtimming[i]=0;
			g_special_phase_timming_plan.m_yellowtimming[i]=0;
			g_special_phase_timming_plan.m_redtimming[i]=0;
		}
		else  if (ctemp == YELLOW_FLASH_CONTROL)
		{
			g_special_phase_timming_plan.m_greentimming[i] = 3; // ÿ��ֻ��3��
			g_special_phase_timming_plan.m_greenflashtimming[i]=0;
			g_special_phase_timming_plan.m_yellowtimming[i]=0;
			g_special_phase_timming_plan.m_redtimming[i]=0;
		}
		else if(ctemp ==ALL_OFF_CONTROL)
		{
			g_special_phase_timming_plan.m_greentimming[i] = 0; // ÿ��ֻ��3��
			g_special_phase_timming_plan.m_greenflashtimming[i]=0;
			g_special_phase_timming_plan.m_yellowtimming[i]=0;
			g_special_phase_timming_plan.m_redtimming[i]=0;
		}
	}

	for(i=0;i<MAX_LAMP_GROUP_COUNT;i++){
		int r,y;
		if (ctemp == YELLOW_FLASH_CONTROL) {					// ��ʼ��������λ
			y = g_config.m_lamp_group_setting.m_lamp_group[i][YELLOW];
			if(y > 0){
				// ���ö��������Ϊ��
				int p = (y-1)/4;		// �������ڵ��ֽ�
				int offset = (y-1)%4;		// �ֽ������ƫ��0-3
				//0921				byte* b = &g_yellow_flash_phase_plan.m_phase[0].m_output[p];
				byte* b = &g_special_phase.m_output[p];
				byte flash = 0x02 << 2*(3 - offset);
				*b &= ~(0x03 << 2*(3 - offset)); //������
				*b |= flash;			// ��ֵ
			}
		}
		if (ctemp == ALL_RED_CONTROL) {					// ��ʼ��ȫ����λ
			r = g_config.m_lamp_group_setting.m_lamp_group[i][RED];
			if(r > 0){
				// ���ö�����Ϊ��
				int p = (r-1)/4;
				int offset = (r-1)%4;
				//0921				byte* b = &g_all_red_phase_plan.m_phase[0].m_output[p];
				byte* b = &g_special_phase.m_output[p];
				byte on = 0x01 << 2*(3 - offset);
				*b &= ~(0x03 << 2*(3- offset));	// ������
				*b |= on;			// ��ֵ
			}
		}
	}
	return 0;
}


/**
 * �жϽ����Ƿ�Ϊ������
 * ����ǵĻ�,���ظ���Ӧ�����е�ʱ�η���
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
// ȷ����ʱ��״̬����Ӧ���ߵ���λ��������ʱ����
int get_next_schedule(byte* schedule,byte* phase,byte* timming,byte second)
{
	int time,count;
	int i,plan;
	schedule_plan_t schedule_plan;

	// �����ж�Ҫʹ���ĸ�ʱ�η���
	// 1. �Ƿ�ʹ���ض���ʱ�η���
	if(g_config.m_multiple_schedule_control_param.m_schedule_plan > 7){
		int p = g_config.m_multiple_schedule_control_param.m_schedule_plan;
		schedule_plan = g_config.m_schedule_table[p-1];
		*schedule = p;
		// 2. �Ƿ����������տ���
	}else if(g_config.m_multiple_schedule_control_param.m_enable_special &&
		(plan = is_today_special_day()) > 0){
			*schedule = plan;
			schedule_plan = g_config.m_schedule_table[plan-1]; 
		}
		// 3. ��ȱʡ��ʽѡ��ʱ�η���
	else{
		int t = g_controller_status.m_week;
		if(t == 0)
			t = 7;		// ������ʹ��ʱ�η���7

		schedule_plan = g_config.m_schedule_table[t-1];
		*schedule = t;
	}

	// ����ȷ��ʱ�Σ�ѡ����λ�Լ���ʱ 
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

	if(count == 0)//��ʾ��ʱ������Ϊ�գ����Ϸ�������
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
// ȷ����ʱ��״̬����Ӧ���ߵ���λ��������ʱ����
int get_current_schedule(byte* schedule,byte* phase,byte* timming)
{
	int time,count;
	int i,plan;
	schedule_plan_t schedule_plan;

	// �����ж�Ҫʹ���ĸ�ʱ�η���
	// 1. �Ƿ�ʹ���ض���ʱ�η���
	if(g_config.m_multiple_schedule_control_param.m_schedule_plan > 7){
		int p = g_config.m_multiple_schedule_control_param.m_schedule_plan;
		schedule_plan = g_config.m_schedule_table[p-1];
		*schedule = p;
	// 2. �Ƿ����������տ���
	}else if(g_config.m_multiple_schedule_control_param.m_enable_special &&
			(plan = is_today_special_day()) > 0){
		*schedule = plan;
		schedule_plan = g_config.m_schedule_table[plan-1]; 
	}
	// 3. ��ȱʡ��ʽѡ��ʱ�η���
	else{
		int t = g_controller_status.m_week;
		if(t == 0)
			t = 7;		// ������ʹ��ʱ�η���7

		schedule_plan = g_config.m_schedule_table[t-1];
		*schedule = t;
	}

	// ����ȷ��ʱ�Σ�ѡ����λ�Լ���ʱ 
	
	time = g_controller_status.m_hour*60 + g_controller_status.m_minute;

    count = 0;
    for(i=0; i<MAX_TIME_SEGMENT_COUNT; i++){
    	if(schedule_plan.m_phase[i] == 0)
    		break;
    	count++;
    		
    }
	*phase   = 0;
	*timming = 0;   

    if(count == 0)//��ʾ��ʱ������Ϊ�գ����Ϸ�������
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
// *	ת�����ض���λ����ʱ
// */
//int switching_with_schedule(byte schedule,byte phase, byte timming)
//{
//	// ��鵱ǰ��λ����ʱ�Ƿ�Ϸ�
//	if(!is_schedule_valid(phase,timming)){
//		// ʱ�����ݲ��Ϸ�
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
//		// ����ʱ��
//		g_controller_status.m_elapse_time = 0;
//		// ȷ���²�����ʱ
//		set_phase_time();
//		// �����µĵ�״̬
//		set_term_status(&g_controller_status.m_current_lamp_status);
//	}
//	else
//		run_current_step();
//	return 0;
//}
/**
 *  ���뵱ǰʹ�õ���λ��������ʱ����
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
	   phase == PHASE_ALL_OFF)//������λ��ʹ��Ĭ����ʱ��
	{
		return 1;
	}

	if(phase == 0 || timming == 0)
		return 0;// ������õ�����λ��������ʱ������Ϊ 0 ʱ����ʾ���Ϸ�
	else if(phase > MAX_PHASE_PLAN_COUNT || timming > MAX_TIMMING_PLAN_COUNT)
		return 0;//����������ã����Ϸ�������0
	
	ps = g_config.m_phase_table[phase-1].m_stage_count;
	ts = g_config.m_timming_table[timming-1].m_step;
	if(ps <= 0 || ts <= 0 || ps != ts)// ������õ�����λ��������ʱ�����ŵ��ܲ�����һ�»�Ϊ 0 ʱ����ʾ���Ϸ�
		return 0;				
	return 1;
}
// �жϵ�ǰ���Ƿ������һ��
int is_last_step()
{
	// ���ҵ�ǰ��λ�Ĳ��������ж��Ƿ��Ѿ����е���󲽷���
	if(g_controller_status.m_stage >= g_controller_status.m_current_phase_plan_p->m_stage_count)
		if(g_controller_status.m_step>=4)
			return 1;
	return 0;
}
//�жϵ�ǰ���Ƿ������׶εĵ�2���������׶εĵ�2����Ҫ��ȡ�¸�ʱ��Ҫ�ߵ���λ��������ʱ����
int is_last_stage_first_step()
{
	if(g_controller_status.m_stage >= g_controller_status.m_current_phase_plan_p->m_stage_count)
		if(g_controller_status.m_step==1)
			return 1;
	return 0;
}
// �жϵ�ǰ���Ƿ��Ѿ�����
int is_step_end()
{
	if(g_controller_status.m_remain_time > 0)
		return 0;
	else
		return 1;

}
//�ж�����Э��ʱ����Ƿ��Ѿ�����
int  is_cordless_coordinate_time()
{
	int hour,minute;
	unsigned char phase;
	unsigned char timming;
	//�ж�ʱ���Ƿ���
	hour = g_config.m_cordless_coordinate_control_param.m_start_hour;
	minute = g_config.m_cordless_coordinate_control_param.m_start_minute;


	if((g_controller_status.m_hour-hour)*(long)3600+
		(g_controller_status.m_minute-minute)*60+
		g_controller_status.m_second < 0)
	{
		// ʱ�仹û��
		return -1;
	}
	//�ж���λ����ʱ�Ƿ���Ч
	phase = g_config.m_cordless_coordinate_control_param.m_phase;
	timming = g_config.m_cordless_coordinate_control_param.m_timming;
	if(!is_schedule_valid(phase,timming))
		return -1;

	return 0;
}
//��ȡ��ʹ�õ���Ȧ��������ж���Ȧ��״̬�û�
//�и���Ȧ���򷵻�-1�����򷵻�0
int judge_detector_state()
{
	int i=0;
	detector_set_t set;
	empty_set(&set);
	//��ȡ��ǰ����λ�������õ�����Ȧ�����
	for(i=0;i<g_config.m_lamp_group_setting.m_lamp_group_count;i++)
	{
		get_lamp_group_detector(i,&set);
	}
	// �ж��Ƿ��м��������
	if(is_detect_state(&set)==-1) 
		return -1;
	return 0;
}
//�жϼ������״̬
int  is_detect_state(detector_set_t *pset)
{
	int i,v;
	get_detector_health(&g_controller_status.m_detector_health);
	//�ж���Ȧ������Ƿ��й���
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
 //�ж��źŵ�״̬
int  is_lamp_state()
{
	int i,u,v;
	get_lamp_health(&g_controller_status.m_lamp_health);
	// �ж��Ƿ��е����Ƿ��й���
	for(i=0; i<MAX_TERMINAL_COUNT; i++)
	{
		u = get_bit_value(g_config.m_lamp_setting.m_use,(i+1),1);
		if(u > 0)//���źŵ�����
		{
			v = get_bit_value(g_controller_status.m_lamp_health.m_health,(i+1),1);
			if(v > 0) return -1;
		}
	}
	return  0;
}
//���õ�ǰ�Ľ׶����Ͷ�Ӧ����λ��
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

// �ж��Ƿ�һ���������н���
//w	int is_cycle_end()
//w	{
//w		return is_last_step() && is_step_end();
//w	}

// ����ȫ��״̬�еĲ���, �����ǰ��
int run_current_step()
{
	memset(&g_controller_status.m_current_lamp_status,0,sizeof(lamp_status_t));
	// ����ʱ��
	g_controller_status.m_elapse_time = 0;
	// ȷ���²�����ʱ
	set_phase_time();
	if(g_controller_status.m_remain_time>0)
	{
		// ȷ���µ���λ
		set_phase_state(&g_controller_status.m_current_lamp_status);	
		// �����µĵ�״̬
		set_term_status(&g_controller_status.m_current_lamp_status);
	}
	return 0;
}
/**
 * ��������
 */
int yellow_flash_control(int mode_switch, int timer)
{
	// ���õ�ɫΪ����
	if(mode_switch){
		init_special_schedule_data(YELLOW_FLASH_CONTROL);
		load_current_schedule(0,PHASE_YELLOW_FLASH,TIMMING_SPECIAL_PHASE);
		g_controller_status.m_step = 1;
		g_controller_status.m_stage=1;
		g_controller_status.m_next_stage=1;
		// ����ʱ��
		g_controller_status.m_elapse_time = 0;
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = timer;
		// �����µĵ�״̬
		set_term_status(&g_controller_status.m_current_lamp_status);
	}
	return 0;
}
/**
 * ��������
 */
int all_red_control(int mode_switch, int timer)
{
	// ���õ�ɫΪ�����
	if(mode_switch){
		init_special_schedule_data(ALL_RED_CONTROL);
		load_current_schedule(0,PHASE_ALL_RED,TIMMING_SPECIAL_PHASE);
		g_controller_status.m_step = 1;
		g_controller_status.m_stage=1;
		g_controller_status.m_next_stage=1;
		// ����ʱ��
		g_controller_status.m_elapse_time = 0;
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = timer;
		// �����µĵ�״̬
		set_term_status(&g_controller_status.m_current_lamp_status);
	}
	return 0;
}
/**
 * ��ƿ���
 */
int all_off_control(int mode_switch, int timer)
{
	if(mode_switch){
		init_special_schedule_data(ALL_OFF_CONTROL);
		load_current_schedule(0,PHASE_ALL_OFF,TIMMING_SPECIAL_PHASE);
		g_controller_status.m_step = 1;
		g_controller_status.m_stage=1;
		g_controller_status.m_next_stage=1;
		// ����ʱ��
		g_controller_status.m_elapse_time = 0;
		g_controller_status.m_remain_time = 
			g_controller_status.m_assign_time = timer;
		// �����µĵ�״̬
		set_term_status(&g_controller_status.m_current_lamp_status);
	}
	return 0;
}

/**
 * ���ݵ�ǰʱ�ε���ʱ�������굱ǰ����
 * ���� 1 ��ʾ�������
 * 
 */
int switching_with_multiple_schedule(int mode_switch, int timer)
{
	if(timer == 0){
		if(is_step_end() && is_last_step()){
			// һ�������Ѿ��������
			// ������Ҫ�����µ�ʱ��
			return 1;
		}
		// ������ǰ���������
		else if(is_step_end()){
			// ���²���
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
 * �����Ȼ����������ķ�ʽ���й���
 * ����1��ʾ�������
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
		stage = 1;	// �������
	}


	switch(stage){
		case 1:	// ����
			if(yellow_time == 0){
				// ���������
				all_red_control(1,0);
				stage = 2;
			}
			if(timer == 0)
				yellow_time--;
			break;
		case 2: // �����

			if(red_time == 0){
				// ���������
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
	// ����ģʽ����
	byte m_mode;
	// ����ģʽ
	control_mode_t m_control_mode;
	// �����õĿ���ģʽ
	control_mode_t m_backup_control_mode;
}control_mode_entry_t;

/**
 * ���Ʋ��Լ���
 */
static 
#ifdef __C51__	// C51������ϣ��������code������
code
#endif
control_mode_entry_t control_mode_table[] = {
	// ���
	{ALL_OFF_CONTROL,all_off_control,all_off_control,},
	// ȫ��
	{ALL_RED_CONTROL,all_red_control,all_off_control,},
	// ����
	{YELLOW_FLASH_CONTROL,yellow_flash_control,all_off_control,},
	// ���
	{MULTIPLE_SCHEDULE_CONTROL,multiple_schedule_control,yellow_flash_control,},
	// ��Ӧ
	{ACTUATE_CONTROL,actuate_control,multiple_schedule_control},
	
	// ������Э������
	{CORDLESS_COORDINATE_CONTROL,cordless_coordinate_control,actuate_control,},

	// ����Э������
	// {ONLINE_HARMONY_CONTROL,actuate_control,},
	// ִ�л�����
	{EXECUTIVE_MACHINE_CONTROL,executive_machine_control,actuate_control,},

	// �ֶ�����
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
 *  ��ȡָ������ģʽ�Ŀ����ӳ���
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
 * ��ȡָ������ģʽ�Ľ��������ӳ���
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
//�жϵ�ǰ�Ŀ���ģʽ�������Ƿ�����
int judge_ctrol_mode_condition(int nMode)
{
	switch(nMode) {
	case EXECUTIVE_MACHINE_CONTROL:// ִ�л�����
		{
			if(g_controller_status.m_online_command.m_cmd!=ONLINE)//�յ���������
					return -1;
			if(!is_detect_health_flag())//Ҫ����
			{
				// �ж��Ƿ��м��������
//				if(judge_detector_state()==-1)
//					return -1;
				// �ж��Ƿ����źŵ��Ƿ��й���
				if(is_lamp_state()==-1)
					return 0;
			}
			return -1;
		}
		break;
	case ONLINE_HARMONY_CONTROL:// ����Э������
		break;
	case CORDLESS_COORDINATE_CONTROL:// ������Э������
		{
			//�ж�ʱ���Ƿ���
			if(is_cordless_coordinate_time()==-1)
				return -1;
			if(!is_detect_health_flag())//Ҫ����
			{
				// �ж��Ƿ��м��������
//				if(judge_detector_state()==-1)
//					return -1;
				// �ж��Ƿ����źŵ��Ƿ��й���
				if(is_lamp_state()==-1)
					return -1;
				return 0;
			}
			return -1;
		}
		break;
	case ACTUATE_CONTROL:// ��Ӧ����
		{
			if(!is_detect_health_flag())//Ҫ����
			{
				// �ж��Ƿ��м��������
				if(judge_detector_state()==-1)
					return -1;
				// �ж��Ƿ����źŵ��Ƿ��й���
				if(is_lamp_state()==-1)
					return -1;
				return 0;
			}
			return -1;
		}
		break;
	case MULTIPLE_SCHEDULE_CONTROL:// ��ο���
		{
			if(!is_detect_health_flag())//Ҫ����
			{
				// �ж��Ƿ����źŵ��Ƿ��й���
				if(is_lamp_state()==-1)
					return -1;
				return 0;
			}
			return -1;
		}
		break;
	case YELLOW_FLASH_CONTROL:// ��������
		return 0;
	case ALL_RED_CONTROL:// ȫ�����
		return 0;
	case ALL_OFF_CONTROL:// ��ƿ���
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
		if(judge_ctrol_mode_condition(nMode)==0)//��ģʽҪ��������Ѿ��ﵽ
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
			if(nMode==-1)//�����ڵķ�ʽ
				return multiple_schedule_control;
		}
	}
	return get_control_mode(nMode);
}
/**
 * ȷ����һ��ʱ�����Ҫ���еĿ���ģʽ
 *
 */
static control_mode_t do_schedule(int control_result,control_mode_t last_control)
{
	control_mode_t control_mode;
	int mode;
	unsigned char i;
	bit run_mode;
	mode = g_controller_status.m_designate_control_mode;

	if(is_manual_control_state()){																	// ���ж��Ƿ����ֶ�����
		g_controller_status.m_designate_control_mode = MANUAL_CONTROL;
	} 												
	else {
		run_mode=get_run_mode();                                                                //�Ƿ����źŻ�ǰ����趨�Ĺ���ģʽ��1����
		if (run_mode) {																			// �����źŻ�ǰ����ϵ��趨����
			clear_run_mode();
			i = get_front_panel_control_mode();
			if (i!=0x00) // �źŻ�ǰ����ϵ��趨��Чʱ����ǰ����ϵ��趨����
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
	//	// ����ģʽ�ı䣬��ս�����־
	//	g_controller_status.m_is_degraded = 0;
	//	g_controller_status.m_degraded_time=0;
	//}
	control_mode = last_control;
	mode = get_mode(last_control);
	if(mode != g_controller_status.m_designate_control_mode && 
		!g_controller_status.m_is_degraded)
	{

		// �µĿ���ģʽ
		control_mode = get_control_mode(g_controller_status.m_designate_control_mode);
		g_controller_status.m_is_degraded = 0;
		g_controller_status.m_degraded_time=0;
	}
	else if(mode != g_controller_status.m_designate_control_mode &&g_controller_status.m_designate_control_mode==MANUAL_CONTROL)
	{
		// �µĿ���ģʽ
		control_mode = get_control_mode(g_controller_status.m_designate_control_mode);
		g_controller_status.m_is_degraded = 0;
		g_controller_status.m_degraded_time=0;
	}
	else if(control_result == -1){

		// ��������
		control_mode = get_backup_control_mode(mode);
		g_controller_status.m_is_degraded = 1;
	}
	else if(g_controller_status.m_is_degraded&&
		    g_controller_status.m_degraded_time>g_config.m_system_param.m_run_degraded_time)
	{
		//���ڽ����ģ�Ҫ�ж�����豸�Ƿ��Ѿ��ָ�
		control_mode=get_current_ctrol_mode(g_controller_status.m_designate_control_mode,mode);
	}
	return control_mode;
}

/**
 * @desc �ȴ�ָ��������
 * @param sec Ҫ�ȴ�������
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
 * �����Ƴ���
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
	
	//�����ĳ�ʼ״̬���Ȼ�����ȫ��
	set_init_state();

	// ��ʼ��ͨ��
	init_command_reactor(&g_command_reactor2);
	g_command_reactor2.m_fd = open_comm(2);

	init_command_reactor(&g_command_reactor7);
	g_command_reactor7.m_fd = open_comm(7);

	init_command_reactor(&g_command_reactor8);
	g_command_reactor8.m_fd = open_comm(8);

	g_controller_status.m_timer = 0;
	g_controller_status.m_restart = 0;
	g_controller_status.m_designate_control_mode=g_config.m_control_mode_selection.m_control_mode;
	// ����Ԥ���Ŀ���ģʽ
	while(!g_controller_status.m_restart){
		// �����ϴε����н���Լ�������Ϣ�ж��Ƿ�Ҫ�л�����ģʽ
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

		// ���е�ǰ�Ŀ���ģʽ
		control_result = current_control(mode_switch,timer);
		//ͨ��
		reactor_handle_command(&g_command_reactor2);					// ͨ�Ŵ��� uart0 ����ͨ�� 2
		reactor_handle_command(&g_command_reactor8);					// ͨ�Ŵ��� cpld ͨ�� 1
		reactor_handle_command(&g_command_reactor7);					// ͨ�Ŵ��� cpld ͨ�� 1		        
		// �ȴ���һ������
		wait_for_100ms();
		timer++;
		if(timer >= 10)
			timer = 0;

		g_controller_status.m_timer = timer;
		//0.1�����һ�γ�������ռ����
		get_volume_occupy(&detector_volume,detector_occupy);
		if(timer == 0){
			// ��������ʱ��
			g_controller_status.m_elapse_time++;
			// ����ʣ��ʱ��
			if(g_controller_status.m_remain_time > 0)
				g_controller_status.m_remain_time--;
			//����ǿ��ʱ��
			if(g_controller_status.m_is_degraded)
				g_controller_status.m_degraded_time++;
			if(g_controller_status.m_is_config_updated){
				save_config_data();
				g_controller_status.m_is_config_updated = 0;
			}

			// һ���Ӹ���һ���¶�
			g_controller_status.m_temperature = get_temperature();

			if(is_door_illegal_open())
				g_controller_status.m_error.m_is_door_illegal_open = 1;
			else
				g_controller_status.m_error.m_is_door_illegal_open = 0;

		}
	}
	return 0;
}
//�õ���������ռ����
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
		{														// ��ǰ��λ��Ӧ�ļ�����������ʾ���г�ͨ��
			detector_occupy[i]++;
		}
	}
	if(minute==0)  minute=g_controller_status.m_minute;
	if(minute!=g_controller_status.m_minute)//�Ѿ�����һ���ӣ�����1���ӳ�������1���ӳ�����Ϣ
	{
		get_detector_counter(&detector_count);
		//��ȡ���λ��
		g_volume_occupy.m_pointer=(g_volume_occupy.m_pointer+1)%MAX_VOLUME_OCCCUPY_NUM;
		//����ʱ��
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_year[0]=g_controller_status.m_year[0];
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_year[1]=g_controller_status.m_year[1];
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_month=g_controller_status.m_month;
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_day=g_controller_status.m_day;
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_hour=g_controller_status.m_hour;
		g_volume_occupy.vo[g_volume_occupy.m_pointer].m_minute=minute;
		for(i=0;i<MAX_DETECTOR_COUNT;i++)
		{
			//������
			if(detector_count.m_counter[i]>=pdetector_volume->m_counter[i])
				g_volume_occupy.vo[g_volume_occupy.m_pointer].m_volume[i]=detector_count.m_counter[i]-pdetector_volume->m_counter[i];
			else
				g_volume_occupy.vo[g_volume_occupy.m_pointer].m_volume[i]=detector_count.m_counter[i]+255-pdetector_volume->m_counter[i];
			//ռ����
			g_volume_occupy.vo[g_volume_occupy.m_pointer].m_occupy[i]=detector_occupy[i]/6;
		}
		//��ʼ����������ռ���ʱ���
		get_detector_counter(pdetector_volume);
		memset(detector_occupy,0,sizeof(int)*MAX_DETECTOR_COUNT);
		minute=g_controller_status.m_minute;
	}
	return 0;
}
//���ó�ʼ״̬
//�Ȼ���n��,��ȫ��n��
void set_init_state()
{
	//����
	init_special_schedule_data(YELLOW_FLASH_CONTROL);
	// �����µĵ�״̬
	set_term_status(&g_special_phase);
	do_wait(g_config.m_system_param.m_initial_yellow_flash_time);
	// ȫ��
	init_special_schedule_data(ALL_RED_CONTROL);
	// �����µĵ�״̬
	load_current_schedule(0,PHASE_ALL_RED,TIMMING_SPECIAL_PHASE);
	g_controller_status.m_step = 1;
	g_controller_status.m_stage=1;
	g_controller_status.m_next_stage=1;
	// ����ʱ��
	g_controller_status.m_elapse_time = 0;
	g_controller_status.m_remain_time = 
		g_controller_status.m_assign_time = g_config.m_system_param.m_initial_all_red_time;
	// �����µĵ�״̬
	set_term_status(&g_controller_status.m_current_lamp_status);
	return;
}
//���õƵ�״̬  state 0- ��1���� 2����
void set_lamp_state(lamp_status_t *phase,int termial,int state)
{
	//int u;
	//u = get_bit_value(g_config.m_lamp_setting.m_use,termial,1);
	//if(u==0) //���źŵƲ����ã�����������״̬������������
	//	return;
	if(termial> 0)
	{
		// ���ö��������Ϊ��
		int p = (termial-1)/4;		// �������ڵ��ֽ�
		int offset = (termial-1)%4;		// �ֽ������ƫ��0-3
		byte* b = &phase->m_output[p];
		byte on = state << 2*(3 - offset);
//		*b &= ~(0x03 << 2*(3 - offset)); //������
		*b |= on;			// ��ֵ
	}
}
//�жϵ��������״̬���Ƿ�Ҫ�����ڳ��죬���̣�����״̬Ŷ 
int judge_lamp_group_work_mode(int nRed,int nYellow,int nGreen)
{
	if(nRed==0&&nGreen==0&&nYellow)//ֻ�лƵƴ���
		return YELLOW;//��ʱ��������
	else if(nYellow==0&&nRed&&nGreen==0)//ֻ�к�ƴ���
		return RED;//��ʱ�������
	else if(nGreen&&nRed==0&&nYellow==0)//ֻ���̵ƴ���
		return GREEN;
	else 
		return -1;
}
//�����µ���λ
int set_phase_state(lamp_status_t* phase)
{
	int i=0;
	int lamp_group=0;
	int m_lamp_group[MAX_LAMP_GROUP_COUNT];
	memset(&m_lamp_group,0,sizeof(int)*MAX_LAMP_GROUP_COUNT);
	for(i=0;i<MAX_PHASE_COUNT;i++)
	{
		lamp_group=g_controller_status.m_current_phase_plan_p->m_stage[g_controller_status.m_stage-1].m_phase[i];
		if(lamp_group<=g_config.m_lamp_group_setting.m_lamp_group_count&&lamp_group>0)//��ǰ�������������
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
			if(g_controller_status.m_step==1)//��һ�������̵�
			{
				set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][GREEN],ON);
			}
			else
			{
				if(find_lamp_group(i+1))//�¸��׶μ������̵�
				{
					set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][GREEN],ON);
				}
				else //�¸��׶������
				{
					if(g_controller_status.m_step==2)//�ڶ�����������
					{
						set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][GREEN],FLASH);
					}
					else if(g_controller_status.m_step==3)//�����������Ƶ�
					{
						if(g_config.m_lamp_group_setting.m_lamp_group[i][YELLOW])//�Ƶƴ���
							set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][YELLOW],ON);
						else
							set_lamp_state(phase,g_config.m_lamp_group_setting.m_lamp_group[i][RED],ON);					
					}
					else if(g_controller_status.m_step==4)//���Ĳ��������
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
//���ҵ�ǰ��������һ���׶����Ƿ����̵�
int find_lamp_group(int lamp_group)
{
	int i=0;
	int m_stage=0;
	////�ж���һ�׶����Ǹ��׶�
	//if(g_controller_status.m_stage==g_controller_status.m_current_phase_plan_p->m_stage_count)
	//	m_stage=1;
	//else
	//	m_stage=g_controller_status.m_stage+1;
	m_stage=g_controller_status.m_next_stage;
	if(m_stage==1)//���׶�
	{
		//���ҵ�ǰ�����Ƿ�����һ�׶���
		for(i=0;i<MAX_PHASE_COUNT;i++)
		{
			if(g_controller_status.m_next_phase_plan_p->m_stage[0].m_phase[i]==lamp_group)
				return 1;
		}
	}
	else
	{
		//���ҵ�ǰ�����Ƿ�����һ�׶���
		for(i=0;i<MAX_PHASE_COUNT;i++)
		{
			if(g_controller_status.m_current_phase_plan_p->m_stage[m_stage-1].m_phase[i]==lamp_group)
				return 1;
		}
	}
	return 0;
}
