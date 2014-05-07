/* @(#)basemenu.hpp
 */

#ifndef _BASEMENU_H
#define _BASEMENU_H 1


#include <stddef.h>
#include <stdlib.h>
#include <cursesm.h>
#include "../control/vm.h"
#include "../control/datastruct.h"

extern vm_t g_vm;

template<typename T>
class MenuItem_T : public NCursesMenuItem
{
	public:
		MenuItem_T(const char* s):NCursesMenuItem(s)
		{
		}
		bool action()
		{
			T form;
			form();
			return FALSE;
		}
};


class CBaseMenu : public NCursesMenu {
public:
	CBaseMenu(int h,int w, int y, int x);
	virtual ~CBaseMenu();
	virtual int getKey();
	virtual int virtualize(int c);	
protected:
	NCursesMenuItem** I;
};

#endif /* _BASEMENU_H */

