/**
 * actuate_control.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-14 15:26:10 中国标准时间
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
 * 判断当前步是不是相位步
 */
int is_phase_step()
{
	if(g_controller_status.m_step==1)//阶段的第一步为相位步
		return 1;
	return 0;
}
// 检测器集
//detector_set_t set;

// 
// 简单的全感应控制
// 给相应的相位一个最小绿，如果持续有车的话，就增加相位时间，一直到最大绿
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
	
	if(mode_switch)//模式转化的时候，初始化参数
	{
		byte ps,ts;

		phase = g_config.m_pedestrian_control_param.m_phase;
		timming = g_config.m_pedestrian_control_param.m_timming;
		ps = g_config.m_phase_table[phase-1].m_step;                                                                                      
		ts = g_config.m_timming_table[timming-1].m_step;                                                                                  
		if(ps <= 0 || ts <= 0 || ps != ts) return -1;				// 若查表得到的相位方案或配时方案号的总步数不一致或为 0 时，表示不合法。直接返回

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
			// 一个周期已经运行完毕
			//重读感应参数
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
			// 根据需要运行新的时段
			get_current_schedule(&s,&p,&t);
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
			// 更新步号
			set_current_stage();
		}
		if ((g_controller_status.m_phase == PHASE_YELLOW_FLASH) || (g_controller_status.m_phase == PHASE_ALL_RED) || (g_controller_status.m_phase == PHASE_ALL_OFF))
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
		if(is_phase_step())//是相位步
		{
			no_car_arrive_time = 0;
			is_in_phase_step = 1;
			// 是相位步, 需要根据情况给定初始配时
			// 根据此前计算的该相位方向的被阻车辆数，给定一个起始绿灯时间
			g_controller_status.m_remain_time = 
				g_controller_status.m_assign_time = min_green;
			// 获取相位相关检测器
			empty_set(&set);
			get_phase_detector(&g_controller_status.m_current_lamp_status,&set);

		}
		else
			is_in_phase_step = 0;
		is_actuate_control = 0;
	}
	if(timer == 5 && is_in_phase_step)
	{
		if(!is_detect_health_flag())//要求检测
		{
			// 判断是否有检测器故障
			if(is_detect_state(&set)==-1) 
				return -1;
			// 判断是否有信号灯是否有故障
			if(is_lamp_state()==-1)
				return -1;
		}
	}

	if(timer == 8 && is_in_phase_step)//检查当前是否有车通过
	{
		int i;
		// 检查检测器状态
		// 只检查相位相关检测器
		detector_status_t st;
		get_detector_status(&st);
		no_car_arrive_time++;

		for(i=0; i<MAX_DETECTOR_COUNT; i++)
		{
			int v;
			if(!is_in_set(i+1,&set))
				continue;
			is_actuate_control = 1;											// 该相位步有对应的检测器
			//判断该检测器对应的相位下一个阶段是否亮绿灯，亮绿灯，则有车通过也不需要增加时间
			if(find_lamp_group(g_config.m_detector_setting.m_lamp_group[i])==0)
			{
				v = get_bit_value(st.m_status,(i+1),1);
				if(v > 0)
				{														// 当前相位对应的检测器有输出表示正有车通过
					no_car_arrive_time = 0;										
					break;
				}
			}
		}
		//相位结束的前 5 秒,读出各检测器的计数值
		if (is_actuate_control && (g_controller_status.m_remain_time == 5))
			get_detector_counter(&detector_count_bak2);
	}
	if(timer == 9 && is_in_phase_step)
	{
		// 判断是不是相位结束的前三秒且当前步是不是通行步
		// 如果是的话，根据检测器情况决定要不要延时
		int i;
		byte is_car_pass = 0;
		if(g_controller_status.m_remain_time != 3 )	
			return 0;	
		if (!is_actuate_control)// 该相位步无对应检测器直接返回
			return 0;									
		get_detector_counter(&detector_count_bak1);
		for(i=0; i<MAX_DETECTOR_COUNT; i++) 
		{
			if(!is_in_set(i+1,&set))
				continue;
			if (detector_count_bak1.m_counter[i] != detector_count_bak2.m_counter[i])// 对应检测器的计数值不一样，表示当前正有车通过，需延时
			{
				//判断该检测器对应的相位下一个阶段是否亮绿灯，亮绿灯，则有车通过也不需要增加时间
				if(find_lamp_group(g_config.m_detector_setting.m_lamp_group[i])==0)
				{
//					detector_count_bak2=detector_count_bak1;
					is_car_pass = 1;												
					break;
				}
			}
		}
		if (!is_car_pass && (no_car_arrive_time > vehicle_gap)) //最近2秒内没车通过，并且空闲时间大于车头时距，则不加时间
			return 0;
		if(g_controller_status.m_assign_time+adjustment > max_green)
			return 0;														// 已经超过最大绿了
		// 延时
		g_controller_status.m_assign_time += adjustment;
		g_controller_status.m_remain_time += adjustment;
	}
	return 0;
}

