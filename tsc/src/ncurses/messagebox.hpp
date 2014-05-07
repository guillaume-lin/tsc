/* @(#)messagebox.hpp
 */

#ifndef _MESSAGEBOX_H
#define _MESSAGEBOX_H 1


#include <stddef.h>
#include "baseform.hpp"

class CMessageBox : public CBaseForm {
public:
	CMessageBox(const char* msg,int nWait=1);
	virtual int virtualize(int c);
private:

};


#endif /* _MESSAGEBOX_H */

