/* @(#)messagebox.cpp
 */

#include "messagebox.hpp"
#include "label.hpp"
#include "key_translate.h"

CMessageBox::CMessageBox(const char* msg,int nWait)
	:CBaseForm(8,24,0,0)
{
	//5,strlen(msg)+4,(lines()-5)/2,(cols()-strlen(msg)-4)/2
	F = new NCursesFormField*[9];
	F[0] = new CLabel("                      ",0,0);
	F[1] = new CLabel("**********************",1,0);
	F[2] = new CLabel("**********************",3,0);
	F[3] = new CLabel("                      ",4,0);
	F[4] = new CLabel("                      ",5,0);
	int nPlace=0;
	int nLen=strlen(msg);
	if(nLen>22)
	{
		char strMsg[20];
		sprintf(strMsg,"错误信息过长");
  	nPlace=(22-12)/2;
  	F[5]=new CLabel("*",2,0);
  	F[6]=new CLabel(strMsg,2,nPlace);
  	F[7]=new CLabel("*",2,21);
	  F[8]=new NCursesFormField();
  }
	else if(nLen>=21)
	{
		  F[5]=new CLabel(msg,2,0);
			F[6]=new NCursesFormField();
	}
  else
  {
  	   nPlace=(22-nLen)/2;
  	   F[5]=new CLabel("*",2,0);
  	   F[6]=new CLabel(msg,2,nPlace);
  	   F[7]=new CLabel("*",2,21);
			 F[8]=new NCursesFormField();
	}
	InitForm(F,TRUE,TRUE);
	if(nWait)
		getKey();
}
int CMessageBox::virtualize(int c)
{
	if(c != ERR)
		return NCursesForm::virtualize(CTRL('X'));
	return NCursesForm::virtualize(c);
}

