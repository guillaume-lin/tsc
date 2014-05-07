/* @(#)messagebox.cpp
 */

#include "messagebox.hpp"
#include "label.hpp"
//#include "key_translate.h"

#define CTRL(X) ((X) & 0x1f)

CMessageBox::CMessageBox(const char* msg)
	:NCursesForm(5,strlen(msg)+4,(lines()-5)/2,(cols()-strlen(msg)-4)/2)
{
	F = new NCursesFormField*[2];
	F[0] = new CLabel(msg,1,1);
	F[1] = new NCursesFormField();

	InitForm(F,TRUE,TRUE);
	//boldframe();
	//getch();
}
int CMessageBox::virtualize(int c)
{
	if(c != ERR)
		return NCursesForm::virtualize(CTRL('X'));
	return NCursesForm::virtualize(c);
}

