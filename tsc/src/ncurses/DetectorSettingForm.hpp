/* @(#)DetectorSettingForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-18 15:40:00 (中国标准时间)
*/

#ifndef _DETECTORSETTINGFORM_H
#define _DETECTORSETTINGFORM_H 1


#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CDetectorSettingForm : public CBaseForm{
public:
	CDetectorSettingForm();
	int doSet();
	int doGet();
	int onSave();
	int onLoad();
	int onPageUp();
	int onPageDown();
	int onFieldChanged(NCursesFormField& f);
private:
	CFieldMonitor   m_Monitor;
	detector_setting_t m_data;
	int      m_CurValue;
	void     setLableValue(void);
	int get_enable(byte attr);
	void set_enable(byte& attr,int v);
	int get_far(byte attr);
	void set_far(byte& attr,int v);
	int get_sensitivity(byte attr);
	void set_sensitivity(byte& attr,int v);
	byte  SetValue(int index);
	void  GetValue(byte attr,int index);

};

#endif /* _SETTIME_H */

