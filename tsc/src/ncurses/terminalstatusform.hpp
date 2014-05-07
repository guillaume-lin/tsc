/* @(#)terminalstatusform.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(����)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-31 14:55:00 (�й���׼ʱ��)
*/

#ifndef _TERMINALSTATUSFORM_H
#define _TERMINALSTATUSFORM_H 1

#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CTerminalStatusForm : public CBaseForm {
public:
	CTerminalStatusForm();

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
