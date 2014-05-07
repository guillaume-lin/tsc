/* @(#)field.hpp
 */

#ifndef _FIELD_H
#define _FIELD_H 1

#include <stddef.h>

#include <cursesf.h>

class CField : public NCursesFormField
{
	public:
		CField(int cols,int row,int col):
			NCursesFormField(1,cols,row,col)
			{
				options_on(O_EDIT|O_ACTIVE);
			};
};

#endif /* _FIELD_H */

