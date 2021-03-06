/* @(#)integerchecker.hpp
 */

#ifndef _INTEGERCHECKER_H
#define _INTEGERCHECKER_H 1

#include <stddef.h>
#include <cursesf.h>

class CBaseForm;

class CIntegerChecker : public UserDefinedFieldType {
public:
	CIntegerChecker(int low,int high);
	virtual bool field_check(NCursesFormField& f);
	virtual bool char_check(int c);
private:
	int m_low;
	int m_high;
};

#endif /* _INTEGERCHECKER_H */

