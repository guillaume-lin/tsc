/* @(#)SchedulePlanForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-19 15:35:00 (中国标准时间)
*/

#ifndef _SCHEDULEPLANFORM_H
#define _SCHEDULEPLANFORM_H 1
#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CSchedulePlanForm : public CBaseForm{
public:
	CSchedulePlanForm();
	int doSet();
	int doGet();
	int onLoad();
	int onSave();
	int onFieldChanged(NCursesFormField& f);
	int onPageUp();
	int onPageDown();
private:
	schedule_plan_t 	m_data[MAX_SCHEDULE_PLAN_COUNT]; 
	int      m_CurValue;
	int      m_Index;
	void     setLableValue(void);
	CFieldMonitor   m_Monitor;
};

#endif /* _SETTIME_H */

