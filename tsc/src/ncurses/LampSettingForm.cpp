/* @(#)LampSettingForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-31 14:55:00 (中国标准时间)
*/

#include "key_translate.h"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"
#include "messagebox.hpp"
#include "LampSettingForm.hpp"
CIntegerChecker lamp_ic(0,1);
CLampSettingForm::CLampSettingForm()
	:CBaseForm(8,24,0,0)
{
	m_CurValue =0;

	F = new NCursesFormField*[32];
	F[0] = new CLabel("信号灯启用设置",0,3);

	F[1] = new CLabel("端子编号",1,0);
	F[2] = new CLabel("1启用 0禁用",1,10);	

	F[3] =  new CLabel("01-06",2,0);	
	F[4] =  new CField(1,2,7);
	F[4]->set_fieldtype(lamp_ic);
	F[5] = new CField(1,2,9);
	F[5]->set_fieldtype(lamp_ic);
	F[6] = new CField(1,2,11);
	F[6]->set_fieldtype(lamp_ic);
	F[7] = new CField(1,2,13);
	F[7]->set_fieldtype(lamp_ic);
	F[8] = new CField(1,2,15);
	F[8]->set_fieldtype(lamp_ic);
	F[9] = new CField(1,2,17);
	F[9]->set_fieldtype(lamp_ic);

	F[10] = new CLabel("07-12",3,0);
	F[11] = new CField(1,3,7);
	F[11]->set_fieldtype(lamp_ic);
	F[12] = new CField(1,3,9);
	F[12]->set_fieldtype(lamp_ic);
	F[13] = new CField(1,3,11);
	F[13]->set_fieldtype(lamp_ic);
	F[14] = new CField(1,3,13);
	F[14]->set_fieldtype(lamp_ic);
	F[15] = new CField(1,3,15);
	F[15]->set_fieldtype(lamp_ic);
	F[16] = new CField(1,3,17);
	F[16]->set_fieldtype(lamp_ic);

	F[17] = new CLabel("13-18",4,0);
	F[18] = new CField(1,4,7);
	F[18]->set_fieldtype(lamp_ic);
	F[19] = new CField(1,4,9);
	F[19]->set_fieldtype(lamp_ic);
	F[20] = new CField(1,4,11);
	F[20]->set_fieldtype(lamp_ic);
	F[21] = new CField(1,4,13);
	F[21]->set_fieldtype(lamp_ic);
	F[22] = new CField(1,4,15);
	F[22]->set_fieldtype(lamp_ic);
	F[23] = new CField(1,4,17);
	F[23]->set_fieldtype(lamp_ic);

	F[24] = new CLabel("19-24",5,0);
	F[25] = new CField(1,5,7);
	F[25]->set_fieldtype(lamp_ic);
	F[26] = new CField(1,5,9);
	F[26]->set_fieldtype(lamp_ic);
	F[27] = new CField(1,5,11);
	F[27]->set_fieldtype(lamp_ic);
	F[28] = new CField(1,5,13);
	F[28]->set_fieldtype(lamp_ic);	
	F[29] = new CField(1,5,15);
	F[29]->set_fieldtype(lamp_ic);
	F[30] = new CField(1,5,17);
	F[30]->set_fieldtype(lamp_ic);

	F[31] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	setLableValue();
}

CLampSettingForm::~CLampSettingForm()
{
}
/**
* bytearray 指向字节数组的指针
* num 位值的编号
* base 组成位值的位的个数
*/
int CLampSettingForm::get_bit_value(unsigned char* bytearray,int num, int base)
{
	int b;
	unsigned char off;
	unsigned char mask;

	b = (num - 1) / (8 / base);
	off = (num - 1) % ( 8 / base);
	mask = 0xff << (8 - base);

	return (bytearray[b] & (mask >> off*base)) >> ( 8 - base*(off+1) );
}
/**
* bytearray 指向字节数组的指针
* num 位值的编号
* base 组成位值的位的个数
* value 要设置的值
*/
int CLampSettingForm::set_bit_value(unsigned char* bytearray,int num, int base, int value)
{
	int b;
	unsigned char off;
	unsigned char mask;

	b = (num - 1) / (8 / base);
	off = (num - 1) % ( 8 / base);
	mask = 0xff << (8 - base);

	bytearray[b] &= ~ (mask >> off*base);	// 清空
	bytearray[b] |= (value << (8 - base)) >> off*base;			// 设置
	return 0;
}
int CLampSettingForm::onLoad()
{
	vm_set_beep_option(&g_vm,0);
	if(vm_get_lamp_setting(&g_vm,&m_lampsetting) != 0)
	{
		CMessageBox box("载入信号灯信息失败");
		box();
		postKey(CTRL('X'));
		return -1;
	}
	return 0;
}

