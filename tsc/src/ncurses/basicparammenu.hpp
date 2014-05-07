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
		I[0] = new MenuItem_T<CPhasePlanForm>("相位方案");
		I[1] = new MenuItem_T<CTimmingPlanForm>("配时方案");
		I[2] = new MenuItem_T<CSchedulePlanForm>("时段方案");
		I[3] = new MenuItem_T<CSpecialDayForm>("特殊日");
		I[4] = new NCursesMenuItem();
		InitMenu(I,TRUE,TRUE);
	}
};

#endif /* _BASICPARAMMENU_H */

