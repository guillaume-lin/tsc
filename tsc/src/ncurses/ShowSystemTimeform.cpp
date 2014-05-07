/* @(#)ShowSystemTimeform.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(����)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-20 14:35:00 (�й���׼ʱ��)
*/
#include <time.h>
#include <stdlib.h>
#include "Log.hpp"
#include "key_translate.h"
#include "ShowSystemTimeform.hpp"
#include "label.hpp"
#include "field.hpp"
#include "../control/error_code.h"
//#include "crossingstatusform.hpp"

CShowSystemTimeform::CShowSystemTimeform()
	:CBaseForm(8,26,0,0)
{
	mIsExitPressed = false;
	m_ICount = 0;
	m_CurValue =0;

	F = new NCursesFormField*[8];

	F[0] = new CLabel("ϵͳ������Ϣ",0,2);

	for(int i=0; i<5; i++){
		F[i+1] = new CLabel("12345678901234567890xx",i+1,0);
	}
	F[6] = new CField(1,0,16);
	m_Monitor.set_form(this);
	F[6]->set_fieldtype(m_Monitor);


	F[7] = new NCursesFormField();

	InitForm(F,TRUE,TRUE);
	memset(&m_data,0,sizeof(m_data));
	std::string strPath="/home/ljx/";
	m_ErrorInfo.SetPath(strPath);
	m_ErrorInfo.Load();
	doGet();

}
int CShowSystemTimeform::doSet()
{
	return 0;
}
char* int2weekday(int w)
{
	char* wd[] = {
		"������",
		"����һ",
		"���ڶ�",
		"������",
		"������",
		"������",
		"������",
	};

	if(w < 0 || w > 6)
		return "";
	else
		return wd[w];
}
int CShowSystemTimeform::doGet()
{
	// get data from mcu and set to ui

	if(vm_get_machine_status(&g_vm,&m_data) != 0)
		;//return -1;
	char   buf[30];
	if(GetErrorInfo(m_data.m_error.m_soft_error)==0)
	{
		sprintf(buf,"����:  %02d%02d��%02d��%02d��",m_data.m_year[0],m_data.m_year[1],m_data.m_month,m_data.m_day);
		m_Value[0] = buf;
		sprintf(buf,"ʱ��:    %02dʱ%02d��%02d��",m_data.m_hour,m_data.m_minute,m_data.m_second);
		m_Value[1] = buf;
		sprintf(buf,"%s     �¶�:%02d.%d",int2weekday(m_data.m_week),m_data.m_temperature_integer,m_data.m_temperature_decimal);
		m_Value[2] = buf;
	}
	sprintf(buf,"Ԥ��ģʽ%02d ����ģʽ%02d",m_data.m_designate_control_mode,m_data.m_actual_control_mode);
	m_Value[3] = buf;
	sprintf(buf,"��ǰ��λ%02d ��ǰ����%02d",m_data.m_stage,m_data.m_step);
	m_Value[4] = buf;


	sprintf(buf,"ʱ�η���%02d ��ʱ����%02d",m_data.m_schedule,m_data.m_timming);
	m_Value[5] = buf;
	sprintf(buf,"��λ����%03d��ǰ��λ%02d",m_data.m_phase,m_data.m_stage);
	int lamp_count = 0;
	for(int i=0; i<MAX_TERMINAL_COUNT; i++)
		if(lamp_health_get_health(&m_data.m_lamp_health,i+1))
			lamp_count++;
	m_Value[6] = buf;
	sprintf(buf,"�趨ʱ��%03d�ƹ�����%02d",m_data.m_assign_time,lamp_count);
	int detector_count = 0;
	for(int i=0; i<MAX_DETECTOR_COUNT; i++)
		if(detector_health_get_health(&m_data.m_detector_health,i+1))
			detector_count++;
	m_Value[7] = buf;
	sprintf(buf,"����ʱ��%03d������%02d",m_data.m_elapse_time,detector_count);
	m_Value[8] = buf;

	sprintf(buf,"ʣ��ʱ��%03d������%02x%02x",m_data.m_remain_time,
		m_data.m_error.m_hard_error,
		m_data.m_error.m_soft_error);
	m_Value[9] = buf;

	F[1]->set_value(m_Value[m_CurValue*5].c_str());
	F[2]->set_value(m_Value[m_CurValue*5+1].c_str());
	F[3]->set_value(m_Value[m_CurValue*5+2].c_str());
	F[4]->set_value(m_Value[m_CurValue*5+3].c_str());
	F[5]->set_value(m_Value[m_CurValue*5+4].c_str());
	m_ErrorInfo.Update(&m_data);
	return 0;
}
int CShowSystemTimeform::onPageUp()
{
//	doSet();
	if(m_CurValue > 0)
		m_CurValue--;
//	setLableValue();
	doGet();
	return 0;
}
int CShowSystemTimeform::onPageDown()
{
//	doSet();
	if(m_CurValue < 1)
		m_CurValue++;
//	setLableValue();
	doGet();
	return 0;
}
int CShowSystemTimeform::virtualize(int c)
{
	if(c == CTRL('D') || c == '1'){
//		CCrossingStatusForm csf;
//		csf();
	}
	else if(c==KEY_ENTER||c==CTRL('W'))
	{
		return NCursesForm::virtualize(CTRL('X'));
	}
	else if(c==KEY_EXIT||c==CTRL('X'))
	{
		mIsExitPressed = true;
		return NCursesForm::virtualize(CTRL('X'));
	}
	doGet();
	return CBaseForm::virtualize(c);
}
int CShowSystemTimeform::onFieldChanged(NCursesFormField& f)
{
	f.set_value("");
	return 0;
}
int CShowSystemTimeform::GetErrorInfo(byte soft_error)
{
	int nErrorCount=0;
	m_Value[0]="";
	m_Value[1]="";
	m_Value[2]="";	
	if(soft_error & INVALID_PHASE)
	{
		m_Value[nErrorCount]="   �Ƿ�����λ����";
		nErrorCount++;
	}
	if(soft_error & INVALID_TIMMING)	
	{
		m_Value[nErrorCount]="   �Ƿ�����ʱ����";
		nErrorCount++;	
	}
	if(soft_error& INVALID_SCHEDULE)
	{
		m_Value[nErrorCount]="   �Ƿ���ʱ�η���";
		nErrorCount++;		
	}
	if(nErrorCount<2)
	{
		if(soft_error&INVALID_CONTROL_PARAM)
		{
			m_Value[nErrorCount]="   �Ƿ��Ŀ��Ʋ���";
			nErrorCount++;				
		}	
	}
	if(nErrorCount<2)
	{
		if(soft_error&INVALID_DETECTOR_SETTING)
		{
			m_Value[nErrorCount]="   �Ƿ����������";
			nErrorCount++;				
		}	
	}
	if(nErrorCount<2)
	{
		if(soft_error&INVALID_DETECTOR_SETTING)
		{
			m_Value[nErrorCount]="   �Ƿ����������";
			nErrorCount++;				
		}	
	}
	return nErrorCount;
}
	