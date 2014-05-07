/* @(#)baseform.cpp
 */


#include "baseform.hpp"
#include "../control/vm.h"
#include "key_translate.h"
#include "messagebox.hpp"
#include "refresh_console.hpp"

const char* itoa(int i)
{
        static char s[20];
        sprintf(s,"%d",i);
        return s;
}


CBaseForm::CBaseForm(int h, int w, int y, int x)
	:NCursesForm(h,w,y,x)
{
	halfdelay(1);
	postKey(CTRL('R'));
}
CBaseForm::~CBaseForm()
{
}
int CBaseForm::onFieldChanged(NCursesFormField& f)
{
	return 0;
}
int CBaseForm::onSave()
{
	return 0;
}
int CBaseForm::onLoad()
{
	return 0;
}
int CBaseForm::onPageUp()
{
	return 0;
}
int CBaseForm::onPageDown()
{
	return 0;
}
int CBaseForm::postKey(int key)
{
	m_keys.push(key);
	return 0;
}
int CBaseForm::getKey()
{
	unsigned char key;
	int ret;
	if(m_keys.size() > 0){
		ret =  m_keys.front();
		m_keys.pop();
		return ret;
	}
	vm_set_beep_option(&g_vm,0);
	ret = vm_get_key_val(&g_vm,&key);
	if(ret == 0 && key != 0 && (ret = translate_key(key)) != 0){
		refresh_console();
		return ret;
	}
	else
		return getch();
}
int CBaseForm::virtualize(int c)
{
	int err;
	refresh();
	switch(c){
	case CTRL('R'):
		if(onLoad() != 0){
			CMessageBox box("载入参数失败，请重试!");
			box();
			return NCursesForm::virtualize(CTRL('X'));
		}
		else
			doGet();
		return REQ_FIRST_FIELD;
	case KEY_UP:
		return REQ_UP_FIELD;
	case KEY_DOWN:
		return REQ_DOWN_FIELD;
	case KEY_LEFT:
		return REQ_PREV_FIELD;
	case KEY_RIGHT:
		return REQ_NEXT_FIELD;
	case KEY_ENTER:case CTRL('W'):
		err = driver(REQ_NEXT_FIELD);
		if(err == E_INVALID_FIELD){
			CMessageBox box("当前输入框的参数非法!");
			box();
			return REQ_PREV_FIELD;
		}
		else{
			// field is ok
			doSet();
			if(onSave() != 0){
				CMessageBox box("保存失败，请重试!");
				box();
			}
/*			else
			{
				CMessageBox box("保存成功");
				box();
			}*/
			return REQ_PREV_FIELD;
		}
	case KEY_EXIT:
		// exit
		return NCursesForm::virtualize(CTRL('X'));
	case KEY_PPAGE: case CTRL('P'):
		onPageUp();
		return REQ_FIRST_FIELD;
	case KEY_NPAGE: case CTRL('N'):
		onPageDown();
		return REQ_FIRST_FIELD;
	case ERR:
		return REQ_OVL_MODE;
	default:
		break;
	}
	return NCursesForm::virtualize(c);

}
