/* @(#)fieldmonitor.cpp
 */

#include "fieldmonitor.hpp"
#include "baseform.hpp"

CFieldMonitor::CFieldMonitor(CBaseForm* f)
	:m_form_p(f)
{
}
bool CFieldMonitor::field_check(NCursesFormField& f)
{
	if(m_form_p == 0)
		return true;
	if(m_form_p->onFieldChanged(f) == 0)
		return true;
	else
		return false;
}
bool CFieldMonitor::char_check(int c)
{
	return true;
}
