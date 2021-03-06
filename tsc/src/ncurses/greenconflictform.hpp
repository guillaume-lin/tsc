/* @(#)greenconflictform.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-31 14:55:00 (中国标准时间)
*/

#ifndef _GREENCONFLICTFORM_H
#define _GREENCONFLICTFORM_H 1


#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CGreenConflictForm : public CBaseForm {
public:
	CGreenConflictForm();

	int onLoad();
	int onSave();

	int doSet();
	int doGet();
	int onPageUp();
	int onPageDown();

	int onFieldChanged(NCursesFormField& f);
private:
	green_conflict_t m_conflict;
	CFieldMonitor m_monitor;
	int      m_CurValue;
	int      m_Index;

	void     setLableValue(void);
};

#endif /* _GREENCONFLICTFORM_H */

