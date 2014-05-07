/* @(#)integerchecker.cpp
 */
#include "integerchecker.hpp"



CIntegerChecker::CIntegerChecker(int l,int h):
	m_low(l),m_high(h)
{
}

bool CIntegerChecker::field_check(NCursesFormField& f)
{
	int v;
	v = atoi(f.value());
	if(v < m_low || v > m_high){
		f.set_value(IntegerToAnsi(v));
		return false;
	}else
		return true;
}
bool CIntegerChecker::char_check(int c)
{
	if(c < '0' || c > '9')
		return false;
	else
		return true;
}

const char* IntegerToAnsi(int i)
{
	static char s[20];
	sprintf(s,"%d",i);
	return s;
}