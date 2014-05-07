/* @(#)PhasePlanForm.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  Leslie(李明)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2005-10-19 19:55:00 (中国标准时间)
*/

#ifndef _PHASEPLANFORM_H
#define _PHASEPLANFORM_H 1
#include <stddef.h>
#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "../control/datastruct.h"

class CPhasePlanForm : public CBaseForm{
public:
	CPhasePlanForm();
	~CPhasePlanForm();
	int doSet();
	int doGet();
	int onLoad();
	int onSave();
	int onFieldChanged(NCursesFormField& f);
	int onPageUp();
	int onPageDown();
private:
	stage_plan_t 	m_data[MAX_PHASE_PLAN_COUNT]; 
	green_conflict_t m_conflict;
	int      m_CurValue;
	int      m_Index;
	int      m_IndexPhase;
	void     setLableValue(void);
	CFieldMonitor   m_Monitor;
	byte  SetValue(int index1,int index2,int index3,int index4);
	void  GetValue(byte attr,int index1,int index2,int index3,int index4);
	byte set_bits(int n, int v,byte phase);
	int get_bits(int n,byte phase);
	int    m_stage_count;
};

#endif /* _PHASE_PLAN_FORM_H */

