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
#include <unistd.h>
#include <assert.h>
#include "fb_util.h"
#include "zk.h"
#include "gdi.h"


int main(int argc, char* argv[])
{
	int i,j;
	int ret;
	fb_device_t dev;
	BITMAP_T* bmp;
	int px,py;

	ret = fb_open(&dev,"/dev/fb0");
	assert(ret == 0);

	//dev.m_mem_start

	InitCharLib("./hzlib.zlb");
	InitGDI(NULL);
	

	
	SetColor(MakeColor(0,0,0));
	//	Canvas_Circle(600,440,40);
	Canvas_MoveTo(0,0);
	Canvas_LineTo(640,480);
	Canvas_TextOut(0,400,"在这1234567890abcdefghijkl");

	bmp = GetCanvas();

	printf("write to screen 0\n");
	fb_switch_to_screen(&dev,0);
	for(i=0;i<640;i++)
		for(j=0;j<480;j++){
			fb_put_pixel(&dev,i,j,65535);
		}
#if 0
	printf("write to screen 1\n");
	fb_switch_to_screen(&dev,1);
	for(i=0;i<640;i++)
		for(j=0;j<480;j++){
			fb_put_pixel(&dev,i,j,65535);
		}
#endif
	printf("write to screen 2\n");
	fb_switch_to_screen(&dev,2);
	for(i=0;i<640;i++)
		for(j=0;j<480;j++){
			char R,G,B;
			short pixel;
			GetPixel(i,j,bmp,&R,&G,&B);
			Encode565(MakeColor(R,G,B),&pixel);
			fb_put_pixel(&dev,i,j,pixel);
		}
	printf("will switch screen\n");
#if 1
	// switch screen
	while(true){
		i++;
		if(i > 2)
			i = 0;
		printf("switch to screen %d\n",i);
		fb_show_screen(&dev,i);
		sleep(1);
	}
#else
	// scroll screen
	for(i=0;i<480;i++){
		fb_scroll_screen(&dev,i*10);
		sleep(2);
	}
#endif
#if 0
	for(px=0;px<590;px+=48)
		for(py=0;py<430;py+=48){
			bmp = GetCanvas();
			for(i=0;i<640;i++)
				for(j=0;j<480;j++)
					PutPixel(i,j,MakeColor(128,128,128));

			Canvas_TextOut(px,py,"在这");
			
			for(i=0;i<640;i++)
				for(j=0;j<480;j++){
					char R,G,B;
					short pixel;
					GetPixel(i,j,bmp,&R,&G,&B);
					Encode565(MakeColor(R,G,B),&pixel);
					fb_put_pixel(&dev,pixel,i,j);
				}
			sleep(1);
		}
#endif

	assert(fb_close(&dev) == 0);

	return 0;
}

