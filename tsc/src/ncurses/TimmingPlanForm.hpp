/* @(#)TimmingPlanForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-19 17:55:00 (中国标准时间)
*/

#ifndef _TIMMINGPLANFORM_H
#define _TIMMINGPLANFORM_H 1

#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CTimmingPlanForm : public CBaseForm{
public:
	CTimmingPlanForm();
	int doSet();
	int doGet();
	int onSave();
	int onLoad();
	int onFieldChanged(NCursesFormField& f);
	int onPageUp();
	int onPageDown();
private:
	timming_plan_t 	m_data[MAX_TIMMING_PLAN_COUNT]; 
	int      m_CurValue;
	int      m_Index;
	void     setLableValue(void);
	CFieldMonitor   m_Monitor;
};

#endif /* _SETTIME_H */

