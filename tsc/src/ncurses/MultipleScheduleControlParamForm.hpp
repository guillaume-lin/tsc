/* @(#)MultipleScheduleControlParamForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 11:44:07 (中国标准时间)

*/

#ifndef _MULTIPLESCHEDULECONTROLPARAMFORM_H
#define _MULTIPLESCHEDULECONTROLPARAMFORM_H 1
#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CMultipleScheduleControlParamForm : public CBaseForm{
public:
	CMultipleScheduleControlParamForm();
	int doSet();
	int doGet();
	int onLoad();
	int onSave();
private:
	multiple_schedule_control_param_t m_data;
};

#endif /* _SETTIME_H */

