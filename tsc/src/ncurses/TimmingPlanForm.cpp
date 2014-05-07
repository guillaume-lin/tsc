/* @(#)TimmingPlanForm.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-19 17:55:00 (中国标准时间)
*/

#include "TimmingPlanForm.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"

static CIntegerChecker plan_count_ic(1,MAX_TIMMING_PLAN_COUNT);
static CIntegerChecker step_count_ic(1,MAX_STAGE_COUNT);
static CIntegerChecker timming_ic(0,255);

CTimmingPlanForm::CTimmingPlanForm()
	:CBaseForm(8,24,0,0)
{
	m_CurValue =0;
	m_Index = 0;
	F = new NCursesFormField*[26];
	F[0] = new CLabel("配时方案设置",0,0);
	
	F[1] = new CLabel("*方案号",1,0);
	F[2] = new CField(2,1,9);
	m_Monitor.set_form(this);
	F[2]->set_fieldtype(m_Monitor);

	F[3] = new CLabel("相位数",1,12);
	F[4] = new CField(1,1,18);
	m_Monitor.set_form(this);
	F[4]->set_fieldtype(m_Monitor);
//	F[4]->set_fieldtype(step_count_ic);

	F[5] = new CLabel("  ",2,0);
  F[6] = new CLabel("绿灯",2,2);
  F[7] = new CLabel("绿闪",2,7);
  F[8] = new CLabel("黄灯",2,12);
  F[9] = new CLabel("红灯",2,17);
   	
	F[10] = new CLabel("     ",3,0);
	F[11] = new CField(3,3,2);
	F[12] = new CField(3,3,7);
	F[13] = new CField(3,3,12);
	F[14] = new CField(3,3,17);

	F[15] = new CLabel("     ",4,0);
	F[16] = new CField(3,4,2);
	F[17] = new CField(3,4,7);
	F[18] = new CField(3,4,12);
	F[19] = new CField(3,4,17);

	F[20] = new CLabel("     ",5,0);
	F[21] = new CField(3,5,2);
	F[22] = new CField(3,5,7);
	F[23] = new CField(3,5,12);
	F[24] = new CField(3,5,17);

	for(int i=1; i<=4; i++){
		for(int j=0; j<4; j++)
			F[5*i+1+j]->set_fieldtype(timming_ic);
	}
	F[25] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	

	onLoad();
	setLableValue();

}
int CTimmingPlanForm::doSet()
{
	// get data from ui and send to mcu
	
	m_data[m_Index].m_step = (byte)atoi(F[4]->value());

	m_data[m_Index].m_greentimming[m_CurValue*3]  = (byte)atoi(F[11]->value());
	m_data[m_Index].m_greenflashtimming[m_CurValue*3]  = (byte)atoi(F[12]->value());
	m_data[m_Index].m_yellowtimming[m_CurValue*3]  = (byte)atoi(F[13]->value());
	m_data[m_Index].m_redtimming[m_CurValue*3]  = (byte)atoi(F[14]->value());

	if(m_CurValue*3+1<m_data[m_Index].m_step)
	{
		m_data[m_Index].m_greentimming[m_CurValue*3+1]  = (byte)atoi(F[16]->value());
		m_data[m_Index].m_greenflashtimming[m_CurValue*3+1]  = (byte)atoi(F[17]->value());
		m_data[m_Index].m_yellowtimming[m_CurValue*3+1] = (byte)atoi(F[18]->value());
		m_data[m_Index].m_redtimming[m_CurValue*3+1] = (byte)atoi(F[19]->value());
	}
	else if(m_CurValue*3+1<MAX_STAGE_COUNT)
	{
		m_data[m_Index].m_greentimming[m_CurValue*3+1]  =0;
		m_data[m_Index].m_greenflashtimming[m_CurValue*3+1]  = 0;
		m_data[m_Index].m_yellowtimming[m_CurValue*3+1] = 0;
		m_data[m_Index].m_redtimming[m_CurValue*3+1] = 0;		
	}
	if(m_CurValue*3+2<m_data[m_Index].m_step)
	{
		m_data[m_Index].m_greentimming[m_CurValue*3+2] = (byte)atoi(F[21]->value());
		m_data[m_Index].m_greenflashtimming[m_CurValue*3+2] = (byte)atoi(F[22]->value());
		m_data[m_Index].m_yellowtimming[m_CurValue*3+2] = (byte)atoi(F[23]->value());
		m_data[m_Index].m_redtimming[m_CurValue*3+2] = (byte)atoi(F[24]->value());
	}
	else if(m_CurValue*3+2<MAX_STAGE_COUNT)
	{
		m_data[m_Index].m_greentimming[m_CurValue*3+2]  =0;
		m_data[m_Index].m_greenflashtimming[m_CurValue*3+2]  = 0;
		m_data[m_Index].m_yellowtimming[m_CurValue*3+2] =0;
		m_data[m_Index].m_redtimming[m_CurValue*3+2] = 0;		
	}	
	return 0;
}
int CTimmingPlanForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	return vm_set_timming_plan(&g_vm,&m_data[m_Index],m_Index+1);
}
int CTimmingPlanForm::onLoad()
{
	vm_set_beep_option(&g_vm,1);
	return vm_get_timming_plan(&g_vm,m_Index+1,&m_data[m_Index]);
}
int CTimmingPlanForm::doGet()
{
	// get data from mcu and set to ui
	F[2]->set_value(itoa(m_Index+1));
	F[4]->set_value(itoa(m_data[m_Index].m_step));

	F[11]->set_value(itoa(m_data[m_Index].m_greentimming[m_CurValue*3]));
	F[12]->set_value(itoa(m_data[m_Index].m_greenflashtimming[m_CurValue*3]));
	F[13]->set_value(itoa(m_data[m_Index].m_yellowtimming[m_CurValue*3]));
	F[14]->set_value(itoa(m_data[m_Index].m_redtimming[m_CurValue*3]));
	
	if((m_CurValue*3+1<m_data[m_Index].m_step)&&(m_CurValue*3+1<MAX_STAGE_COUNT))
	{
  	F[16]->options_on(O_VISIBLE);
  	F[17]->options_on(O_VISIBLE);
  	F[18]->options_on(O_VISIBLE);
  	F[19]->options_on(O_VISIBLE);
		
		F[16]->set_value(itoa(m_data[m_Index].m_greentimming[m_CurValue*3+1]));
		F[17]->set_value(itoa(m_data[m_Index].m_greenflashtimming[m_CurValue*3+1]));
		F[18]->set_value(itoa(m_data[m_Index].m_yellowtimming[m_CurValue*3+1]));
		F[19]->set_value(itoa(m_data[m_Index].m_redtimming[m_CurValue*3+1]));
  }
  else
  {
  	F[16]->options_off(O_VISIBLE);
  	F[17]->options_off(O_VISIBLE);
  	F[18]->options_off(O_VISIBLE);
  	F[19]->options_off(O_VISIBLE);
	}
	if(m_CurValue*3+2<m_data[m_Index].m_step&&(m_CurValue*3+2<MAX_STAGE_COUNT))
	{
  	F[21]->options_on(O_VISIBLE);
  	F[22]->options_on(O_VISIBLE);
  	F[23]->options_on(O_VISIBLE);
  	F[24]->options_on(O_VISIBLE);

		F[21]->set_value(itoa(m_data[m_Index].m_greentimming[m_CurValue*3+2]));
		F[22]->set_value(itoa(m_data[m_Index].m_greenflashtimming[m_CurValue*3+2]));
		F[23]->set_value(itoa(m_data[m_Index].m_yellowtimming[m_CurValue*3+2]));
		F[24]->set_value(itoa(m_data[m_Index].m_redtimming[m_CurValue*3+2]));
	}
	else
	{
  	F[21]->options_off(O_VISIBLE);
  	F[22]->options_off(O_VISIBLE);
  	F[23]->options_off(O_VISIBLE);
  	F[24]->options_off(O_VISIBLE);		
	}
	return 0;
}

