/* @(#)integerchecker.cpp
 */
#include "baseform.hpp"
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
		f.set_value(itoa(v));
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
