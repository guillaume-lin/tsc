/**
 * menu.hpp
 *
 * menu for config
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-15 10:45:42 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_MENU_HPP
#define INCLUDED_MENU_HPP
#include <stddef.h>
#include <string>
#include "cursesm.h"
#include "PhasePlanForm.hpp"

template<typename T>
class MenuItem_T : public NCursesMenuItem
{
	public:
		MenuItem_T(const char* s):NCursesMenuItem(s)
		{
		}
		bool action()
		{
			T form;
			form();
			return FALSE;
		}
};
class CQuitMenuItem : public NCursesMenuItem
{
	public:
		CQuitMenuItem():NCursesMenuItem("返回")
		{
		}
		bool action()
		{
			return TRUE;
		}
};

class CSubMenu : public NCursesMenu {
public:
	CSubMenu():NCursesMenu(4,16,0,0)
	{
		I = new NCursesMenuItem*[2];
		I[0] = new CQuitMenuItem();
		I[1] = new NCursesMenuItem();
		InitMenu(I,TRUE,TRUE);
	}
private:
	NCursesMenuItem** I;
};
class CSubMenuItem : public NCursesMenuItem{
public:
	CSubMenuItem():NCursesMenuItem("子菜单")
	{
	}
	bool action()
	{
		CSubMenu subMenu;
		subMenu();
		return FALSE;
	}
};
class CMenu : public NCursesMenu
{
	public:
#define MENU_ITEM_COUNT 2
		CMenu():NCursesMenu(MENU_ITEM_COUNT+2,16,
				(lines()-MENU_ITEM_COUNT)/2,(cols()-16)/2)
		{
			I = new NCursesMenuItem*[MENU_ITEM_COUNT+1];

			I[0] = new MenuItem_T<CPhasePlanForm>("控制参数");

			/*
			I[0] = new MenuItem_T<CFormStatus>("信号机状态");
			I[1] = new MenuItem_T<CFormControlMode>("控制模式");
			I[2] = new MenuItem_T<CFormCommParam>("通信参数");
			I[3] = new MenuItem_T<CSetTimeForm>("设置时间");
			I[4] = new MenuItem_T<CFormCordless>("无线缆协调控制");
			I[5] = new MenuItem_T<CFormActuate>("感应控制");
			I[6] = new MenuItem_T<CFormMulti>("多时段控制");
			I[7] = new MenuItem_T<CFormSpecialDay>("特殊日设置");
			I[8] = new MenuItem_T<CFormTimeSchedule>("时段方案设置");
			I[9] = new MenuItem_T<CFormTimming>("配时方案设置");
			I[10] = new MenuItem_T<CFormSafe>("保安时间设置");
			I[11] = new MenuItem_T<CFormPhase>("相位方案设置");
			I[12] = new MenuItem_T<CFormDetector>("检测器设置");
			I[13] = new MenuItem_T<CFormLampGroup>("灯组设置");
			I[14] = new MenuItem_T<CFormBasic>("基本设置");
*/
			I[1] = new CQuitMenuItem();
			I[2] = new NCursesMenuItem();

			halfdelay(2);	// 0.2 秒产生一次按键超时
			InitMenu(I,TRUE,TRUE);
		}

		~CMenu()
		{
		}
	private:
		NCursesMenuItem** I;

};
#endif