int CTimmingPlanForm::onPageUp()
{
	driver(REQ_FIRST_FIELD);
	doSet();
	if(m_CurValue > 0)
		m_CurValue--;
	setLableValue();
	return 0;
}
int CTimmingPlanForm::onPageDown()
{	
	driver(REQ_FIRST_FIELD);
	int nValue;
	doSet();
	if(m_data[m_Index].m_step%3==0)
		 nValue=m_data[m_Index].m_step/3-1;
	else
		nValue=m_data[m_Index].m_step/3;
	if(m_CurValue < nValue)
		m_CurValue++;
	setLableValue();
	return 0;
}
void  CTimmingPlanForm::setLableValue(void)
{
	char buf[5];
	int nCurValue=m_CurValue*3;
	sprintf(buf,"%d",nCurValue+1);
	F[10]->set_value(buf);
	if(nCurValue+2>m_data[m_Index].m_step)
	{
		sprintf(buf," ");
	}
	else
	{
		sprintf(buf,"%d",nCurValue+2);
  }
	F[15]->set_value(buf);
	if(nCurValue+3>m_data[m_Index].m_step)
	{
		sprintf(buf," ");
	}
	else
	{  
		sprintf(buf,"%d",nCurValue+3);
	}
	F[20]->set_value(buf);
	doGet();
}
int CTimmingPlanForm::onFieldChanged(NCursesFormField& f)
{
	int   FiledValue = atoi(f.value());
	if(F[2]==&f)
	{
			if(FiledValue<1  || FiledValue > MAX_TIMMING_PLAN_COUNT)
			{
				f.set_value(itoa(m_Index+1));
				//f.set_changed(false);
				return -1;
			}
			m_Index = FiledValue-1;
			m_CurValue=0;
			// load new timming plan
			onLoad();
			setLableValue();
	}
	else if(F[4]==&f)
	{
			if(FiledValue<1  || FiledValue > MAX_STAGE_COUNT)
			{
					f.set_value(itoa(m_data[m_Index].m_step));
					return -1;
			}
			m_data[m_Index].m_step=FiledValue;
			m_CurValue=0;
			setLableValue();	
	}
	return 0;
}
