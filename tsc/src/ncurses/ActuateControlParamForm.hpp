/* @(#)ActuateControlParamForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 13:30:00 (中国标准时间)
*/

#ifndef _ACTUATECONTROLPARAMFORM_H
#define _ACTUATECONTROLPARAMFORM_H 1


#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CActuateControlParamForm : public CBaseForm{
public:
	CActuateControlParamForm();
	int doSet();
	int doGet();
	int onLoad();
	int onSave();
private:
	actuate_control_param_t m_data;
};

#endif /* _SETTIME_H */

