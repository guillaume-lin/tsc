/* @(#)systemparammenu.hpp
 */

#ifndef _SYSTEMPARAMMENU_H
#define _SYSTEMPARAMMENU_H 1

#include "basemenu.hpp"
#include "settimeform.hpp"
#include "LampGroupSettingForm.hpp"
#include "LampSettingForm.hpp"
#include "DetectorSettingForm.hpp"
#include "greenconflictform.hpp"

class CSystemParamMenu : public CBaseMenu {
public:
	CSystemParamMenu()
		:CBaseMenu(8,16,(lines()-8)/2,(cols()-16)/2)
	{
		I = new NCursesMenuItem*[6];
		I[0] = new MenuItem_T<CSetTimeForm>("ʱ������");
		I[1] = new MenuItem_T<CLampSettingForm>("�źŵ�����");		
		I[2] = new MenuItem_T<CLampGroupSettingForm>("��������");
		I[3] = new MenuItem_T<CDetectorSettingForm>("���������");
		I[4] = new MenuItem_T<CGreenConflictForm>("�̳�ͻ����");
		I[5] = new NCursesMenuItem();
		InitMenu(I,TRUE,TRUE);
	}
};

#endif /* _SYSTEMPARAMMENU_H */

