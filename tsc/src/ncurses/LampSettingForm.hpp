/* @(#)lampsettingform.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  张继良
* E-Mail:  pony_fx@sina.com
* Date:    2008-11-18 14:55:00 (中国标准时间)
*/

#ifndef _LAMPSETTINGFORM_H
#define _LAMPSETTINGFORM_H

#include <stddef.h>

#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CLampSettingForm : public CBaseForm {
public:
	CLampSettingForm();
	~CLampSettingForm();
	
	int doSet();
	int doGet();
	int onLoad();
	int onSave();
	int onPageUp();
	int onPageDown();
private:
	lamp_setting_t m_lampsetting;

	int      m_CurValue;
	int      m_Index;
	int      get_bit_value(unsigned char* bytearray,int num, int base);
	int     set_bit_value(unsigned char* bytearray,int num, int base, int value);
	void     setLableValue(void);
};

#endif /* _TERMINALSTATUSFORM_H */

