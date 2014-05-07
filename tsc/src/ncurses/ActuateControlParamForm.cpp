/* @(#)ActuateControlParamForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 13:30:00 (中国标准时间)
*/

#include "ActuateControlParamForm.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"

static CIntegerChecker min_green_ic(3,255);
static CIntegerChecker max_green_ic(3,255);
//static CIntegerChecker timming_ic(1,MAX_TIMMING_PLAN_COUNT);
//static CIntegerChecker phase_ic(1,MAX_PHASE_PLAN_COUNT);
static CIntegerChecker strategy_ic(1,2);
static CIntegerChecker car_dist_ic(0,255);
static CIntegerChecker detector_ic(1,2);
static CIntegerChecker adjust_ic(0,255);

CActuateControlParamForm::CActuateControlParamForm()
	:CBaseForm(7,24,0,0)
{
	F = new NCursesFormField*[14];
	F[0] = new CLabel("感应控制参数设置",0,3);
	
	F[5] = new CLabel("最小绿:",2,0);
	F[6] = new CField(2,2,9);
	F[6]->set_fieldtype(min_green_ic);

	F[7] = new CLabel("最大绿:",2,11);
	F[8] = new CField(2,2,19);
	F[8]->set_fieldtype(max_green_ic);

//	F[9] = new CLabel("配时方案",2,0);
//	F[10] = new CField(2,2,9);
//	F[10]->set_fieldtype(timming_ic);

//	F[11] = new CLabel("相位方案",2,11);
//	F[12] = new CField(1,2,19);
//	F[12]->set_fieldtype(phase_ic);

	F[9] = new CLabel("控制策略",3,0);
	F[10] = new CField(1,3,9);
	F[10]->set_fieldtype(strategy_ic);

	F[11] = new CLabel("车头时距:",3,11);
	F[12] = new CField(2,3,19);
	F[12]->set_fieldtype(car_dist_ic);

	F[1] = new CLabel("检测方式",4,0);
	F[2] = new CField(2,4,9);
	F[2]->set_fieldtype(detector_ic);

	F[3] = new CLabel("步长调整量",4,11);
	F[4] = new CField(2,4,19);
	F[4]->set_fieldtype(adjust_ic);

	F[13] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	onLoad();
	doGet();
}
int CActuateControlParamForm::onLoad()
{	
	vm_set_beep_option(&g_vm,1);
	return vm_get_actuate_control_param(&g_vm,&m_data);
}
int CActuateControlParamForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	return vm_set_actuate_control_param(&g_vm,&m_data);
}
int CActuateControlParamForm::doSet()
{
	// get data from ui and send to mcu
	//beep();
	m_data.m_detect_method = (byte)atoi(F[2]->value());
	m_data.m_adjust_quantum = (byte)atoi(F[4]->value());
	m_data.m_minmum_green = (byte)atoi(F[6]->value());
	m_data.m_maximum_green = (byte)atoi(F[8]->value());
//	m_data.m_timming = (byte)atoi(F[10]->value());
//	m_data.m_phase = (byte)atoi(F[12]->value());
	m_data.m_strategy = (byte)atoi(F[10]->value());
	m_data.m_vehicle_gap = (byte)atoi(F[12]->value());
	return 0;
}
int CActuateControlParamForm::doGet()
{
	// get data from mcu and set to ui
	F[2]->set_value(itoa(m_data.m_detect_method));
	F[4]->set_value(itoa(m_data.m_adjust_quantum));
	F[6]->set_value(itoa(m_data.m_minmum_green));
	F[8]->set_value(itoa(m_data.m_maximum_green));
//	F[10]->set_value(itoa(m_data.m_timming));
//	F[12]->set_value(itoa(m_data.m_phase));
	F[10]->set_value(itoa(m_data.m_strategy));
	F[12]->set_value(itoa(m_data.m_vehicle_gap));
	return 0;
}
