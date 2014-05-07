
#include "nc1.hpp"	

#define CTRL(X) ((X) & 0x1f)

CMyForm::CMyForm()
	: NCursesForm(14, 23, (lines()-14)/2, (cols()-23)/2)
{	
		int nCol = 1, nLine = 0;
		m_n1 = m_n2 = 0;
		m_chOper = '+';

		m_pChecker = new UserDefinedFieldType*[3];

		m_pChecker[0] = new CIntegerChecker(0, 999);
		m_pChecker[1] = new COperatorFieldType();
		m_pChecker[2] = new CIntegerChecker(0, 999);

		m_pFormField = new NCursesFormField*[10];
		m_pFormField[0] = new CLabel("Demo:Calculator", 
							nLine++, (23 - 15)/2);
		m_pFormField[1] = new CLabel("One:", nLine, 1);
		m_pFormField[2] = new CField(3, nLine + 1, 1);
		m_pFormField[2]->set_fieldtype(*m_pChecker[0]);

		m_pFormField[3] = new CLabel("Operator:", nLine, 6);
		m_pFormField[4] = new CField(1, nLine + 1, 10);
		m_pFormField[4]->set_fieldtype(*m_pChecker[1]);

		m_pFormField[5] = new CLabel("Two:", nLine, 16);
		m_pFormField[6] = new CField(3, nLine + 1, 16);
		m_pFormField[6]->set_fieldtype(*m_pChecker[2]);
		
		m_pFormField[7] = new CLabel("Ctrl+W: calculat", nLine + 9, 1);
		m_pFormField[8] = new CLabel("Ctrl+X: exit", nLine + 10, 1);
		
		m_pFormField[9] = new NCursesFormField;
		InitForm(m_pFormField, TRUE, TRUE);	
		boldframe();
		doGet();
}

CMyForm::~CMyForm()
{ 
//	for(int i = 0; i < sizeof(m_pFormField)/sizeof(void*); i++)
//		delete m_pFormField[i];
	for(int j = 0; j < sizeof(m_pChecker)/sizeof(void*); j++)
		delete m_pChecker[j];
	delete m_pChecker; 
}

int CMyForm::virtualize(int c)
{
	int err;

	switch(c)
	{
	case KEY_UP:
		return REQ_UP_FIELD;
	case KEY_DOWN:
		return REQ_DOWN_FIELD;
	case KEY_LEFT:
		return REQ_PREV_FIELD;
	case KEY_RIGHT:
		return REQ_NEXT_FIELD;
	case CTRL('W'):
			onSave();
			return REQ_PREV_FIELD;
	case KEY_EXIT:
		return NCursesForm::virtualize(CTRL('X'));
	case ERR:
		return REQ_OVL_MODE;
	default:
		break;
	}
	return NCursesForm::virtualize(c);

}

int CMyForm::getKey()
{
	//unsigned char key;
	int ret;
	
	if(m_keys.size() > 0){
		ret =  m_keys.front();
		m_keys.pop();
		return ret;
	}
	return getch();
}

int CMyForm::onSave()
{
	doSet();
	int nRet = 0;
	switch(m_chOper)
	{
	case '+': nRet = m_n1 + m_n2; break;					
	case '-': nRet = m_n1 - m_n2; break;	
	case '*': nRet = m_n1 * m_n2; break;
	case '/':
	{
		if(m_n2 != 0 )
			nRet = (int)(m_n1 / m_n2);
		else
		{
			CMessageBox msg("Numeral2 can't be 0!");
			msg();
			return -1;
 		}
		break;
	}
	default:
		CMessageBox msg("Error Operator!");	
		msg();
		return -1;
	}
	char s[20];
	sprintf(s,"The result is %d", nRet);
	CMessageBox msg(s);
	msg();

	return nRet;
}

int CMyForm::doGet()
{
	//UpdateData(FALSE)
	m_pFormField[2]->set_value(IntegerToAnsi(m_n1));
	m_pFormField[6]->set_value(IntegerToAnsi(m_n2));
	m_pFormField[4]->set_value(&m_chOper);
	
	return 0;
}
int CMyForm::doSet()
{
	//UpdateData(TRUE)
	m_n1 = atoi(m_pFormField[2]->value());
	m_n2 = atoi(m_pFormField[6]->value());
	m_chOper = *(m_pFormField[4]->value());

	return 0;
}





