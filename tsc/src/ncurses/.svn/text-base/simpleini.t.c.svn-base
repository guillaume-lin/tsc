/* @(#)simpleini.t.c
 */
#include <stdio.h>
#include <assert.h>
#include "simpleini.h"


int main(int argc, char* argv[])
{
	char* file;
	char* s;
	int i;
	int lamp_count;
	int detector_count;
	simple_ini_t ini;
	if(argc > 1)
		file = argv[1];
	else
		file = "crossing.txt";

	
	assert(simple_ini_open(&ini,file) == 0);
	simple_ini_set_section(&ini,"general");
	lamp_count = simple_ini_get_int(&ini,"lamp_count");
	detector_count=simple_ini_get_int(&ini,"detector_count");
	printf("lamp_count=%d\n",lamp_count);
	printf("detector_count=%d\n",detector_count);
	for(i=0; i<lamp_count; i++){
		char buf[20];
		sprintf(buf,"lamp%d",(i+1));
		simple_ini_set_section(&ini,buf);
		printf("[%s]\n",buf);
		s = simple_ini_get_string(&ini,"x");
		printf("x=%s\n",s);
		s = simple_ini_get_string(&ini,"y");
		printf("y=%s\n",s);
		s = simple_ini_get_string(&ini,"r");
		printf("r=%s\n",s);
		s = simple_ini_get_string(&ini,"color");
		printf("color=%s\n",s);
	}
	for(i=0; i<detector_count; i++){
		char buf[20];
		sprintf(buf,"detector%d",(i+1));
		simple_ini_set_section(&ini,buf);
		printf("[%s]\n",buf);
		s = simple_ini_get_string(&ini,"x");
		printf("x=%s\n",s);
		s = simple_ini_get_string(&ini,"y");
		printf("y=%s\n",s);
		s = simple_ini_get_string(&ini,"w");
		printf("w=%s\n",s);
		s = simple_ini_get_string(&ini,"h");
		printf("h=%s\n",s);
		s = simple_ini_get_string(&ini,"color");
		printf("color=%s\n",s);
	}
	simple_ini_close(&ini);

	return 0;
}
