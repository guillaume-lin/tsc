/**
 * label.hpp
 *
 * a Label
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2004-11-08 14:05:51 �й���׼ʱ��
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_LABEL_HPP
#define INCLUDED_LABEL_HPP

#include <stddef.h>
#include <cursesf.h>
/**
 * һ��Label
 */
class CLabel : public NCursesFormField
{
	public:
		CLabel(const char* title,int row,int col):
			NCursesFormField(1,(int)::strlen(title),row,col)
			{
				set_value(title);
				options_off(O_EDIT|O_ACTIVE);
			};
};
#endif
