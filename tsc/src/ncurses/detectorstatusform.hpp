/* @(#)terminalstatusform.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  �ż���
* E-Mail:  pony_fx@sina.com
* Date:    2008-5-8 14:55:00 (�й���׼ʱ��)
*/

#ifndef _DETECTORSTATUSFORM_H
#define _DETECTORSTATUSFORM_H


#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CDetectorStatusForm : public CBaseForm {
public:
	CDetectorStatusForm();

	int doGet();
	int onPageUp();
	int onPageDown();

	int virtualize(int c);
private:
	machine_status_t m_machine_status;

	int      m_CurValue;
	int      m_Index;

	void     setLableValue(void);
};

#endif /* _TERMINALSTATUSFORM_H */
