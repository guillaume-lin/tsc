/* @(#)ControlModeSelectionForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 18:44:07 (中国标准时间)
*/

#include "ControlModeSelectionForm.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"


CIntegerChecker mode_ic(1,8);

CControlModeSelectionForm::CControlModeSelectionForm()
	:CBaseForm(7,24,1,1)
{
	F = new NCursesFormField*[4];
	F[0] = new CLabel("控制模式设置",1,3);
	F[1] = new CLabel("控制模式:",2,3);
	F[2] = new CField(1,2,12);
	F[2]->set_fieldtype(mode_ic);
	F[3] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	onLoad();
	doGet();
}
int CControlModeSelectionForm::onLoad()
{
	vm_set_beep_option(&g_vm,1);
	return vm_get_setup_control_mode(&g_vm,&m_data);
}
int CControlModeSelectionForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	return vm_set_setup_control_mode(&g_vm,&m_data);
}
int CControlModeSelectionForm::doSet()
{
	// get data from ui and send to mcu
	//beep();
	m_data.m_control_mode = (byte)atoi(F[2]->value());
	return 0;
}
int CControlModeSelectionForm::doGet()
{
	// get data from mcu and set to ui
	F[2]->set_value(itoa(m_data.m_control_mode));
	return 0;
}
