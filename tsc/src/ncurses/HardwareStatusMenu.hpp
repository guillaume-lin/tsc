/* @(#)mainmenu.hpp
 */

#ifndef _HARDWARE_STATUS_MENU_H
#define _HARDWARE_STATUS_MENU_H 1

#include <stddef.h>
#include <cursesm.h>
#include "key_translate.h"
#include "basemenu.hpp"
#include "terminalstatusform.hpp"
#include "detectorstatusform.hpp"

class CHardwareStatusMenu : public CBaseMenu {
public:
		CHardwareStatusMenu():CBaseMenu(8,16,(lines()-8)/2,(cols()-16)/2)
		{
			I = new NCursesMenuItem*[3];

			I[0] = new MenuItem_T<CTerminalStatusForm>("����״̬��ʾ");
			I[1] = new MenuItem_T<CDetectorStatusForm>("��Ȧ״̬��ʾ");
			I[2] = new NCursesMenuItem();

			InitMenu(I,TRUE,TRUE);
		}
};


#endif /* _MAINMENU_H */