/**
 * 计算当前运行的相位表的总相位数
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
 *  半感应控制
 *  只在次相位的方向上安装检测器
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
	static byte is_actuate_control;//是否要用感应控制（无对应线圈，就没有办法使用感应控制）
	if(mode_switch)
	{
		// 初始化 
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
		// 更新步伐
		int i;
		if(is_last_step())
		{
			byte s,p,t;
			// 一个周期已经运行完毕
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
			// 根据需要运行新的时段
			get_current_schedule(&s,&p,&t);
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
			// 更新步号
			set_current_stage();
		}
		run_current_step();		
		if(is_phase_step())
		{
			no_car_arrive_time = 0;
			is_in_phase_step = 1;
			// 获取相位相关检测器
			empty_set(&set);
			get_phase_detector(&g_controller_status.m_current_lamp_status,&set);
			
			is_actuate_control = 0;
			for(i=0; i<MAX_DETECTOR_COUNT; i++) 
			{
				if(!is_in_set(i+1,&set))
					continue;
				is_actuate_control = 1;							// 该相位步有对应的检测器，为次相位
				break;
			}
			// 是相位步, 需要根据情况给定初始配时
			// 根据此前计算的该相位方向的被阻车辆数，给定一个起始绿灯时间
			if (is_actuate_control) 							// 初始化次相位最大最小时间
			{
				g_controller_status.m_remain_time = g_controller_status.m_assign_time = min_green;
			}
			else	// 初始化主相位最大最小时间
			{												
				max_green = max_green + 20;
				g_controller_status.m_remain_time = g_controller_status.m_assign_time = min_green + 10;
			}
		}
		else
			is_in_phase_step = 0;
	}
	
	if(timer == 5 && is_in_phase_step) {
		if(!is_detect_health_flag())//要求检测
		{
			// 判断是否有检测器故障
			if(is_detect_state(&set)==-1) 
				return -1;
			// 判断是否有信号灯是否有故障
			if(is_lamp_state()==-1)
				return -1;
		}
	}

	if(timer == 8 && is_in_phase_step) {
		int i;
		// 检查检测器状态
		// 检查所有检测器
		detector_status_t st;
		get_detector_status(&st);

		for(i=0; i<MAX_DETECTOR_COUNT; i++) {
			int v;
			v = get_bit_value(st.m_status,(i+1),1);							// 读取当前相位对应的车辆检测器状态
			//判断该检测器对应的相位下一个阶段是否亮绿灯，亮绿灯，则有车通过也不需要增加时间
			if(find_lamp_group(g_config.m_detector_setting.m_lamp_group[i])==0)
			{

				if(v > 0){														// 有车
					no_car_arrive_time = 0;
					break;
						}
					}
			}
//			相位结束的前 5 秒,读出各检测器的计数值
		if (g_controller_status.m_remain_time == 5) 
			get_detector_counter(&detector_count_bak2);
	}

	if(timer == 9 && is_in_phase_step)
	{
		// 判断是不是相位结束的前三秒且当前步是不是通行步
		// 如果是的话，根据检测器情况决定要不要延时
		int i;
		byte is_current_car_pass=0;
		if(g_controller_status.m_remain_time != 3 )
			return 0;	
		get_detector_counter(&detector_count_bak1);
		for(i=0; i<MAX_DETECTOR_COUNT; i++)
		{
			if (detector_count_bak1.m_counter[i] != detector_count_bak2.m_counter[i]) 			// 对应检测器的计数值不一样，表示当前
			{
				//判断该检测器对应的相位下一个阶段是否亮绿灯，亮绿灯，则有车通过也不需要增加时间
				if(find_lamp_group(g_config.m_detector_setting.m_lamp_group[i])==0)
				{
					is_current_car_pass = 1;						// 正有车通过，需延时						
					break;
				}
			}
		}
		if (!is_actuate_control)											// 主相位（未装车辆检测器）
		{
			if (is_current_car_pass | (no_car_arrive_time < vehicle_gap)) return 0; 		// 车辆检测器检测到次相位有车
		}									
		else																// 次相位（装有车辆检测器）
		{
			if (!is_current_car_pass && (no_car_arrive_time > vehicle_gap)) return 0;	// 车辆检测器没检测到次相位有车 
		}
		if(g_controller_status.m_assign_time +adjustment> max_green) return 0;														// 已经超过最大绿了
		// 延时
		g_controller_status.m_assign_time += adjustment;
		g_controller_status.m_remain_time += adjustment;
	}
	return 0;
}

