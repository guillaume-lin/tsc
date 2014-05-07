/* @(#)mainmenu.hpp
 */

#ifndef _MAINMENU_H
#define _MAINMENU_H 1

#include <stddef.h>
#include <cursesm.h>
#include "key_translate.h"
#include "basemenu.hpp"
#include "systemparammenu.hpp"
#include "basicparammenu.hpp"
#include "controlparammenu.hpp"
#include "ShowSystemTimeform.hpp"
#include "HardwareStatusMenu.hpp"
#include "LoadDefaultParamform.hpp"
#include "PassValidateForm.hpp"
#include "VersionInfoForm.hpp"

class CMainMenu : public CBaseMenu {
public:
		CMainMenu():CBaseMenu(8,16,(lines()-8)/2,(cols()-16)/2)
		{
			I = new NCursesMenuItem*[7];

			I[0] = new MenuItem_T<CSystemParamMenu>("ϵͳ��������");
			I[1] = new MenuItem_T<CControlParamMenu>("���Ʋ�������");
			I[2] = new MenuItem_T<CBasicParamMenu>("�������в���");
			I[3] = new MenuItem_T<CHardwareStatusMenu>("״̬��Ϣ��ʾ");
			I[4] = new MenuItem_T<CPassValidateForm>("װ��Ĭ�ϲ���");
			I[5] = new MenuItem_T<CVersionInfoForm>("�汾��Ϣ��ʾ");
			I[6] = new NCursesMenuItem();

			InitMenu(I,TRUE,TRUE);
		}
};


#endif /* _MAINMENU_H */
