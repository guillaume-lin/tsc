/* @(#)ShowSystemTimeform.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-20 14:35:00 (中国标准时间)
*/

#ifndef _SHOWSYSTEMTIMEFORM_H
#define _SHOWSYSTEMTIMEFORM_H 1


#include <string>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "uc_ErrorInfo.h"
#include "../control/datastruct.h"

class CShowSystemTimeform : public CBaseForm{
public:
	CShowSystemTimeform();
	int doSet();
	int doGet();
	int virtualize(int c);
	int onPageUp();
	int onPageDown();
	int onFieldChanged(NCursesFormField& f);
	bool isExitPressed(){ return mIsExitPressed; };
private:
	bool mIsExitPressed;
	CFieldMonitor   m_Monitor;
	machine_status_t m_data;
	int  m_CurValue;
	int m_ICount;
	std::string   m_Value[10];
	uc_ErrorInfo m_ErrorInfo;
private:
	int GetErrorInfo(byte  soft_error);
};

#endif /* _SETTIME_H */