/**
 * int vm_set_phase_plan(vm_t* self,int planNo,int step_count,int step,phase_t* phase);
 *
 */
int CLampSettingForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	return vm_set_lamp_setting(&g_vm,&m_lampsetting);
}

int CLampSettingForm::doSet()
{
	// get data from ui and send to mcu
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+1, 1, atoi(F[4]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+2, 1, atoi(F[5]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+3, 1, atoi(F[6]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+4, 1, atoi(F[7]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+5, 1, atoi(F[8]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+6, 1, atoi(F[9]->value()));
	
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+7, 1, atoi(F[11]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+8, 1, atoi(F[12]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+9, 1, atoi(F[13]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+10, 1, atoi(F[14]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+11, 1, atoi(F[15]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+12, 1, atoi(F[16]->value()));

	set_bit_value(m_lampsetting.m_use, m_CurValue*24+13, 1, atoi(F[18]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+14, 1, atoi(F[19]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+15, 1, atoi(F[20]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+16, 1, atoi(F[21]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+17, 1, atoi(F[22]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+18, 1, atoi(F[23]->value()));

	set_bit_value(m_lampsetting.m_use, m_CurValue*24+19, 1, atoi(F[25]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+20, 1, atoi(F[26]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+21, 1, atoi(F[27]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+22, 1, atoi(F[28]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+23, 1, atoi(F[29]->value()));
	set_bit_value(m_lampsetting.m_use, m_CurValue*24+24, 1, atoi(F[30]->value()));	
	return 0;
}
int CLampSettingForm::doGet()
{
	// get data from mcu and set to ui
	F[4]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+1, 1)));
	F[5]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+2, 1)));
	F[6]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+3, 1)));
	F[7]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+4, 1)));
	F[8]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+5, 1)));
	F[9]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+6, 1)));

	F[11]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+7, 1)));
	F[12]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+8, 1)));
	F[13]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+9, 1)));
	F[14]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+10, 1)));
	F[15]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+11, 1)));
	F[16]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+12, 1)));
	
	F[18]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+13, 1)));
	F[19]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+14, 1)));
	F[20]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+15, 1)));
	F[21]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+16, 1)));
	F[22]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+17, 1)));
	F[23]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+18, 1)));

	F[25]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+19, 1)));
	F[26]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+20, 1)));
	F[27]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+21, 1)));
	F[28]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+22, 1)));
	F[29]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+23, 1)));
	F[30]->set_value(itoa(get_bit_value(m_lampsetting.m_use, m_CurValue*24+24, 1)));
	return 0;
}

int CLampSettingForm::onPageUp()
{
	doSet();
	if(m_CurValue > 0)
		m_CurValue--;
	setLableValue();
	return 0;
}
int CLampSettingForm::onPageDown()
{
	doSet();
	if(m_CurValue < MAX_TERMINAL_COUNT/24-1)
		m_CurValue++;
	setLableValue();
	return 0;
}
void     CLampSettingForm::setLableValue(void)
{
	char buf[5];
	sprintf(buf,"%02d-%02d",m_CurValue*24+1,m_CurValue*24+6);
	F[3]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*24+7,m_CurValue*24+12);
	F[10]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*24+13,m_CurValue*24+18);
	F[17]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*24+19,m_CurValue*24+24);
	F[24]->set_value(buf);
	doGet();
}

