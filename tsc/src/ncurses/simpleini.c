/* @(#)simpleini.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simpleini.h"

int simple_ini_open(simple_ini_t* self, char* file)
{
	FILE* f;
	long len;
	memset(self->m_section,0,MAX_STRING_LENGTH);
	memset(self->m_string,0,MAX_STRING_LENGTH);
	f = fopen(file,"r");
	if(fseek(f,0,SEEK_END) != 0){
		fclose(f);
		return -1;
	}

	len = ftell(f);
	self->m_ini_buffer = (char*)malloc(len);
	if(self->m_ini_buffer == NULL){
		fclose(f);
		return -1;
	}
	rewind(f);
	if(fread(self->m_ini_buffer,len,1,f) != 1){
		fclose(f);
		return -1;
	}
	fclose(f);
	return 0;
}
int simple_ini_close(simple_ini_t* self)
{
	free(self->m_ini_buffer);
	return 0;
}
int simple_ini_load_from(simple_ini_t* self,char* file)
{
	return simple_ini_open(self,file);
}
int simple_ini_save_as(simple_ini_t* self)
{
	return 0;
}

int simple_ini_set_section(simple_ini_t* self, const char* section)
{
	int len;
	int l;
	l = strlen(section);
	len = l > MAX_STRING_LENGTH - 4 ? MAX_STRING_LENGTH - 4: l;
	
	self->m_section[0] = '[';
	strncpy(self->m_section+1,section,len);
	self->m_section[len+1] = ']';
	self->m_section[len+2] = '\0';
	return len;
}
char* simple_ini_get_section(simple_ini_t* self)
{
	return self->m_section;
}
char* simple_ini_get_value(simple_ini_t* self, const char* name)
{
	char* sb,*se;
	char* value;
	char* b,*e;
	char* posn, *posr;
	sb = strstr(self->m_ini_buffer,self->m_section);
	if(sb == NULL)
		return NULL;
	se = strchr(sb+1,'[');
	value = strstr(sb,name);

	if(se != NULL && se < value)
		return NULL;
	if(value == NULL)
		return NULL;

	b = strchr(value,'=');
	posn = strchr(value,'\n');
	posr = strchr(value,'\r');
	e = posn < posr ? posn : posr;
	if(b == NULL || e == NULL)
		return NULL;
	strncpy(self->m_string,b+1,e-b-1);
	self->m_string[e-b-1] = '\0';
	return self->m_string;
}
char* simple_ini_get_string(simple_ini_t* self,const char* name)
{
	return simple_ini_get_value(self,name);
}
int simple_ini_set_string(simple_ini_t* self, const char* name,
				  const char* value)
{
	return -1;
}
int simple_ini_get_int(simple_ini_t* self, const char* name)
{
	char* s;
	s = simple_ini_get_value(self,name);
	if(s != NULL)
		return atoi(s);
	else
		return -1;
}
int simple_ini_set_int(simple_ini_t* self, const char* name, int i)
{
	return -1;
}

