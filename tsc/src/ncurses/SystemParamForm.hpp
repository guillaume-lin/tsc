/* @(#)SystemParamForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(ÀîÃ÷)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-19 
 */

#ifndef _SYSTEMPARAMFORM_H
#define _SYSTEMPARAMFORM_H 1
#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CSystemParamForm : public CBaseForm{
public:
	CSystemParamForm();
	int doSet();
	int doGet();
	int onSave();
	int onLoad();
private:
	system_param_t m_data;
};

#endif /* _SETTIME_H */

