/* @(#)settimeform.cpp
 */
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include "settimeform.hpp"
#include "label.hpp"
#include "field.hpp"
#include "messagebox.hpp"

CIntegerChecker yf(2000,2099);
CIntegerChecker mf(1,12);
CIntegerChecker df(1,31);
CIntegerChecker hf(0,23);
CIntegerChecker minf(0,59);
CIntegerChecker sf(0,59);

CSetTimeForm::CSetTimeForm()
	:CBaseForm(8,24,0,0)
{
	F = new NCursesFormField*[14];
	F[0] = new CLabel("设置日期时间",1,3);

	F[1] = new CLabel("年",2,7);
	F[2] = new CField(4,2,3);
	F[2]->set_fieldtype(yf);

	F[3] = new CLabel("月",2,11);
	F[4] = new CField(2,2,9);
	F[4]->set_fieldtype(mf);

	F[5] = new CLabel("日",2,15);
	F[6] = new CField(2,2,13);
	F[6]->set_fieldtype(df);
   
	F[7]  = new CLabel("时",3,7);
	F[8]  = new CField(2,3,3);
	F[8]->set_fieldtype(hf);

	F[9]  = new CLabel("分",3,11);
	F[10] = new CField(2,3,9);
	F[10]->set_fieldtype(minf);

	F[11] = new CLabel("秒",3,15);
	F[12] = new CField(2,3,13);
	F[12]->set_fieldtype(sf);

	F[13] = new NCursesFormField();

	InitForm(F,TRUE,TRUE);
	onLoad();
	doGet();
}
int CSetTimeForm::onSave()
{
	doSet();
	vm_set_beep_option(&g_vm,1);
	//设置Linux系统的时间
	struct timeval tv;
	struct tm t;
	t.tm_sec = m_dt.m_second;
	t.tm_min = m_dt.m_minute;
	t.tm_hour = m_dt.m_hour;
	t.tm_mday = m_dt.m_day;
	t.tm_mon  = m_dt.m_month - 1;
	t.tm_year = m_dt.m_year[0]*100+m_dt.m_year[1]-1900;
	t.tm_isdst = -1;
	tv.tv_sec = mktime(&t);
	tv.tv_usec = 0;
	if(settimeofday(&tv,NULL)==0)
	{
		//设置成功
		//获取星期几参数
		time_t t=time(0);
		struct tm* ptm=localtime(&t);
		if(ptm!=0)
		{
			m_dt.m_weekday=ptm->tm_wday;
			return vm_set_system_time(&g_vm,&m_dt);
		}
		else
			return -1;
  }
	else
		return -1;
}
int CSetTimeForm::onLoad()
{
	vm_set_beep_option(&g_vm,1);
	return vm_get_system_time(&g_vm,&m_dt);
}
int CSetTimeForm::doSet()
{
	// get data from ui and send to mcu

	m_dt.m_year[0] = atoi(F[2]->value())/100;
	m_dt.m_year[1] = atoi(F[2]->value())%100;
	m_dt.m_month   = atoi(F[4]->value());
	m_dt.m_day     = atoi(F[6]->value());
	m_dt.m_hour    = atoi(F[8]->value());
	m_dt.m_minute  = atoi(F[10]->value());
	m_dt.m_second  = atoi(F[12]->value());

	return 0;
}
int CSetTimeForm::doGet()
{
	// get data from mcu and set to ui

	F[2]->set_value(itoa(m_dt.m_year[0]*100+m_dt.m_year[1]));
	F[4]->set_value(itoa(m_dt.m_month));
	F[6]->set_value(itoa(m_dt.m_day));
	F[8]->set_value(itoa(m_dt.m_hour));
	F[10]->set_value(itoa(m_dt.m_minute));
	F[12]->set_value(itoa(m_dt.m_second));
	return 0;
}
int CSetTimeForm::onFieldChanged(NCursesFormField& f)
{
	// F[2] will lost focus
	int y;
	y = atoi(F[2]->value());
	if(y> 1900 && y < 9999)
		return 0;
	else{
		f.set_value(itoa(y));
		return -1;
	}

}
