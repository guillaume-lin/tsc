/* @(#)basemenu.cpp
 */

#include <curses.h>
#include "basemenu.hpp"
#include "../control/vm.h"
#include "key_translate.h"
#include "refresh_console.hpp"

CBaseMenu::CBaseMenu(int h,int w, int y, int x)
	:NCursesMenu(h,w,y,x)
{
	halfdelay(1);
}
CBaseMenu:: ~CBaseMenu()
{
}
int CBaseMenu::getKey()
{
	unsigned char key;
	int ret;
	vm_set_beep_option(&g_vm,0);
	ret = vm_get_key_val(&g_vm,&key);
	if(ret == 0 && key != 0 && (ret = translate_key(key)) != 0){
		refresh_console();
		return ret;
	}
	else
		return getch();
}
int CBaseMenu::virtualize(int c)
{     

	switch(c){
	case KEY_ENTER:
		return NCursesMenu::virtualize(CTRL('M'));
	case KEY_EXIT:
		// exit
		return NCursesMenu::virtualize(CTRL('X'));
	default:
		break;
	}
	
	return NCursesMenu::virtualize(c);
}
