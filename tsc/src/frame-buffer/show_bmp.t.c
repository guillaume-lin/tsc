/**
 * fb_util.t.c
 * 
 * test for fb_util
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-25 11:16:12 �й���׼ʱ��
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
	show_bmp(argv[1],atoi(argv[2]),argv[3]);
	return 0;
	int i,j;
	int ret;
	fb_device_t dev;
//	BITMAP_T* bmp;
//	int px,py;
	s_ImageInfo ImageInfo;
	ret = fb_open(&dev,"/dev/fb0");
//	InitCharLib("./hzlib.zlb");
//	InitGDI(NULL);
	

//	SetColor(MakeColor(0,0,0));
//	Canvas_Circle(600,440,40);
//	Canvas_MoveTo(0,0);
//	Canvas_LineTo(640,480);
//	Canvas_TextOut(0,400,"����1234567890abcdefghijkl");

//	bmp = GetCanvas();

	printf("write to screen 0\n");
	fb_switch_to_screen(&dev,0);
	if(load_bmp_file("/home/ljx/logoimg.bmp",&ImageInfo)==0)
		fb_bitblt(&dev,&ImageInfo);

	printf("write to screen 2\n");
	fb_switch_to_screen(&dev,2);
	for(i=0;i<640;i++)
		for(j=0;j<480;j++){
			fb_put_pixel(&dev,i,j,65535);
		}
	printf("will switch screen\n");
	// switch screen
	while(true){
		i++;
		if(i > 2)
			i = 0;
		printf("switch to screen %d\n",i);
		fb_show_screen(&dev,0);
		sleep(1);
	}
	assert(fb_close(&dev) == 0);
	free(ImageInfo.pData);
	return 0;
}
