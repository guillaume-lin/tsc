/* @(#)CordlessCoordinateControlParamForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 18:44:07 (中国标准时间)
*/

#include "CordlessCoordinateControlParamForm.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"

static CIntegerChecker offset_ic(0,255);
static CIntegerChecker phase_ic(1,MAX_PHASE_PLAN_COUNT);
CIntegerChecker timming_ic(1,MAX_TIMMING_PLAN_COUNT);
CIntegerChecker hour_ic(0,23);
CIntegerChecker minute_ic(0,59);

CCordlessCoordinateControlParamForm::CCordlessCoordinateControlParamForm()
	:CBaseForm(8,24,0,0)
{
	F = new NCursesFormField*[12];
	F[0] = new CLabel("无线缆协调参数设置",0,3);

	F[1] = new CLabel("相位差:",1,3);
	F[2] = new CField(3,1,10);
	F[2]->set_fieldtype(offset_ic);

	F[3] = new CLabel("相位方案:",2,3);
	F[4] = new CField(1,2,12);
	F[4]->set_fieldtype(phase_ic);

	F[5] = new CLabel("配时方案:",3,3);
	F[6] = new CField(2,3,12);
	F[6]->set_fieldtype(timming_ic);

	F[7] = new CLabel("基准小时:",4,3);
	F[8] = new CField(2,4,12);
	F[8]->set_fieldtype(hour_ic);

	F[9] = new CLabel("基准分钟:",5,3);
	F[10] = new CField(2,5,12);
	F[10]->set_fieldtype(minute_ic);

	F[11] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	onLoad();
	doGet();
}
int CCordlessCoordinateControlParamForm::onLoad()
{
	vm_set_beep_option(&g_vm,1);
	return vm_get_cordless_coordinate_control_param(&g_vm,&m_data);
}
int CCordlessCoordinateControlParamForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	return vm_set_cordless_coordinate_control_param(&g_vm,&m_data);
}
int CCordlessCoordinateControlParamForm::doSet()
{
	// get data from ui and send to mcu
	//beep();
	m_data.m_offset = (byte)atoi(F[2]->value());
	m_data.m_phase = (byte)atoi(F[4]->value());
	m_data.m_timming = (byte)atoi(F[6]->value());
	m_data.m_start_hour = (byte)atoi(F[8]->value());
	m_data.m_start_minute = (byte)atoi(F[10]->value());
	return 0;
}
int CCordlessCoordinateControlParamForm::doGet()
{
	// get data from mcu and set to ui
	F[2]->set_value(itoa(m_data.m_offset));
	F[4]->set_value(itoa(m_data.m_phase));
	F[6]->set_value(itoa(m_data.m_timming));
	F[8]->set_value(itoa(m_data.m_start_hour));
	F[10]->set_value(itoa(m_data.m_start_minute));
	return 0;
}
