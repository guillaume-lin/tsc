/* @(#)greenconflictform.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-31 14:55:00 (中国标准时间)
*/

#include "greenconflictform.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"
#include "messagebox.hpp"

static CIntegerChecker zero_one_ic(0,1);
CGreenConflictForm::CGreenConflictForm()
	:CBaseForm(8,24,0,0)
{
	m_CurValue =0;
	m_Index = 1;

	F = new NCursesFormField*[22];
	F[0] = new CLabel("绿冲突设置",0,0);

	F[1] = new CLabel("*当前灯组",0,11);
	F[2] = new CField(2,0,20);
	m_monitor.set_form(this);
	F[2]->set_fieldtype(m_monitor);
	
	F[3] = new CLabel("灯组编号",1,0);
	F[4] = new CLabel("冲突情况",1,10);	

	F[5] = new CLabel("     ",2,0);
	F[6] = new CField(1,2,10);
	F[6]->set_fieldtype(zero_one_ic);
	F[7] = new CField(1,2,14);
	F[7]->set_fieldtype(zero_one_ic);
	F[8] = new CField(1,2,18);
	F[8]->set_fieldtype(zero_one_ic);

	F[9] = new CLabel("     ",3,0);
	F[10] = new CField(1,3,10);
	F[10]->set_fieldtype(zero_one_ic);
	F[11] = new CField(1,3,14);
	F[11]->set_fieldtype(zero_one_ic);
	F[12] = new CField(1,3,18);
	F[12]->set_fieldtype(zero_one_ic);

	F[13] = new CLabel("     ",4,0);
	F[14] = new CField(1,4,10);
	F[14]->set_fieldtype(zero_one_ic);
	F[15] = new CField(1,4,14);
	F[15]->set_fieldtype(zero_one_ic);
	F[16] = new CField(1,4,18);
	F[16]->set_fieldtype(zero_one_ic);

	F[17] = new CLabel("     ",5,0);
	F[18] = new CField(1,5,10);
	F[18]->set_fieldtype(zero_one_ic);
	F[19] = new CField(1,5,14);
	F[19]->set_fieldtype(zero_one_ic);
	F[20] = new CField(1,5,18);
	F[20]->set_fieldtype(zero_one_ic);

	
	F[21] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	//keypad(TRUE);
	memset((void*)&m_conflict,0x00,sizeof(green_conflict_t));
	onLoad();
	setLableValue();
}
int CGreenConflictForm::onLoad()
{
	vm_set_beep_option(&g_vm,1);
	return vm_get_green_conflict(&g_vm,&m_conflict);
}
int CGreenConflictForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	return vm_set_green_conflict(&g_vm,&m_conflict);
}
int CGreenConflictForm::onFieldChanged(NCursesFormField& f)
{
	int   FiledValue = atoi(f.value());
	if(FiledValue < 1  || FiledValue > MAX_LAMP_GROUP_COUNT)
	{
			f.set_value(itoa(m_Index));
			return -1;
	}
	m_Index = FiledValue;

	m_CurValue=0;
	setLableValue();
	return 0;
}
int CGreenConflictForm::doSet()
{
	// get data from ui and send to mcu

	m_Index = atoi(F[2]->value());
	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+1,atoi(F[6]->value()));
	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+2,atoi(F[7]->value()));
	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+3,atoi(F[8]->value()));

	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+4,atoi(F[10]->value()));
	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+5,atoi(F[11]->value()));
	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+6,atoi(F[12]->value()));

	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+7,atoi(F[14]->value()));
	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+8,atoi(F[15]->value()));
	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+9,atoi(F[16]->value()));

	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+10,atoi(F[18]->value()));
	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+11,atoi(F[19]->value()));
	green_conflict_set_conflict(&m_conflict,m_Index,m_CurValue*12+12,atoi(F[20]->value()));

	return 0;
}
int CGreenConflictForm::doGet()
{
	// get data from mcu and set to ui
	F[2]->set_value(itoa(m_Index));
	
	F[6]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+1)));
	F[7]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+2)));
	F[8]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+3)));

	F[10]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+4)));
	F[11]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+5)));
	F[12]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+6)));

	F[14]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+7)));
	F[15]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+8)));
	F[16]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+9)));

	F[18]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+10)));
	F[19]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+11)));
	F[20]->set_value(itoa(green_conflict_is_conflict(&m_conflict,m_Index,m_CurValue*12+12)));	

	return 0;
}

int CGreenConflictForm::onPageUp()
{
	doSet();
	if(m_CurValue > 0)
		m_CurValue--;
	setLableValue();
	return 0;
}
int CGreenConflictForm::onPageDown()
{
	doSet();
	if(m_CurValue < (MAX_LAMP_GROUP_COUNT/12)-1)
		m_CurValue++;
	setLableValue();
	return 0;
}
void     CGreenConflictForm::setLableValue(void)
{
	char buf[5];
	sprintf(buf,"%02d-%02d",m_CurValue*12+1,m_CurValue*12+3);
	F[5]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*12+4,m_CurValue*12+6);
	F[9]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*12+7,m_CurValue*12+9);
	F[13]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*12+10,m_CurValue*12+12);
	F[17]->set_value(buf);
	doGet();
}
