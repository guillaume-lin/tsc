/* @(#)messagebox.hpp
 */

#ifndef _MESSAGEBOX_H
#define _MESSAGEBOX_H 1
#include  <stdlib.h>
#include <cursesf.h>

class CMessageBox : public NCursesForm {
public:
	CMessageBox(const char* msg);
	virtual int virtualize(int c);
private:
	NCursesFormField** F;
};


#endif /* _MESSAGEBOX_H */

