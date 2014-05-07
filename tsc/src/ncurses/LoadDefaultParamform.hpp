/* @(#)terminalstatusform.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  张继良（pony_fx@sina.com)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2008-7-3 14:55:00 (中国标准时间)
*/

#ifndef _LOADDEFAULTPARAMFORM_H
#define _LOADDEFAULTPARAMFORM_H 1

#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CLoadDefaultParamForm : public CBaseForm {
public:
	CLoadDefaultParamForm();
	int  onSave();	
	virtual int virtualize(int c);
	int onFieldChanged(NCursesFormField& f);
private:
	CFieldMonitor   m_Monitor;	
//	int             m_bFlag;//通过验证
private:
	int SaveConfig(system_config_t* config_p);
	int LoadDefault_Param(system_config_t &config);
	int Save();	
};

#endif /* _TERMINALSTATUSFORM_H */

