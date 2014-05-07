/**
 * main.cpp
 * 
 * App entry for NCurses 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-15 10:45:43 中国标准时间
 * 
 * $log$
 *
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ncurses.h>
#include <iostream>
#include <cursesapp.h>
#include "mainmenu.hpp"
#include "../control/vm.h"
#include "../control/io.h"
#include "../frame-buffer/show_bmp.h"
#include "../frame-buffer/fb_util.h"
#include "ShowSystemTimeform.hpp"

int showbmp(const char* strImageName, char* fb)
{
	fb_device_t dev;
	s_ImageInfo ImageInfo;
	if(fb_open(&dev,fb)!=0)
	{
		printf("open %s failed\n",fb);
		return -1;
	}
	if(load_bmp_file(strImageName,&ImageInfo)!=0)
	{
		fb_close(&dev);	
		printf("read bmp(%s) data failed",strImageName);
		return -1;
	}
	fb_switch_to_screen(&dev,2);	

	fb_bitblt(&dev,&ImageInfo);
	fb_show_screen(&dev,2);
	sleep(3);
	fb_show_screen(&dev,0);	
	fb_close(&dev);
	free(ImageInfo.pData);
	return 0;
}
int resetfb(char* fb)
{
	fb_device_t dev;
	if(fb_open(&dev,fb)!=0)
	{
		printf("open %s failed.\n",fb);
		return -1;
	}
	fb_set_screen_count(&dev,1);
	int w = fb_get_width(&dev);
	int h = fb_get_height(&dev);
	for(int i=0; i<w; i++)
		for(int j=0;j<h;j++)
			fb_put_pixel(&dev,w,h,0);	
	fb_close(&dev);
	return 0;
}

class App : public NCursesApplication
{
protected:
	int titlesize() const {return 1;};
	void title()
	{
	}
public:
	App():NCursesApplication(TRUE)
	{
	}
	int run()
	{
		def_prog_mode();
		endwin();
		//显示图片
		//showbmp("/bin/gxlogo.bmp","/dev/fb");

		reset_prog_mode();
		refresh();
		try{
			vm_init(&g_vm);
			g_vm.m_fd = open_comm(2);
			while(true){
				// 显示信号机运行状态
				resetfb("/dev/fb");
				CShowSystemTimeform status;
				status();
				if(status.isExitPressed()) // lets exit to restart
					break;
		
				CMainMenu M;
				M();
			}
		}catch(NCursesException& nex){
			std::cout<<nex.classname()<<":"<<nex.message<<std::endl;
		}catch(std::exception& ex){
			std::cout<<ex.what()<<std::endl;
		}catch(...){
			std::cout<<"unknown exception"<<std::endl;
		}

		return 0;
	}

};
static App app;
