/* @(#)MultipleScheduleControlParamForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 11:44:07 (中国标准时间)
*/

#include "MultipleScheduleControlParamForm.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"

static CIntegerChecker zero_one_ic(0,1);
static CIntegerChecker schedule_ic(1,MAX_SCHEDULE_PLAN_COUNT);

CMultipleScheduleControlParamForm::CMultipleScheduleControlParamForm()
	:CBaseForm(7,24,1,1)
{
	F = new NCursesFormField*[6];
	F[0] = new CLabel("多时段控制参数",1,3);
	F[1] = new CLabel("启用特殊日控制:",2,3);
	F[2] = new CField(1,2,18);
	F[2]->set_fieldtype(zero_one_ic);
	F[3] = new CLabel("使用时段方案号:",3,3);
	F[4] = new CField(2,3,18);
	F[4]->set_fieldtype(schedule_ic);
	F[5] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	onLoad();
	doGet();
}
int CMultipleScheduleControlParamForm::onLoad()
{
	vm_set_beep_option(&g_vm,1);
	return vm_get_multiple_schedule_control_param(&g_vm,&m_data);
}
int CMultipleScheduleControlParamForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	return vm_set_multiple_schedule_control_param(&g_vm,&m_data);
}
int CMultipleScheduleControlParamForm::doSet()
{
	// get data from ui and send to mcu
	//beep();
	m_data.m_enable_special = (byte)atoi(F[2]->value());
	m_data.m_schedule_plan = (byte)atoi(F[4]->value());
	return 0;
}
int CMultipleScheduleControlParamForm::doGet()
{
	// get data from mcu and set to ui
	F[2]->set_value(itoa(m_data.m_enable_special));
	F[4]->set_value(itoa(m_data.m_schedule_plan	));
	return 0;
}
