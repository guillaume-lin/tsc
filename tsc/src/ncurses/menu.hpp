/**
 * menu.hpp
 *
 * menu for config
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-15 10:45:42 �й���׼ʱ��
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
		CQuitMenuItem():NCursesMenuItem("����")
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
	CSubMenuItem():NCursesMenuItem("�Ӳ˵�")
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

			I[0] = new MenuItem_T<CPhasePlanForm>("���Ʋ���");

			/*
			I[0] = new MenuItem_T<CFormStatus>("�źŻ�״̬");
			I[1] = new MenuItem_T<CFormControlMode>("����ģʽ");
			I[2] = new MenuItem_T<CFormCommParam>("ͨ�Ų���");
			I[3] = new MenuItem_T<CSetTimeForm>("����ʱ��");
			I[4] = new MenuItem_T<CFormCordless>("������Э������");
			I[5] = new MenuItem_T<CFormActuate>("��Ӧ����");
			I[6] = new MenuItem_T<CFormMulti>("��ʱ�ο���");
			I[7] = new MenuItem_T<CFormSpecialDay>("����������");
			I[8] = new MenuItem_T<CFormTimeSchedule>("ʱ�η�������");
			I[9] = new MenuItem_T<CFormTimming>("��ʱ��������");
			I[10] = new MenuItem_T<CFormSafe>("����ʱ������");
			I[11] = new MenuItem_T<CFormPhase>("��λ��������");
			I[12] = new MenuItem_T<CFormDetector>("���������");
			I[13] = new MenuItem_T<CFormLampGroup>("��������");
			I[14] = new MenuItem_T<CFormBasic>("��������");
*/
			I[1] = new CQuitMenuItem();
			I[2] = new NCursesMenuItem();

			halfdelay(2);	// 0.2 �����һ�ΰ�����ʱ
			InitMenu(I,TRUE,TRUE);
		}

		~CMenu()
		{
		}
	private:
		NCursesMenuItem** I;

};
#endif

