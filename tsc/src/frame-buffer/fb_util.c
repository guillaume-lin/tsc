/**
 * fb_util.c
 * 
 * utility to operate on a linux frame buffer
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-25 09:51:53 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <asm/page.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/page.h>
#include <linux/fb.h>
#include "fb_util.h"

void print_var_info(struct fb_var_screeninfo* var)
{
	printf("var->xres = %d\n",var->xres);
	printf("var->yres = %d\n",var->yres);
	printf("var->xres_virtual = %d\n",var->xres_virtual);
	printf("var->yres_virtual = %d\n",var->yres_virtual);
	printf("var->xoffset = %d\n",var->xoffset);
	printf("var->bits_per_pixel = %d\n",var->bits_per_pixel);
	printf("var->nonstd = %d\n",var->nonstd);
}
/**
 * 打开并初始化指定的设备
 */
int fb_open(fb_device_t* dev,char* name)
{
	dev->m_mem_start = NULL;
	// 打开设备
	dev->m_fd = open(name,O_RDWR);
	if(dev->m_fd < 0){
		printf("Error opening %s.\n",name);
		perror(name);
		return -1;
	}

	// 获取设备信息
	if (ioctl(dev->m_fd, FBIOGET_FSCREENINFO, &dev->m_fixed_info) < 0) {
		printf("Unable to retrieve fixed screen info: %s\n",
				strerror(errno));
		close(dev->m_fd);
		return -1;
	}

	assert(dev->m_fixed_info.visual == FB_VISUAL_TRUECOLOR);

	/* Now get the variable screen info.
	   This contains more info about the current video mode.
	   (Note that this is effectively fixed on some framebuffer
	   devices -- many don't support mode switching.) */

	if (ioctl(dev->m_fd, FBIOGET_VSCREENINFO, &dev->m_var_info) < 0) {
		printf("Unable to retrieve variable screen info: %s\n",
				strerror(errno));
		close(dev->m_fd);
		return -1;
	}
	assert(dev->m_var_info.bits_per_pixel == 16);
	assert(dev->m_var_info.xres == 640);
	assert(dev->m_var_info.yres == 480);

	// print_var_info(&dev->m_var_info);
	// 设置虚拟屏幕为三个缓冲区
	dev->m_var_info.yres_virtual = dev->m_var_info.yres*3;
	if (ioctl(dev->m_fd, FBIOPUT_VSCREENINFO, &dev->m_var_info) < 0) {
		printf("Unable to set three screen buffer: %s\n",
				strerror(errno));
		dev->m_var_info.yres_virtual = dev->m_var_info.yres;
		dev->m_screen_mem_len = 0;
	}else{
		// 每个象素两个字节
		dev->m_screen_mem_len = dev->m_var_info.xres*dev->m_var_info.yres*2;
	}
	// 映射设备空间到进程的地址空间
	dev->m_mem_start = mmap(NULL,
			dev->m_fixed_info.smem_len,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev->m_fd,
			0);
	if ((int)dev->m_mem_start == -1) {
		printf("Unable to mmap framebuffer: %s\n",
				strerror(errno));
		close(dev->m_fd);
		dev->m_mem_start = NULL;
		return -1;
	}

	dev->m_mem_len = dev->m_fixed_info.smem_len;
	
	dev->m_screen_mem_start = dev->m_mem_start;

	return 0;
}
int fb_set_screen_count(fb_device_t* dev, int count)
{
	dev->m_var_info.yres_virtual = dev->m_var_info.yres*count;
	if (ioctl(dev->m_fd, FBIOPUT_VSCREENINFO, &dev->m_var_info) < 0) {
		printf("Unable to set %d screen : %s\n",count,
				strerror(errno));
		dev->m_var_info.yres_virtual = dev->m_var_info.yres;
		dev->m_screen_mem_len = 0;
		return -1;
	}else{
		// 每个象素两个字节
		dev->m_screen_mem_len = dev->m_var_info.xres*dev->m_var_info.yres*2;
	}
	return 0;
}
int fb_get_width(fb_device_t* dev)
{
	return dev->m_var_info.xres;
}
int fb_get_height(fb_device_t* dev)
{
	return dev->m_var_info.yres;
}
int fb_switch_to_screen(fb_device_t* dev, int screen)
{
	if(screen < 0 || screen > 2)
		screen = 0;
	dev->m_screen_mem_start = dev->m_mem_start + 
		dev->m_screen_mem_len * screen;
	return 0;
}
/**
 * 放置像素到指定的位置
 */
int fb_put_pixel(fb_device_t* dev, int x, int y, unsigned int pixel)
{
	if(x < 0 || x > dev->m_var_info.xres)
		return -1;
	if(y < 0 || y > dev->m_var_info.yres)
		return -1;

	*((u_int16_t*)((u_int8_t*)dev->m_screen_mem_start + dev->m_fixed_info.line_length * y + 2*x)) = (u_int16_t)pixel;
	return 0;
}

/**
 *  读取指定位置的像素
 */
int fb_get_pixel(fb_device_t* dev,int x, int y,unsigned int* pixel)
{
	if(x < 0 || x > dev->m_var_info.xres)
		return -1;
	if(y < 0 || y > dev->m_var_info.yres)
		return -1;

	(*pixel) = *((u_int16_t*)((u_int8_t*)dev->m_screen_mem_start + dev->m_fixed_info.line_length * y + 2*x));
	return 0;
}
/**
 * 显示帧缓冲内容
 */
int fb_show_screen(fb_device_t* dev,int screen)
{
	if(screen < 0 || screen > 2 || dev->m_screen_mem_len == 0 )
		screen = 0;
	
	dev->m_var_info.xoffset = 0;
	dev->m_var_info.yoffset = dev->m_var_info.yres*screen;
		

	// FBIOPAN_DISPLAY
	if (ioctl(dev->m_fd, FBIOPAN_DISPLAY, &dev->m_var_info) < 0) {
		printf("Unable to pan display: %s\n",
				strerror(errno));
		return -1;
	}

	return 0;
}
/**
 *  滚屏
 *
 */
int fb_scroll_screen(fb_device_t* dev, int offset)
{
	dev->m_var_info.yoffset += offset;

	if(dev->m_var_info.yoffset < 0 || 
	   dev->m_var_info.yoffset > 
	   (dev->m_var_info.yres_virtual - dev->m_var_info.yres))
		dev->m_var_info.yoffset = 0;

	// FBIOPAN_DISPLAY
	if (ioctl(dev->m_fd, FBIOPAN_DISPLAY, &dev->m_var_info) < 0) {
		printf("Unable to pan display: %s\n",
				strerror(errno));
		return -1;
	}
	return 0;
}
/**
 * 关闭设备
 */
int fb_close(fb_device_t* dev)
{
	// 断开内存映射
	if(dev->m_mem_start != NULL)
		munmap(dev->m_mem_start,dev->m_mem_len);
	return 0;
}


