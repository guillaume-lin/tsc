/* @(#)LampGroupSettingForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 16:0:00 (中国标准时间)
*/

#include "LampGroupSettingForm.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"

CIntegerChecker group_count(1,MAX_LAMP_GROUP_COUNT);
CIntegerChecker term_value(0,MAX_TERMINAL_COUNT);

CLampGroupSettingForm::CLampGroupSettingForm()
	:CBaseForm(8,24,0,0)
{
	m_CurValue =0;
	m_LampGroupCount=0;
	F = new NCursesFormField*[24];
	F[0] = new CLabel("灯组设置",0,8);
	
	F[1] = new CLabel("灯组总数：",1,0);
	F[2] = new CField(2,1,11);
//F[2]->set_fieldtype(group_count);
	m_Monitor.set_form(this);
	F[2]->set_fieldtype(m_Monitor);
	F[3] = new CLabel("编号:",2,0);
	F[4] = new CLabel("  ",2,7);
	F[5] = new CLabel("  ",2,11);
	F[6] = new CLabel("  ",2,15);
	F[7] = new CLabel("  ",2,19);
	F[8] = new CLabel("红灯:",3,0);
	F[9] = new CField(2,3,7);
	F[9]->set_fieldtype(term_value);
	F[10] = new CField(2,3,11);
	F[10]->set_fieldtype(term_value);
	F[11] = new CField(2,3,15);
	F[11]->set_fieldtype(term_value);
	F[12] = new CField(2,3,19);
	F[12]->set_fieldtype(term_value);

	F[13] = new CLabel("黄灯:",4,0);
	F[14] = new CField(2,4,7);
	F[14]->set_fieldtype(term_value);
	F[15] = new CField(2,4,11);
	F[15]->set_fieldtype(term_value);
	F[16] = new CField(2,4,15);
	F[16]->set_fieldtype(term_value);
	F[17] = new CField(2,4,19);
	F[17]->set_fieldtype(term_value);

	F[18] = new CLabel("绿灯:",5,0);
	F[19] = new CField(2,5,7);
	F[19]->set_fieldtype(term_value);
	F[20] = new CField(2,5,11);
	F[20]->set_fieldtype(term_value);
	F[21] = new CField(2,5,15);
	F[21]->set_fieldtype(term_value);
	F[22] = new CField(2,5,19);
	F[22]->set_fieldtype(term_value);

	F[23] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	onLoad();
	setLableValue();
}
int CLampGroupSettingForm::onLoad()
{
	vm_set_beep_option(&g_vm,1);
	if(vm_get_lamp_group_setting(&g_vm,&m_data) == 0)
		return 0;
	else
		return -1;
}
int CLampGroupSettingForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	
	if(vm_set_lamp_group_setting(&g_vm,&m_data) == 0)
		return 0;
	else
		return -1;

}
int CLampGroupSettingForm::doSet()
{
	// get data from ui and send to mcu
	m_data.m_lamp_group_count = (byte)atoi(F[2]->value());
	
	m_data.m_lamp_group[m_CurValue*4][RED] = (byte)atoi(F[9]->value());
	m_data.m_lamp_group[m_CurValue*4][YELLOW] = (byte)atoi(F[14]->value());
	m_data.m_lamp_group[m_CurValue*4][GREEN] = (byte)atoi(F[19]->value());

	if(m_CurValue*4+1<m_data.m_lamp_group_count)
	{
		m_data.m_lamp_group[m_CurValue*4+1][RED] = (byte)atoi(F[10]->value());
		m_data.m_lamp_group[m_CurValue*4+1][YELLOW] = (byte)atoi(F[15]->value());
		m_data.m_lamp_group[m_CurValue*4+1][GREEN] = (byte)atoi(F[20]->value());		
	}	
	else if(m_CurValue*4+1<MAX_LAMP_GROUP_COUNT)
	{
		m_data.m_lamp_group[m_CurValue*4+1][RED] = 0;
		m_data.m_lamp_group[m_CurValue*4+1][YELLOW] = 0;
		m_data.m_lamp_group[m_CurValue*4+1][GREEN] = 0;		
	}

	if(m_CurValue*4+2<m_data.m_lamp_group_count)
	{
		m_data.m_lamp_group[m_CurValue*4+2][RED] = (byte)atoi(F[11]->value());
		m_data.m_lamp_group[m_CurValue*4+2][YELLOW] = (byte)atoi(F[16]->value());
		m_data.m_lamp_group[m_CurValue*4+2][GREEN] = (byte)atoi(F[21]->value());
	}
	else if(m_CurValue*4+2<MAX_LAMP_GROUP_COUNT)
	{
		m_data.m_lamp_group[m_CurValue*4+2][RED] = 0;
		m_data.m_lamp_group[m_CurValue*4+2][YELLOW] = 0;
		m_data.m_lamp_group[m_CurValue*4+2][GREEN] = 0;		
	}
	
	if(m_CurValue*4+3<m_data.m_lamp_group_count)
	{
		m_data.m_lamp_group[m_CurValue*4+3][RED] = (byte)atoi(F[12]->value());
		m_data.m_lamp_group[m_CurValue*4+3][YELLOW] = (byte)atoi(F[17]->value());
		m_data.m_lamp_group[m_CurValue*4+3][GREEN] = (byte)atoi(F[22]->value());
	}
	else if(m_CurValue*4+3<MAX_LAMP_GROUP_COUNT)
	{
		m_data.m_lamp_group[m_CurValue*4+3][RED] = 0;
		m_data.m_lamp_group[m_CurValue*4+3][YELLOW] = 0;
		m_data.m_lamp_group[m_CurValue*4+3][GREEN] = 0;		
	}
	return 0;
}
int CLampGroupSettingForm::doGet()
{
	// get data from mcu and set to ui		
	F[2]->set_value(itoa(m_data.m_lamp_group_count));
	m_LampGroupCount=m_data.m_lamp_group_count;
	
	F[9]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4][RED]));
	F[14]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4][YELLOW]));
	F[19]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4][GREEN]));
	
	if(m_CurValue*4+1<m_LampGroupCount)
	{
  	F[10]->options_on(O_VISIBLE);
  	F[15]->options_on(O_VISIBLE);
  	F[20]->options_on(O_VISIBLE);

		F[10]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+1][RED]));
		F[15]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+1][YELLOW]));
		F[20]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+1][GREEN]));
	}
	else
	{
  	F[10]->options_off(O_VISIBLE);
  	F[15]->options_off(O_VISIBLE);
  	F[20]->options_off(O_VISIBLE);
	}
	
	if(m_CurValue*4+2<m_LampGroupCount)
	{
  	F[11]->options_on(O_VISIBLE);
  	F[16]->options_on(O_VISIBLE);
  	F[21]->options_on(O_VISIBLE);
  	
		F[11]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+2][RED]));
		F[16]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+2][YELLOW]));
		F[21]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+2][GREEN]));
	}
	else
	{
  	F[11]->options_off(O_VISIBLE);
  	F[16]->options_off(O_VISIBLE);
  	F[21]->options_off(O_VISIBLE);
	}
	

	if(m_CurValue*4+3<m_LampGroupCount)
	{
  	F[12]->options_on(O_VISIBLE);
  	F[17]->options_on(O_VISIBLE);
  	F[22]->options_on(O_VISIBLE);

		F[12]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+3][RED]));
		F[17]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+3][YELLOW]));
		F[22]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+3][GREEN]));
	}
	else
	{
  	F[12]->options_off(O_VISIBLE);
  	F[17]->options_off(O_VISIBLE);
  	F[22]->options_off(O_VISIBLE);		
	}
	return 0;
}

