/* @(#)controlparammenu.hpp
 */

#ifndef _CONTROLPARAMMENU_H
#define _CONTROLPARAMMENU_H 1

#include <stddef.h>
#include "basemenu.hpp"
#include "SystemParamForm.hpp"
#include "ControlModeSelectionForm.hpp"
#include "MultipleScheduleControlParamForm.hpp"
#include "ActuateControlParamForm.hpp"
#include "CordlessCoordinateControlParamForm.hpp"

class CControlParamMenu : public CBaseMenu {
public:
	CControlParamMenu()
		:CBaseMenu(7,16,(lines()-7)/2,(cols()-16)/2)
	{
		I = new NCursesMenuItem*[6];
		I[0] = new MenuItem_T<CSystemParamForm>("系统参数");
		I[1] = new MenuItem_T<CControlModeSelectionForm>("控制模式选择");
		I[2] = new MenuItem_T<CMultipleScheduleControlParamForm>("多时段参数");
		I[3] = new MenuItem_T<CActuateControlParamForm>("感应控制参数");
		I[4] = new MenuItem_T<CCordlessCoordinateControlParamForm>("无线缆协调");
		I[5] = new NCursesMenuItem();
		InitMenu(I,TRUE,TRUE);
	}
};

#endif /* _CONTROLPARAMMENU_H */

