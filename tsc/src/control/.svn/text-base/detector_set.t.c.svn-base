/**
 * detector_set.t.c
 * 
 * test for detector set
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-18 17:11:33 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <assert.h>
#include "detector_set.h"


int main(int argc, char* argv[])
{
	int i;
	detector_set_t set;
	empty_set(&set);
	add_set(1,&set);
	add_set(20,&set);
	add_set(1,&set);
	add_set(3,&set);
	for(i=0; i<32; i++){
		if(is_in_set(i+1,&set)){
			printf("%d\n",i+1);
		}
	}
	return 0;
}
