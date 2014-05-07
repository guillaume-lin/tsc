/**
 * fb_util.t.c
 * 
 * test for fb_util
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-25 11:16:12 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "fb_util.h"
#include "zk.h"
#include "gdi.h"
#include "show_bmp.h"


int main(int argc, char* argv[])
{
	if(argc < 4){
		printf("Usage: %s <fbdev> <sec> <bmp>\n",argv[0]);
		return -1;
	}
	show_bmp_16bit(argv[1],atoi(argv[2]),argv[3]);
	return 0;
}

