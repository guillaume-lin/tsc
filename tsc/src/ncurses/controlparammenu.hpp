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
		I[0] = new MenuItem_T<CSystemParamForm>("ϵͳ����");
		I[1] = new MenuItem_T<CControlModeSelectionForm>("����ģʽѡ��");
		I[2] = new MenuItem_T<CMultipleScheduleControlParamForm>("��ʱ�β���");
		I[3] = new MenuItem_T<CActuateControlParamForm>("��Ӧ���Ʋ���");
		I[4] = new MenuItem_T<CCordlessCoordinateControlParamForm>("������Э��");
		I[5] = new NCursesMenuItem();
		InitMenu(I,TRUE,TRUE);
	}
};

#endif /* _CONTROLPARAMMENU_H */
