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

			I[0] = new MenuItem_T<CSystemParamMenu>("系统参数设置");
			I[1] = new MenuItem_T<CControlParamMenu>("控制参数设置");
			I[2] = new MenuItem_T<CBasicParamMenu>("基本运行参数");
			I[3] = new MenuItem_T<CHardwareStatusMenu>("状态信息显示");
			I[4] = new MenuItem_T<CPassValidateForm>("装载默认参数");
			I[5] = new MenuItem_T<CVersionInfoForm>("版本信息显示");
			I[6] = new NCursesMenuItem();

			InitMenu(I,TRUE,TRUE);
		}
};


#endif /* _MAINMENU_H */

