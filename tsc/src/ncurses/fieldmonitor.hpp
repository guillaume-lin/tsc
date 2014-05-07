/* @(#)fieldmonitor.hpp
 */

#ifndef _FIELDMONITOR_H
#define _FIELDMONITOR_H 1


#include <stddef.h>
#include <cursesf.h>

class CBaseForm;

class CFieldMonitor : public UserDefinedFieldType {
public:
	CFieldMonitor()
	{
		m_form_p = 0;
	}
	CFieldMonitor(CBaseForm* f);
	void set_form(CBaseForm* f)
	{
		m_form_p = f;
	}
	virtual bool field_check(NCursesFormField& f);
	virtual bool char_check(int c);
private:
	CBaseForm* m_form_p;
};

#endif /* _FIELDMONITOR_H */

