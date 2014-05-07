/* @(#)DetectorSettingForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 15:40:00 (中国标准时间)
*/

#include "DetectorSettingForm.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"

CIntegerChecker dist_ic(1,999);
CIntegerChecker zero_one_ic(0,1);
CIntegerChecker sense_ic(0,3);
CIntegerChecker group_ic(1,MAX_LAMP_GROUP_COUNT);

CDetectorSettingForm::CDetectorSettingForm()
	:CBaseForm(8,24,0,0)
{
	m_CurValue =0;
	F = new NCursesFormField*[29];
	F[0] = new CLabel("检测器",0,0);
	
	F[1] = new CLabel("上下游距离:",0,7);
	F[2] = new CField(3,0,18);
	F[2]->set_fieldtype(dist_ic);

	F[3] = new CLabel("编号:",1,0);
	F[4] = new CLabel("  ",1,7);
	F[5] = new CLabel("  ",1,11);
	F[6] = new CLabel("  ",1,15);
	F[7] = new CLabel("  ",1,19);

	F[8] = new CLabel("启用:",2,0);
	F[9] = new CField(1,2,7);
	m_Monitor.set_form(this);
	F[9]->set_fieldtype(m_Monitor);
	F[10] = new CField(1,2,11);
	F[10]->set_fieldtype(m_Monitor);	
	F[11] = new CField(1,2,15);
	F[11]->set_fieldtype(m_Monitor);
	F[12] = new CField(1,2,19);
	F[12]->set_fieldtype(m_Monitor);
	
	F[13] = new CLabel("位置:",3,0);
	F[14] = new CField(1,3,7);
	F[14]->set_fieldtype(zero_one_ic);
	F[15] = new CField(1,3,11);
	F[15]->set_fieldtype(zero_one_ic);
	F[16] = new CField(1,3,15);
	F[16]->set_fieldtype(zero_one_ic);
	F[17] = new CField(1,3,19);
	F[17]->set_fieldtype(zero_one_ic);

	F[18] = new CLabel("灵敏度:",4,0);
	F[19] = new CField(1,4,7);
	F[19]->set_fieldtype(sense_ic);
	F[20] = new CField(1,4,11);
	F[20]->set_fieldtype(sense_ic);
	F[21] = new CField(1,4,15);
	F[21]->set_fieldtype(sense_ic);
	F[22] = new CField(1,4,19);
	F[22]->set_fieldtype(sense_ic);

	F[23] = new CLabel("灯组:",5,0);
	F[24] = new CField(2,5,7);
	F[24]->set_fieldtype(group_ic);
	F[25] = new CField(2,5,11);
	F[25]->set_fieldtype(group_ic);
	F[26] = new CField(2,5,15);
	F[26]->set_fieldtype(group_ic);
	F[27] = new CField(2,5,19);
	F[27]->set_fieldtype(group_ic);

	
	F[28] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	//doGet();
	onLoad();
	setLableValue();
}
int CDetectorSettingForm::onLoad()
{
	vm_set_beep_option(&g_vm,1);
	return vm_get_detector_setting(&g_vm,&m_data);
}
int CDetectorSettingForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	doGet();
	return vm_set_detector_setting(&g_vm,&m_data);
}
int CDetectorSettingForm::doSet()
{
	// get data from ui and send to mcu
	
	m_data.m_distance = (byte)atoi(F[2]->value());

	m_data.m_attribute[m_CurValue*4] = SetValue(9);
	if(m_data.m_attribute[m_CurValue*4]==0)
		m_data.m_lamp_group[m_CurValue*4]=0;
	else
		m_data.m_lamp_group[m_CurValue*4] = (byte)atoi(F[24]->value());
		
	m_data.m_attribute[m_CurValue*4+1] = SetValue(10);
	if(m_data.m_attribute[m_CurValue*4+1]==0)
		m_data.m_lamp_group[m_CurValue*4+1]=0;
	else	
		m_data.m_lamp_group[m_CurValue*4+1] = (byte)atoi(F[25]->value());
	
	m_data.m_attribute[m_CurValue*4+2] = SetValue(11);
	if(m_data.m_attribute[m_CurValue*4+2]==0)
		m_data.m_lamp_group[m_CurValue*4+2]=0;		
	else
		m_data.m_lamp_group[m_CurValue*4+2] = (byte)atoi(F[26]->value());
		
	m_data.m_attribute[m_CurValue*4+3] = SetValue(12);
	if(m_data.m_attribute[m_CurValue*4+3]==0)
		m_data.m_lamp_group[m_CurValue*4+3]=0;		
	else
		m_data.m_lamp_group[m_CurValue*4+3] = (byte)atoi(F[27]->value());

	return 0;
}
int CDetectorSettingForm::doGet()
{
	// get data from mcu and set to ui
	F[2]->set_value(itoa(m_data.m_distance));

	GetValue(m_data.m_attribute[m_CurValue*4],9);
	F[24]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4]));
	if(m_data.m_lamp_group[m_CurValue*4]==0)
			m_data.m_attribute[m_CurValue*4]=0;
	GetValue(m_data.m_attribute[m_CurValue*4],9);	
			
	F[25]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+1]));
	if(m_data.m_lamp_group[m_CurValue*4+1]==0)
			m_data.m_attribute[m_CurValue*4+1]=0;
	GetValue(m_data.m_attribute[m_CurValue*4+1],10);	
	
	
	F[26]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+2]));
	if(m_data.m_lamp_group[m_CurValue*4+2]==0)
			m_data.m_attribute[m_CurValue*4+2]=0;
	GetValue(m_data.m_attribute[m_CurValue*4+2],11);	

	F[27]->set_value(itoa(m_data.m_lamp_group[m_CurValue*4+3]));
	if(m_data.m_lamp_group[m_CurValue*4+3]==0)
			m_data.m_attribute[m_CurValue*4+3]=0;
	GetValue(m_data.m_attribute[m_CurValue*4+3],12);	
	
	return 0;
}

