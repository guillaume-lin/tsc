/* @(#)terminalstatusform.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  张继良（pony_fx@sina.com)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2008-7-3 14:55:00 (中国标准时间)
*/
#include <assert.h>
#include "LoadDefaultParamform.hpp"
#include "label.hpp"
#include "field.hpp"
#include "messagebox.hpp"
#include "../control/vm.h"
#include "key_translate.h"

CLoadDefaultParamForm::CLoadDefaultParamForm()
	:CBaseForm(8,24,0,0)
{
	F = new NCursesFormField*[4];
	F[0] = new CLabel("装载默认参数",1,6);
//	F[1] = new CField(6,2,9);
//	m_Monitor.set_form(this);
//	F[1]->set_fieldtype(m_Monitor);
	F[1] = new CLabel("确认键  装载",3,6);
	F[2] = new CLabel("退出键  放弃",4,6);
	F[3] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
//	m_bFlag=0;
}
int CLoadDefaultParamForm::virtualize(int c)
{
	int err;
	switch(c){
	case KEY_ENTER:
	case CTRL('W'):
		{
				if(onSave()==0)
				{
					CMessageBox box("装载成功");
					box();
				}
				else
				{
					CMessageBox box("装载失败，请重试!");
					box();
				}	
				return NCursesForm::virtualize(CTRL('X'));					
		}
	case KEY_EXIT:// exit
		{
			return NCursesForm::virtualize(CTRL('X'));
		}
	default:
		break;
	}
	return NCursesForm::virtualize(c);
}
int CLoadDefaultParamForm::onSave()
{
		F[0]->set_value(" 装载参数中 ");
		F[1]->set_value("   请稍等   ");
		F[2]->set_value("            ");
		refresh();
		return Save();
}
int CLoadDefaultParamForm::Save()
{
	system_config_t system_config;
	LoadDefault_Param(system_config);
//vm_set_beep_option(&g_vm,1);
  int nRet=SaveConfig(&system_config);
	return nRet;
}
int CLoadDefaultParamForm::SaveConfig(system_config_t* config_p)
{
		if(vm_set_system_param(&g_vm,&config_p->m_system_param) != 0)
			return -1;
		if(vm_set_lamp_group_setting(&g_vm,&config_p->m_lamp_group_setting) != 0)
			return -1;
		if(vm_set_detector_setting(&g_vm,&config_p->m_detector_setting) != 0)
			return -1;
		if(vm_set_green_conflict(&g_vm,&config_p->m_green_conflict) != 0)
			return -1;
		//相位方案
		for(int p=0; p<MAX_PHASE_PLAN_COUNT; p++)
		{
			int step;
			int step_count;
			stage_plan_t plan = config_p->m_phase_table[p];
			step_count = plan.m_stage_count;
			for(step = 1; step <= step_count; step++)
				if(vm_set_phase_plan(&g_vm,p+1,step,step_count,&plan.m_stage[step-1]) != 0)
						return -1;
		}
		//配时方案
		for(int p=1; p<=MAX_TIMMING_PLAN_COUNT; p++)
		{
			timming_plan_t plan = config_p->m_timming_table[p-1];
			if(vm_set_timming_plan(&g_vm,&plan,p) != 0)
					return -1;
		}
		//时段方案		
		for(int p=1; p<=MAX_SCHEDULE_PLAN_COUNT; p++)
		{
					schedule_plan_t plan = config_p->m_schedule_table[p-1];
					if(vm_set_schedule_plan(&g_vm,&plan,p) != 0)
						return -1;
		}
		//特殊日
		if(vm_set_special_day_table(&g_vm,&config_p->m_special_day_table) != 0)
				return -1;
		//多时段
		if(vm_set_multiple_schedule_control_param(&g_vm,&config_p->m_multiple_schedule_control_param) != 0)
				return -1;
		//感应参数
		if(vm_set_actuate_control_param(&g_vm,&config_p->m_actuate_control_param) != 0)
				return -1;
		//无线协调参数
		if(vm_set_cordless_coordinate_control_param(&g_vm,&config_p->m_cordless_coordinate_control_param) != 0)
			return -1;
		//通讯端口
		for(int p=1; p<=MAX_COMM_COUNT; p++)
			if(vm_set_comm_param(&g_vm,p,&config_p->m_comm_param[p-1]) != 0)
				return -1;
		//系统控制模式
		if(vm_set_current_control_mode(&g_vm,&config_p->m_control_mode_selection) != 0)
			return -1;
		return 0;
}
int CLoadDefaultParamForm::LoadDefault_Param(system_config_t &config)
{
	memset(&config,0,sizeof(system_config_t));
	//验证感应参数
	config.m_actuate_control_param.m_detect_method=2;
	config.m_actuate_control_param.m_minmum_green=3;
	config.m_actuate_control_param.m_maximum_green =20;
	config.m_actuate_control_param.m_adjust_quantum=2;
	config.m_actuate_control_param.m_strategy=2;
	//无线协调控制模式
	config.m_cordless_coordinate_control_param.m_phase=1;
	config.m_cordless_coordinate_control_param.m_timming=1;
	//多时段
	config.m_multiple_schedule_control_param.m_schedule_plan=1;
	//控制模式
	config.m_control_mode_selection.m_control_mode=MULTIPLE_SCHEDULE_CONTROL;
	//系统参数
	config.m_system_param.m_initial_all_red_time=5;
	config.m_system_param.m_initial_yellow_flash_time=10;
	config.m_system_param.m_run_degraded_time=100;
	//相位表
	config.m_phase_table[0].m_stage_count=4;
	config.m_phase_table[0].m_stage[0].m_phase[0]=1;
	config.m_phase_table[0].m_stage[0].m_phase[1]=3;
	config.m_phase_table[0].m_stage[0].m_phase[2]=7;
	config.m_phase_table[0].m_stage[0].m_phase[3]=9;
	config.m_phase_table[0].m_stage[0].m_phase[4]=11;
	config.m_phase_table[0].m_stage[0].m_phase[5]=15;	
	config.m_phase_table[0].m_stage[1].m_phase[0]=2;
	config.m_phase_table[0].m_stage[1].m_phase[1]=3;
	config.m_phase_table[0].m_stage[1].m_phase[2]=7;
	config.m_phase_table[0].m_stage[1].m_phase[3]=10;
	config.m_phase_table[0].m_stage[1].m_phase[4]=11;
	config.m_phase_table[0].m_stage[1].m_phase[5]=15;
	config.m_phase_table[0].m_stage[1].m_phase[6]=21;
	config.m_phase_table[0].m_stage[1].m_phase[7]=22;
	config.m_phase_table[0].m_stage[1].m_phase[8]=23;
	config.m_phase_table[0].m_stage[1].m_phase[9]=24;
	config.m_phase_table[0].m_stage[2].m_phase[0]=3;
	config.m_phase_table[0].m_stage[2].m_phase[1]=5;
	config.m_phase_table[0].m_stage[2].m_phase[2]=7;
	config.m_phase_table[0].m_stage[2].m_phase[3]=11;
	config.m_phase_table[0].m_stage[2].m_phase[4]=13;
	config.m_phase_table[0].m_stage[2].m_phase[5]=15;
	config.m_phase_table[0].m_stage[3].m_phase[0]=3;
	config.m_phase_table[0].m_stage[3].m_phase[1]=6;
	config.m_phase_table[0].m_stage[3].m_phase[2]=7;
	config.m_phase_table[0].m_stage[3].m_phase[3]=11;
	config.m_phase_table[0].m_stage[3].m_phase[4]=14;
	config.m_phase_table[0].m_stage[3].m_phase[5]=15;
	config.m_phase_table[0].m_stage[3].m_phase[6]=17;
	config.m_phase_table[0].m_stage[3].m_phase[7]=18;
	config.m_phase_table[0].m_stage[3].m_phase[8]=19;
	config.m_phase_table[0].m_stage[3].m_phase[9]=20;
	//配时表
	config.m_timming_table[0].m_step=4;
	config.m_timming_table[0].m_greentimming[0]=15;
	config.m_timming_table[0].m_greenflashtimming[0]=2;
	config.m_timming_table[0].m_yellowtimming[0]=3;
	config.m_timming_table[0].m_redtimming[0]=2;
	config.m_timming_table[0].m_greentimming[1]=20;
	config.m_timming_table[0].m_greenflashtimming[1]=2;
	config.m_timming_table[0].m_yellowtimming[1]=3;
	config.m_timming_table[0].m_redtimming[1]=2;
	config.m_timming_table[0].m_greentimming[2]=15;
	config.m_timming_table[0].m_greenflashtimming[2]=2;
	config.m_timming_table[0].m_yellowtimming[2]=3;
	config.m_timming_table[0].m_redtimming[2]=2;
	config.m_timming_table[0].m_greentimming[3]=20;
	config.m_timming_table[0].m_greenflashtimming[3]=2;
	config.m_timming_table[0].m_yellowtimming[3]=3;
	config.m_timming_table[0].m_redtimming[3]=2;

	config.m_timming_table[1].m_step=4;
	config.m_timming_table[1].m_greentimming[0]=2;
	config.m_timming_table[1].m_greenflashtimming[0]=2;
	config.m_timming_table[1].m_yellowtimming[0]=2;
	config.m_timming_table[1].m_redtimming[0]=2;
	config.m_timming_table[1].m_greentimming[1]=2;
	config.m_timming_table[1].m_greenflashtimming[1]=2;
	config.m_timming_table[1].m_yellowtimming[1]=2;
	config.m_timming_table[1].m_redtimming[1]=2;
	config.m_timming_table[1].m_greentimming[2]=2;
	config.m_timming_table[1].m_greenflashtimming[2]=2;
	config.m_timming_table[1].m_yellowtimming[2]=2;
	config.m_timming_table[1].m_redtimming[2]=2;
	config.m_timming_table[1].m_greentimming[3]=2;
	config.m_timming_table[1].m_greenflashtimming[3]=2;
	config.m_timming_table[1].m_yellowtimming[3]=2;
	config.m_timming_table[1].m_redtimming[3]=2;
	//时段表
	config.m_schedule_table[0].m_time_segment_count=7;
        int i=0;
	for(i=0;i<7;i++)
	{
		config.m_schedule_table[i].m_phase[0]=1;
		config.m_schedule_table[i].m_timming[0]=1;
	}
	//信号灯的设置
	for(i=0;i<MAX_TERMINAL_COUNT/8;i++)
		config.m_lamp_setting.m_use[i]=255;
	//灯组设置
	config.m_lamp_group_setting.m_lamp_group_count=24;
	config.m_lamp_group_setting.m_lamp_group[0][0]=1;
	config.m_lamp_group_setting.m_lamp_group[0][2]=2;
	config.m_lamp_group_setting.m_lamp_group[0][1]=3;
	config.m_lamp_group_setting.m_lamp_group[1][0]=4;
	config.m_lamp_group_setting.m_lamp_group[1][2]=5;
	config.m_lamp_group_setting.m_lamp_group[1][1]=6;
	config.m_lamp_group_setting.m_lamp_group[2][0]=7;
	config.m_lamp_group_setting.m_lamp_group[2][2]=8;
	config.m_lamp_group_setting.m_lamp_group[2][1]=9;
	config.m_lamp_group_setting.m_lamp_group[3][0]=0;
	config.m_lamp_group_setting.m_lamp_group[3][2]=0;
	config.m_lamp_group_setting.m_lamp_group[3][1]=0;
	config.m_lamp_group_setting.m_lamp_group[4][0]=13;
	config.m_lamp_group_setting.m_lamp_group[4][2]=14;
	config.m_lamp_group_setting.m_lamp_group[4][1]=15;
	config.m_lamp_group_setting.m_lamp_group[5][0]=16;
	config.m_lamp_group_setting.m_lamp_group[5][2]=17;
	config.m_lamp_group_setting.m_lamp_group[5][1]=18;
	config.m_lamp_group_setting.m_lamp_group[6][0]=19;
	config.m_lamp_group_setting.m_lamp_group[6][2]=20;
	config.m_lamp_group_setting.m_lamp_group[6][1]=21;
	config.m_lamp_group_setting.m_lamp_group[7][0]=0;
	config.m_lamp_group_setting.m_lamp_group[7][2]=0;
	config.m_lamp_group_setting.m_lamp_group[7][1]=0;
	config.m_lamp_group_setting.m_lamp_group[8][0]=25;
	config.m_lamp_group_setting.m_lamp_group[8][2]=26;
	config.m_lamp_group_setting.m_lamp_group[8][1]=27;
	config.m_lamp_group_setting.m_lamp_group[9][0]=28;
	config.m_lamp_group_setting.m_lamp_group[9][2]=29;
	config.m_lamp_group_setting.m_lamp_group[9][1]=30;
	config.m_lamp_group_setting.m_lamp_group[10][0]=31;
	config.m_lamp_group_setting.m_lamp_group[10][2]=32;
	config.m_lamp_group_setting.m_lamp_group[10][1]=33;
	config.m_lamp_group_setting.m_lamp_group[11][0]=0;
	config.m_lamp_group_setting.m_lamp_group[11][2]=0;
	config.m_lamp_group_setting.m_lamp_group[11][1]=0;
	config.m_lamp_group_setting.m_lamp_group[12][0]=37;
	config.m_lamp_group_setting.m_lamp_group[12][2]=38;
	config.m_lamp_group_setting.m_lamp_group[12][1]=39;
	config.m_lamp_group_setting.m_lamp_group[13][0]=40;
	config.m_lamp_group_setting.m_lamp_group[13][2]=41;
	config.m_lamp_group_setting.m_lamp_group[13][1]=42;
	config.m_lamp_group_setting.m_lamp_group[14][0]=43;
	config.m_lamp_group_setting.m_lamp_group[14][2]=44;
	config.m_lamp_group_setting.m_lamp_group[14][1]=45;
	config.m_lamp_group_setting.m_lamp_group[15][0]=0;
	config.m_lamp_group_setting.m_lamp_group[15][1]=0;
	config.m_lamp_group_setting.m_lamp_group[15][2]=0;
	config.m_lamp_group_setting.m_lamp_group[16][0]=49;
	config.m_lamp_group_setting.m_lamp_group[16][2]=0;
	config.m_lamp_group_setting.m_lamp_group[16][1]=51;
	config.m_lamp_group_setting.m_lamp_group[17][0]=52;
	config.m_lamp_group_setting.m_lamp_group[17][2]=0;
	config.m_lamp_group_setting.m_lamp_group[17][1]=54;
	config.m_lamp_group_setting.m_lamp_group[18][0]=55;
	config.m_lamp_group_setting.m_lamp_group[18][2]=0;
	config.m_lamp_group_setting.m_lamp_group[18][1]=57;
	config.m_lamp_group_setting.m_lamp_group[19][0]=58;
	config.m_lamp_group_setting.m_lamp_group[19][2]=0;
	config.m_lamp_group_setting.m_lamp_group[19][1]=60;
	config.m_lamp_group_setting.m_lamp_group[20][0]=61;
	config.m_lamp_group_setting.m_lamp_group[20][2]=0;
	config.m_lamp_group_setting.m_lamp_group[20][1]=63;
	config.m_lamp_group_setting.m_lamp_group[21][0]=64;
	config.m_lamp_group_setting.m_lamp_group[21][2]=0;
	config.m_lamp_group_setting.m_lamp_group[21][1]=66;
	config.m_lamp_group_setting.m_lamp_group[22][0]=67;
	config.m_lamp_group_setting.m_lamp_group[22][2]=0;
	config.m_lamp_group_setting.m_lamp_group[22][1]=69;
	config.m_lamp_group_setting.m_lamp_group[23][0]=70;
	config.m_lamp_group_setting.m_lamp_group[23][2]=0;
	config.m_lamp_group_setting.m_lamp_group[23][1]=72;
	//检测器设置
	for(i=0;i<MAX_DETECTOR_COUNT;i++)
	{
		if(i<16)
		{
			config.m_detector_setting.m_attribute[i]=255;
			config.m_detector_setting.m_lamp_group[i]=i+1;
		}
//		else if(i<24)
//		{
//			config.m_detector_setting.m_attribute[i]=255;
//			config.m_detector_setting.m_lamp_group[i]=i-12+1;
//		}
		else
		{
			config.m_detector_setting.m_attribute[i]=0;
			config.m_detector_setting.m_lamp_group[i]=0;

		}
	}
	config.m_detector_setting.m_distance=4;
	return 0;
}
int CLoadDefaultParamForm::onFieldChanged(NCursesFormField& f)
{
//		int   FiledValue = atol(f.value());
//		if(F[1] == &f)//密码
//		{
//			if(FiledValue!=183392)
//				return -1;
//			m_bFlag=1;
//			postKey(CTRL('I'));
//		}
		return 0;	
}
