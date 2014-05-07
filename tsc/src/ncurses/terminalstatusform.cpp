/* @(#)terminalstatusform.cpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-31 14:55:00 (中国标准时间)
*/

#include "terminalstatusform.hpp"
#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"
#include "messagebox.hpp"

static CIntegerChecker zero_one_ic(0,1);

CTerminalStatusForm::CTerminalStatusForm()
	:CBaseForm(8,24,0,0)
{
	m_CurValue =0;

	F = new NCursesFormField*[22];
	F[0] = new CLabel("端子状态",0,0);

	F[1] = new CLabel("故障个数",0,10);
	F[2] = new CLabel("00",0,19);

	F[3] = new CLabel("端子编号",1,0);
	F[4] = new CLabel("灯态/故障",1,10);	

	F[5] = new CLabel("01-03",2,0);
	F[6] = new CLabel("04-06",3,0);
	F[7] = new CLabel("07-09",4,0);
	F[8] = new CLabel("10-12",5,0);

	F[9] = new CLabel("1/1",2,10);
	F[10] = new CLabel("1/1",2,14);
	F[11] = new CLabel("1/1",2,18);

	F[12] = new CLabel("1/1",3,10);
	F[13] = new CLabel("1/1",3,14);
	F[14] = new CLabel("1/1",3,18);

	F[15] = new CLabel("1/1",4,10);
	F[16] = new CLabel("1/1",4,14);
	F[17] = new CLabel("1/1",4,18);

	F[18] = new CLabel("1/1",5,10);
	F[19] = new CLabel("1/1",5,14);
	F[20] = new CLabel("1/1",5,18);


	
	F[21] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
	setLableValue();
}
int CTerminalStatusForm::doGet()
{
	int count = 0;
	char buf[10];
	lamp_status_t* p;
	lamp_health_t* h;
	if(vm_get_machine_status(&g_vm,&m_machine_status) != 0)
		return -1;
	p = &m_machine_status.m_current_lamp_status;
	h = &m_machine_status.m_lamp_health;
	

	for(int i=m_CurValue*12; i<MAX_TERMINAL_COUNT && i<m_CurValue*12+12; i++){
		int pv = phase_get_output(p,i+1);
		int hv = lamp_health_get_health(h,i+1);
		sprintf(buf,"%01d/%01d",pv,hv);
		F[9+i-m_CurValue*12]->set_value(buf);
	}
	count = 0;
	for(int i=0; i<MAX_TERMINAL_COUNT; i++){
		if(lamp_health_get_health(h,i+1))
			count++;
	}
	sprintf(buf,"%2d",count);
	F[2]->set_value(buf);
	return 0;
}
int CTerminalStatusForm::virtualize(int c)
{
	doGet();
	return CBaseForm::virtualize(c);
}
int CTerminalStatusForm::onPageUp()
{
	if(m_CurValue > 0)
		m_CurValue--;
	setLableValue();
	return 0;
}
int CTerminalStatusForm::onPageDown()
{
	if(m_CurValue < (MAX_TERMINAL_COUNT/12 - 1))
		m_CurValue++;
	setLableValue();
	return 0;
}
void CTerminalStatusForm::setLableValue(void)
{
	char buf[5];
	sprintf(buf,"%02d-%02d",m_CurValue*12+1,m_CurValue*12+3);
	F[5]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*12+4,m_CurValue*12+6);
	F[6]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*12+7,m_CurValue*12+9);
	F[7]->set_value(buf);

	sprintf(buf,"%02d-%02d",m_CurValue*12+10,m_CurValue*12+12);
	F[8]->set_value(buf);
	doGet();
}
