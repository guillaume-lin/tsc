/* @(#)baseform.hpp
 */

#ifndef _BASEFORM_H
#define _BASEFORM_H 1
#include <queue>
#include <stdlib.h>

#include <stddef.h>
#include <cursesf.h>
#include <cursesm.h>
#include "../control/vm.h"
#include "../control/datastruct.h"
#include "fieldmonitor.hpp"
#include "integerchecker.hpp"

extern const char* itoa(int i);
extern vm_t g_vm;

class CBaseForm : public NCursesForm 
{
public:
	CBaseForm(int h, int w, int y, int x);
	virtual ~CBaseForm();
	virtual int doGet(){return 0;};
	virtual int doSet() {return 0;};
	virtual int onSave();
	virtual int onLoad();
	virtual int onPageUp();
	virtual int onPageDown();
	virtual int onFieldChanged(NCursesFormField& f);
	// ��������
	virtual int getKey();
	virtual int virtualize(int c);
protected:
	NCursesFormField** F;
	int postKey(int key);
private:
	CBaseForm();
	CBaseForm(const CBaseForm& rhs);
	std::queue<int> m_keys;
};


#endif /* _BASEFORM_H */

