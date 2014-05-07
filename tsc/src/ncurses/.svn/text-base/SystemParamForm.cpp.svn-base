/* @(#)SystemParamForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-19 
*/

#include "SystemParamForm.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"

CIntegerChecker time_ic(3,255);

CSystemParamForm::CSystemParamForm()
	:CBaseForm(7,24,1,1)
{
	F = new NCursesFormField*[8];
	F[0] = new CLabel("系统参数设置",1,3);
	F[1] = new CLabel("黄闪时间:",2,3);
	F[2] = new CField(3,2,12);
	F[2]->set_fieldtype(time_ic);
	F[3] = new CLabel("四面全红时间:",3,3);
	F[4] = new CField(3,3,16);
	F[4]->set_fieldtype(time_ic);
	F[5] = new CLabel("恢复时间间隔:",4,3);
	F[6] = new CField(3,4,16);
	F[7] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	onLoad();
	doGet();
}
int CSystemParamForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	return vm_set_system_param(&g_vm,&m_data);
}
int CSystemParamForm::onLoad()
{
	vm_set_beep_option(&g_vm,1);
	return vm_get_system_param(&g_vm,&m_data);
}
int CSystemParamForm::doSet()
{
	// get data from ui and send to mcu
	//beep();
	m_data.m_initial_yellow_flash_time = (byte)atoi(F[2]->value());
	m_data.m_initial_all_red_time = (byte)atoi(F[4]->value());
	m_data.m_run_degraded_time=(byte)atoi(F[6]->value());
	return 0;
}
int CSystemParamForm::doGet()
{
	// get data from mcu and set to ui
	F[2]->set_value(itoa(m_data.m_initial_yellow_flash_time));
	F[4]->set_value(itoa(m_data.m_initial_all_red_time	));
	F[6]->set_value(itoa(m_data.m_run_degraded_time));
	return 0;
}
