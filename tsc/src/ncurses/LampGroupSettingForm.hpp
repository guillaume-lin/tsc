/* @(#)LampGroupSettingForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(����)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 16:00:00 (�й���׼ʱ��)
*/

#ifndef _LAMPGROUPSETTINGFORM_H
#define _LAMPGROUPSETTINGFORM_H 1

#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CLampGroupSettingForm : public CBaseForm{
public:
	CLampGroupSettingForm();
	int doSet();
	int doGet();
	int onLoad();
	int onSave();
	int onPageUp();
	int onPageDown();
	int onFieldChanged(NCursesFormField& f);
private:
	CFieldMonitor   m_Monitor;
	lamp_group_setting_t m_data;
	int      m_LampGroupCount;//��������
	int      m_CurValue;
	void     setLableValue(void);
};

#endif /* _SETTIME_H */
