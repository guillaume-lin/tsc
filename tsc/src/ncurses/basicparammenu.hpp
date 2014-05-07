/* @(#)basicparammenu.hpp
 */

#ifndef _BASICPARAMMENU_H
#define _BASICPARAMMENU_H 1


#include <stddef.h>
#include "basemenu.hpp"
#include "PhasePlanForm.hpp"
#include "TimmingPlanForm.hpp"
#include "SchedulePlanForm.hpp"
#include "SpecialDayForm.hpp"

class CBasicParamMenu : public CBaseMenu {
public:
	CBasicParamMenu()
		:CBaseMenu(6,12,(lines()-6)/2,(cols()-12)/2)
	{
		I = new NCursesMenuItem*[5];
		I[0] = new MenuItem_T<CPhasePlanForm>("��λ����");
		I[1] = new MenuItem_T<CTimmingPlanForm>("��ʱ����");
		I[2] = new MenuItem_T<CSchedulePlanForm>("ʱ�η���");
		I[3] = new MenuItem_T<CSpecialDayForm>("������");
		I[4] = new NCursesMenuItem();
		InitMenu(I,TRUE,TRUE);
	}
};

#endif /* _BASICPARAMMENU_H */
