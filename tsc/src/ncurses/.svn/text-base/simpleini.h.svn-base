/* @(#)simpleini.h
 *
 * 
 *
 * Company: Xiamen Uni-Creative Information Technology Corp.
 * Author:  Lin Jingxian(аж╬╟ом)
 * Email:   cckk_lin@yahoo.ie
 * Date:    2005-11-15 15:35:53
 *
 * $log$
 *
 */

#ifndef INCLUDED_SIMPLEINI_H
#define INCLUDED_SIMPLEINI_H
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_STRING_LENGTH 255
	typedef struct {
		char m_string[MAX_STRING_LENGTH];
		char m_section[MAX_STRING_LENGTH];
		char* m_ini_buffer;
	} simple_ini_t;

	int simple_ini_open(simple_ini_t* self, char* file);
	int simple_ini_close(simple_ini_t* self);
	int simple_ini_load_from(simple_ini_t* self,char* file);
	int simple_ini_save_as(simple_ini_t* self);

	int simple_ini_set_section(simple_ini_t* self, const char* section);
	char* simple_ini_get_section(simple_ini_t* self);	
	char* simple_ini_get_string(simple_ini_t* self,const char* name);
	int simple_ini_set_string(simple_ini_t* self, const char* name,
				  const char* value);
	int simple_ini_get_int(simple_ini_t* self, const char* name);
	int simple_ini_set_int(simple_ini_t* self, const char* name, int i);


#ifdef __cplusplus
};
#endif

#endif /* INCLUDED_SIMPLEINI_H */

