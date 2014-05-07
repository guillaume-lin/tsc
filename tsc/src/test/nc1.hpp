#include <stdlib.h>
#include <cursesapp.h>
#include <cursesf.h>
#include <stdlib.h>
#include <queue>

#include "label.hpp"
#include "field.hpp"
#include "integerchecker.hpp"
#include "messagebox.hpp"

class COperatorFieldType : public UserDefinedFieldType{
public:	
	COperatorFieldType()
	{
		//m_chList = chList;
	}
	
	virtual bool char_check(int c)
	{
		for(int i = 0; i < 4; i++)
		{
			if(*(chList[i]) == c)
				return true;
		}
		return false;
	}
	virtual bool field_check(NCursesFormField& f)
	{
		//char* str = f.value();
		//for(int i = 0; i < m_nCount; i++)
		//{
		//	if(!strcmp(m_chList[i], str))
				return true;
		//}
		//return false;
	}

private:
	static char* chList[4];
};
char* COperatorFieldType::chList[4] ={"+", "-", "*", "/"}; 

class CMyForm : public NCursesForm{

public:	
	CMyForm();
	~CMyForm();
	virtual int virtualize(int c);
	virtual int getKey();

	virtual int doGet();
	virtual int doSet();
	virtual int onSave();

protected:
	UserDefinedFieldType** m_pChecker; 
	NCursesFormField** m_pFormField;
private:
	int m_n1;
	int m_n2;
	char m_chOper;
	std::queue<int> m_keys;
	static const char* chList[]; 
};


class CApp : public NCursesApplication{
public:
	CApp():NCursesApplication(TRUE)
	{
	}
	
        int run()
	{
		CMyForm F;
		F();
		return 0;
	}
};

static CApp app;


