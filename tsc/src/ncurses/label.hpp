/**
 * label.hpp
 *
 * a Label
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2004-11-08 14:05:51 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_LABEL_HPP
#define INCLUDED_LABEL_HPP

#include <stddef.h>
#include <cursesf.h>
/**
 * 一个Label
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
