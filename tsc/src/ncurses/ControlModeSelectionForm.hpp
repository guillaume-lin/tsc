/* @(#)ControlModeSelectionForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 18:44:07 (中国标准时间)
*/

#ifndef _CONTROLMODESELECTIONFORM_H
#define _CONTROLMODESELECTIONFORM_H 1
#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CControlModeSelectionForm : public CBaseForm{
public:
	CControlModeSelectionForm();
	int doSet();
	int doGet();
	int onLoad();
	int onSave();

private:
	control_mode_selection_t m_data;
};

#endif /* _SETTIME_H */

