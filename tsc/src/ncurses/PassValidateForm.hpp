/* @(#)PassValidateForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  �ż�����pony_fx@sina.com)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2008-7-3 14:55:00 (�й���׼ʱ��)
*/

#ifndef _PASSVALIDATEFORM_H
#define _PASSVALIDATEFORM_H 1

#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CPassValidateForm : public CBaseForm {
public:
	CPassValidateForm();
	virtual int virtualize(int c);
	int onFieldChanged(NCursesFormField& f);
private:
	CFieldMonitor   m_Monitor;	
};

#endif /* _TERMINALSTATUSFORM_H */