int CDetectorSettingForm::onPageUp()
{
	doSet();
	if(m_CurValue > 0)
		m_CurValue--;
	setLableValue();
	return 0;
}
int CDetectorSettingForm::onPageDown()
{
	doSet();
	if(m_CurValue < (MAX_DETECTOR_COUNT/4)-1)
		m_CurValue++;
	setLableValue();
	return 0;
}
void     CDetectorSettingForm::setLableValue(void)
{
	F[4]->set_value(itoa(m_CurValue*4+1));
	F[5]->set_value(itoa(m_CurValue*4+2));
	F[6]->set_value(itoa(m_CurValue*4+3));
	F[7]->set_value(itoa(m_CurValue*4+4));
	doGet();
}
int CDetectorSettingForm::get_enable(byte attr)
{
	int v = attr & 0x40;
	if(v != 0)
		return 1;
	else
		return 0;

}
void CDetectorSettingForm::set_enable(byte& attr,int v)
{
	attr &=  ~0x40;
	if(v != 0)
		attr |= 0x40;
}
int CDetectorSettingForm::get_far(byte attr)
{
	int v = attr & 0x80;
	if(v != 0)
		return 1;
	else
		return 0;

}
void CDetectorSettingForm::set_far(byte& attr,int v)
{
	attr &=  ~0x80;
	if( v != 0)
		attr |= 0x80;
}
int CDetectorSettingForm::get_sensitivity(byte attr)
{
	int v = attr & 0x03;
	return v;
}
void CDetectorSettingForm::set_sensitivity(byte& attr,int v)
{
	attr &= ~0x03;
	attr |= (v & 0x03);
}
byte  CDetectorSettingForm::SetValue(int index)
{
	byte attr;	
	attr = 0;
	int v = atoi(F[index]->value());
	if(v==0)//对应的线圈为关闭状态，则清空属性
		return attr;
	set_enable(attr,v);
	v = atoi(F[index+5]->value());
	set_far(attr,v);
	v = atoi(F[index+10]->value());
	if(v==0)//对应的灯组为零，则清空属性
		return 0;
	set_sensitivity(attr,v);
	return attr;
}
void  CDetectorSettingForm::GetValue(byte attr,int index)
{	
	int v=get_enable(attr);
	F[index]->set_value(itoa(v));
	if(v==0)
	{
		F[index+5]->set_value(itoa(0));
		F[index+10]->set_value(itoa(0));
		F[index+15]->set_value(itoa(0));
		return;
	}
	F[index+5]->set_value(itoa(get_far(attr)));
	F[index+10]->set_value(itoa(get_sensitivity(attr)));
}
int CDetectorSettingForm::onFieldChanged(NCursesFormField& f)
{
		int   FiledValue = atoi(f.value());
		if(F[9] == &f)
		{
				if(FiledValue >1  || FiledValue <0 )
				{
						f.set_value(itoa(0));
						return -1;
				}
				if(FiledValue==0)
				{
					F[14]->set_value("0");
					F[19]->set_value("0");
					F[24]->set_value("0");
				}
		}
		else if(F[10]==&f)
		{
				if(FiledValue >1  || FiledValue <0 )
				{
						f.set_value(itoa(0));
						return -1;
				}
				if(FiledValue==0)
				{
					F[15]->set_value("0");
					F[20]->set_value("0");
					F[25]->set_value("0");
				}
		}
		else if(F[11]==&f)
		{
				if(FiledValue >1  || FiledValue <0 )
				{
						f.set_value(itoa(0));
						return -1;
				}
				if(FiledValue==0)
				{
					F[16]->set_value("0");
					F[21]->set_value("0");
					F[26]->set_value("0");
				}
		}
		else if(F[12]==&f)
		{
				if(FiledValue >1  || FiledValue <0 )
				{
						f.set_value(itoa(0));
						return -1;
				}
				if(FiledValue==0)
				{
					F[17]->set_value("0");
					F[22]->set_value("0");
					F[27]->set_value("0");
				}
		}
		return 0;
}
