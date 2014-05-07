/* @(#)keyboardtestform.cpp
 */

#include "label.hpp"
#include "field.hpp"
#include "key_translate.h"
#include "keyboardtestform.hpp"

CKeyboardTestForm::CKeyboardTestForm()
	:CBaseForm(8,24,0,0)
{
	F = new NCursesFormField*[8];
	F[0] = new CLabel("¼üÅÌ²âÊÔ",0,9);
	F[1] = new CLabel("µ±Ç°°´¼ü",2,5);
	F[2] = new CField(2,2,13);
	F[3] = new CLabel("×ª»»ºóµÄ·ûºÅ",3,0);
	F[4] = new CField(2,3,13);
	F[5] = new CLabel("×´Ì¬ÐÐ×´Ì¬ÐÐ×´Ì¬",5,0);
	F[6] = new CLabel("×´Ì¬ÐÐ",5,16);
	F[7] = new NCursesFormField();
	boldframe();
	InitForm(F,TRUE,TRUE);

}
int CKeyboardTestForm::virtualize(int c)
{
	unsigned char key;
	int ret;

	key = 255;
	ret = vm_get_key_val(&g_vm,&key);
	if(ret == 0 && key != 0){
		F[2]->set_value(itoa(key));

	}
	return NCursesForm::virtualize(c);
}

int CKeyboardTestForm::doGet()
{
	return 0;
}
int CKeyboardTestForm::doSet()
{
	return 0;
}

