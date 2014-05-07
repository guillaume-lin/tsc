/* @(#)PhasePlanForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-19 19:55:00 (中国标准时间)
*/

#include "PhasePlanForm.hpp"
#include "key_translate.h"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"
#include "messagebox.hpp"

CIntegerChecker plan_ic(1,MAX_PHASE_PLAN_COUNT);
CIntegerChecker step_count_ic(1,MAX_STAGE_COUNT);
static CIntegerChecker phase_ic(0,MAX_LAMP_GROUP_COUNT);

CPhasePlanForm::CPhasePlanForm()
	:CBaseForm(8,24,0,0)
{
	m_CurValue =0;
	m_Index = 0;
	m_IndexPhase =0;

	F = new NCursesFormField*[24];
	F[0] = new CLabel("相位方案",0,0);
	
	F[1] = new CLabel("*方案号",0,10);
	F[2] = new CField(1,0,18);
	m_Monitor.set_form(this);
	F[2]->set_fieldtype(m_Monitor);

	F[3] = new CLabel("相位数",1,0);
	F[4] = new CField(1,1,7);
	F[4]->set_fieldtype(m_Monitor);

	F[5] = new CLabel("*当前相位",1,10);
	F[6] = new CField(1,1,19);
	m_Monitor.set_form(this);
	F[6]->set_fieldtype(m_Monitor);

	F[7] = new CLabel("     ",2,0);
	F[8] = new CField(2,2,10);
	F[9] = new CField(2,2,14);
	F[10] = new CField(2,2,18);
	
	F[11] = new CLabel("     ",3,0);
	F[12] = new CField(2,3,10);
	F[13] = new CField(2,3,14);
	F[14] = new CField(2,3,18);

	F[15] = new CLabel("     ",4,0);
	F[16] = new CField(2,4,10);
	F[17] = new CField(2,4,14);
	F[18] = new CField(2,4,18);

	F[19] = new CLabel("     ",5,0);
	F[20] = new CField(2,5,10);
	F[21] = new CField(2,5,14);
	F[22] = new CField(2,5,18);

	for(int i=0; i<4; i++){
		for(int j=0; j<3; j++)
			F[8+4*i+j]->set_fieldtype(phase_ic);
	}
	F[23] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	//keypad(TRUE);
	m_stage_count=0;
	setLableValue();
}
CPhasePlanForm::~CPhasePlanForm()
{
}
byte CPhasePlanForm::set_bits(int n, int v,byte phase)
{
	int off = n%4;
	unsigned char mask = 0x03 << (3 - off)*2;
	v = v << (3 - off)*2;
	unsigned char c = phase & ~mask;
	return (byte) c | v;

	//return 0;
}
 int CPhasePlanForm::get_bits(int n,byte phase)
{
	int off = n % 4;
	return 0x03 & (phase >> (3 - off)*2);

}
byte  CPhasePlanForm::SetValue(int index1,int index2,int index3,int index4)
{
	byte attr;	
	attr = 0;
	int v = atoi(F[index1]->value());
	attr = set_bits(0,v,attr);
	v = atoi(F[index2]->value());
	attr = set_bits(1,v,attr);
	v = atoi(F[index3]->value());
	attr = set_bits(2,v,attr);
	v = atoi(F[index4]->value());
	attr = set_bits(3,v,attr);
	return attr;
}
void  CPhasePlanForm::GetValue(byte attr,int index1,int index2,int index3,int index4)
{	
	//get_bits();	
	F[index1]->set_value(itoa(get_bits(0,attr)));
	F[index2]->set_value(itoa(get_bits(1,attr)));
	F[index3]->set_value(itoa(get_bits(2,attr)));
	F[index4]->set_value(itoa(get_bits(3,attr)));
}
/**
 * vm_get_phase_plan(vm_t* self,int plan_no,int step, int* step_count, phase_t* phase);
 *
 */
int CPhasePlanForm::onLoad()
{
	vm_set_beep_option(&g_vm,0);
	if(vm_get_green_conflict(&g_vm,&m_conflict) != 0){
		CMessageBox box("载入绿冲突表失败");
		box();
		postKey(CTRL('X'));
		return -1;
	}
	vm_set_beep_option(&g_vm,1);
	return vm_get_phase_plan(&g_vm,m_Index+1,m_IndexPhase+1,
				 (int*)&m_data[m_Index].m_stage_count,
				 &m_data[m_Index].m_stage[m_IndexPhase]);
}

/**
 * int vm_set_phase_plan(vm_t* self,int planNo,int step_count,int step,phase_t* phase);
 *
 */
int CPhasePlanForm::onSave()
{
	int i,j;
	stage_t* stage;
	doSet();
	stage = &m_data[m_Index].m_stage[m_IndexPhase];

	for(i=0; i<MAX_PHASE_COUNT; i++){
		for(j=0; j<MAX_PHASE_COUNT; j++){
				if(green_conflict_is_conflict(&m_conflict,stage->m_phase[i],stage->m_phase[j])){
					char buf[100];
					sprintf(buf,"相位%d , %d 有绿冲突",i+1,j+1);
					CMessageBox box(buf);
					box();
					return 0;
				}
		}
	}
	m_stage_count=0;
	vm_set_beep_option(&g_vm,1);
	return vm_set_phase_plan(&g_vm,m_Index+1,
				 m_IndexPhase+1,
				 m_data[m_Index].m_stage_count,
				 &m_data[m_Index].m_stage[m_IndexPhase]);
}

