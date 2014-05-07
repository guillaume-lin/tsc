/* @(#)statusmenu.hpp
 */

#ifndef _STATUSMENU_H
#define _STATUSMENU_H 1

#include "basemenu.hpp"
#include "ShowSystemTimeform.hpp"

/**
 * ��ʾϵͳ״̬
 */

class CStatusMenu : public CBaseMenu{
public:
	CStatusMenu()
		:CBaseMenu(3,12,(lines()-3)/2,(cols()-12)/2)
	{
		I = new NCursesMenuItem*[2];
		I[0] = new MenuItem_T<CShowSystemTimeform>("ϵͳʱ��");
		I[1] = new NCursesMenuItem();
		InitMenu(I,TRUE,TRUE);
		boldframe();
	}
};

#endif /* _STATUSMENU_H */

