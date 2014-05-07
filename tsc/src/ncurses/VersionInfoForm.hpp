/* @(#)terminalstatusform.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  �ż�����pony_fx@sina.com)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2008-7-3 14:55:00 (�й���׼ʱ��)
*/

#ifndef _VERSIONINFOFORM_H
#define _VERSIONINFOFORM_H 1

#include <cursesm.h>
#include <cursesf.h>
#include "baseform.hpp"
#include "label.hpp"
#include "../control/datastruct.h"
class CVersionInfoForm : public CBaseForm {
public:
	CVersionInfoForm():CBaseForm(8,26,0,0)
	{
		int nVersion=0;
		vm_get_version(&g_vm,&nVersion);
		char strTemp[128];
		sprintf(strTemp,"MCU�汾V%d",nVersion);
		F = new NCursesFormField*[5];
		F[0] = new CLabel("UI�汾 V1.13",1,1);
		F[1] = new CLabel(strTemp,2,1);			
		F[2] = new CLabel("��Ȩ:�����ż����ӿƼ�",3,1);
		F[3] = new CLabel("�绰��0592-5213662",4,1);
		F[4] = new NCursesFormField();
		InitForm(F,TRUE,TRUE);	
	}
};

#endif /* _TERMINALSTATUSFORM_H */

