/* @(#)settimeform.hpp
 */

#ifndef _SETTIME_H
#define _SETTIME_H 1


#include "baseform.hpp"


class CSetTimeForm : public CBaseForm{
public:
	CSetTimeForm();
	int onLoad();
	int onSave();
	int doSet();
	int doGet();
	int onFieldChanged(NCursesFormField& f);
private:
	CFieldMonitor m_yearMonitor;
	date_time_t m_dt;

};

#endif /* _SETTIME_H */

