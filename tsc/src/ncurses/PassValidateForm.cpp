/* @(#)terminalstatusform.hpp
* Company: Xiamen Uni-Creative Technology Corporation
* Author:  张继良（pony_fx@sina.com)
* E-Mail:  mli0080@uni-Creative.com
* Date:    2008-7-3 14:55:00 (中国标准时间)
*/
#include <assert.h>
#include "LoadDefaultParamform.hpp"
#include "label.hpp"
#include "field.hpp"
#include "messagebox.hpp"
#include "../control/vm.h"
#include "key_translate.h"
#include "PassValidateForm.hpp"
CPassValidateForm::CPassValidateForm()
	:CBaseForm(8,24,0,0)
{
	F = new NCursesFormField*[3];
	F[0] = new CLabel("请输入密码：",1,6);
	F[1] = new CField(6,2,9);
	m_Monitor.set_form(this);
	F[1]->set_fieldtype(m_Monitor);
	F[2] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);	
}
int CPassValidateForm::virtualize(int c)
{
	int err;
	switch(c){
	case KEY_ENTER:
		break;
	case KEY_EXIT:// exit
		{
			return NCursesForm::virtualize(CTRL('X'));
		}
	default:
		break;
	}
	return NCursesForm::virtualize(c);
}
int CPassValidateForm::onFieldChanged(NCursesFormField& f)
{
		int   FiledValue = atoi(f.value());
		if(F[1] == &f)//密码
		{
			if(FiledValue!=183392)
				return -1;
			CLoadDefaultParamForm Form;
			Form();
			postKey(CTRL('X'));
		}
		return 0;	
}
