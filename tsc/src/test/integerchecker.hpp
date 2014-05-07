/* @(#)integerchecker.hpp
 */

#ifndef _INTEGERCHECKER_H
#define _INTEGERCHECKER_H 1

#include <stdlib.h>
#include <cursesf.h>
#include <stdlib.h>

class CIntegerChecker : public UserDefinedFieldType {
public:
	CIntegerChecker(int low,int high);
	virtual bool field_check(NCursesFormField& f);
	virtual bool char_check(int c);
private:
	int m_low;
	int m_high;
};

const char* IntegerToAnsi(int i);

#endif /* _INTEGERCHECKER_H */