int CPhasePlanForm::doSet()
{
	// get data from ui and send to mcu
	
	m_data[m_Index].m_stage_count = (byte)atoi(F[4]->value());
	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12]  = (byte)atoi(F[8]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+1]  = (byte)atoi(F[9]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+2]  = (byte)atoi(F[10]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+3]  = (byte)atoi(F[12]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+4]  = (byte)atoi(F[13]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+5]  = (byte)atoi(F[14]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+6]  = (byte)atoi(F[16]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+7]  = (byte)atoi(F[17]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+8]  = (byte)atoi(F[18]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+9]  = (byte)atoi(F[20]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+10]  = (byte)atoi(F[21]->value());	
	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+11]  = (byte)atoi(F[22]->value());	
//	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*3]    = /*(byte)atoi(*/SetValue(8,9,10,12);
//	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*3+1]  = /*(byte)atoi(*/SetValue(13,14,16,17);
//	m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*3+2]  = /*(byte)atoi(*/SetValue(18,20,21,22);
/*
	m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+3]  = (byte)atoi(F[12]->value());
	m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+4]  = (byte)atoi(F[13]->value());
	m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+5]  = (byte)atoi(F[14]->value());

	m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+6]  = (byte)atoi(F[16]->value());
	m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+7]  = (byte)atoi(F[17]->value());
	m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+8]  = (byte)atoi(F[18]->value());

	m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+9]  = (byte)atoi(F[20]->value());
	m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+10] = (byte)atoi(F[21]->value());
	m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+11] = (byte)atoi(F[22]->value());
*/
	
	return 0;
}
int CPhasePlanForm::doGet()
{
	// get data from mcu and set to ui
	if(m_stage_count!=0)
		m_data[m_Index].m_stage_count=m_stage_count;
	F[2]->set_value(itoa(m_Index+1));
	F[4]->set_value(itoa(m_data[m_Index].m_stage_count));
	F[6]->set_value(itoa(m_IndexPhase+1));
	
	F[8]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12]));
	F[9]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+1]));
	F[10]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+2]));
	F[12]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+3]));
	F[13]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+4]));
	F[14]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+5]));
	F[16]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+6]));
	F[17]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+7]));
	F[18]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+8]));
	F[20]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+9]));
	F[21]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+10]));
	F[22]->set_value(itoa(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*12+11]));
		
//	GetValue(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*3],8,9,10,12);
//	GetValue(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*3+1],13,14,16,17);
//	GetValue(m_data[m_Index].m_stage[m_IndexPhase].m_phase[m_CurValue*3+2],18,20,21,22);
/*
	F[8]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12]));
	F[9]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+1]));
	F[10]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+2]));

	F[12]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+3]));
	F[13]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+4]));
	F[14]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+5]));

	F[16]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+6]));
	F[17]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+7]));
	F[18]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+8]));

	F[20]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+9]));
	F[21]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+10]));
	F[22]->set_value(itoa(m_data[m_Index].m_phase[m_IndexPhase].m_output[m_CurValue*12+11]));	
*/
	return 0;
}

int CPhasePlanForm::onPageUp()
{
	doSet();
	if(m_CurValue > 0)
		m_CurValue--;
	setLableValue();
	return 0;
}
int CPhasePlanForm::onPageDown()
{
	doSet();
	if(m_CurValue < 0/*(MAX_TERMINAL_COUNT/12)-1*/)
		m_CurValue++;
	setLableValue();
	return 0;
}
void     CPhasePlanForm::setLableValue(void)
{
	char buf[5];
	sprintf(buf,"%02d-%02d",m_CurValue*12+1,m_CurValue*12+3);
	F[7]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*12+4,m_CurValue*12+6);
	F[11]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*12+7,m_CurValue*12+9);
	F[15]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*12+10,m_CurValue*12+12);
	F[19]->set_value(buf);
	doGet();
}
int CPhasePlanForm::onFieldChanged(NCursesFormField& f)
{
	int   FiledValue = atoi(f.value());
	if(F[2] == &f)//方案数
	{
		if(FiledValue < 1  || FiledValue > MAX_PHASE_PLAN_COUNT)
		{
			f.set_value(itoa(m_Index+1));
			return -1;
		}
		m_Index = FiledValue-1;
		m_IndexPhase = 0;
		m_stage_count=0;
	}
	else if(F[4]==&f)//阶段总数
	{
		if(FiledValue < 1  || FiledValue > MAX_STAGE_COUNT)
		{
			f.set_value(itoa(m_data[m_Index].m_stage_count));
			return -1;
		}
		m_stage_count=FiledValue;
		m_data[m_Index].m_stage_count=m_stage_count;
		if(m_IndexPhase<FiledValue)//当前阶段数小于总阶段数
				return 0;
		F[6]->set_value(itoa(1));	
		m_IndexPhase=0;
	}
	else if(F[6]==&f)//当前阶段数
	{
		if(FiledValue < 1  || FiledValue > m_data[m_Index].m_stage_count)
		{
			f.set_value(itoa(m_IndexPhase+1));
			return -1;
		}
		m_IndexPhase = FiledValue-1;		
	}
	m_CurValue=0;
	onLoad();
	setLableValue();
	return 0;
}