int CLampGroupSettingForm::onPageUp()
{
	driver(REQ_FIRST_FIELD);
	doSet();
	if(m_CurValue > 0)
		m_CurValue--;
	setLableValue();
	return 0;
}
int CLampGroupSettingForm::onPageDown()
{
	driver(REQ_FIRST_FIELD);
	int nValue=0;
	doSet();
	if(m_data.m_lamp_group_count%4==0)
		 nValue=m_data.m_lamp_group_count/4-1;
	else
		nValue=m_data.m_lamp_group_count/4;
	if(m_CurValue < nValue)
		m_CurValue++;
	setLableValue();
	return 0;
}
void CLampGroupSettingForm::setLableValue(void)
{
	F[4]->set_value(itoa(m_CurValue*4+1));
	if(m_CurValue*4+2>m_data.m_lamp_group_count)
		F[5]->set_value(" ");
	else
		F[5]->set_value(itoa(m_CurValue*4+2));
	if(m_CurValue*4+3>m_data.m_lamp_group_count)
		F[6]->set_value(" ");
	else
		F[6]->set_value(itoa(m_CurValue*4+3));
	if(m_CurValue*4+4>m_data.m_lamp_group_count)
		F[7]->set_value(" ");
	else
		F[7]->set_value(itoa(m_CurValue*4+4));
	doGet();
}
int CLampGroupSettingForm::onFieldChanged(NCursesFormField& f)
{
		int   FiledValue = atoi(f.value());
		if(F[2] == &f)//灯组数
		{
				if(FiledValue < 1  || FiledValue > MAX_LAMP_GROUP_COUNT)
				{
						return -1;
				}
				m_LampGroupCount=FiledValue;
				m_data.m_lamp_group_count=FiledValue;
				m_CurValue=0;
				setLableValue();	
		}
		return 0;
}
