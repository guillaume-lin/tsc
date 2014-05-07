/* @(#)SpecialDayForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(����)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 18:44:07 (�й���׼ʱ��)
*/

#ifndef _SPECIALDAYFORM_H
#define _SPECIALDAYFORM_H 1

#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CSpecialDayForm : public CBaseForm{
public:
	CSpecialDayForm();
	int doSet();
	int doGet();
	int onLoad();
	int onSave();
	int onPageUp();
	int onPageDown();
private:
	special_day_table_t m_data;
	int      m_CurValue;
	void     setLableValue(void);

};

#endif /* _SETTIME_H */
