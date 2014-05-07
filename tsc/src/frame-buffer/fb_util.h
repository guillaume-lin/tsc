/**
 * fb_util.h
 *
 * utility to operate on linux frame buffer
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-25 09:52:11 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_FB_UTIL_H
#define INCLUDED_FB_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/fb.h>

#define MAX_FB_NAME_LEN 12

typedef struct {
	int m_fd;
	char m_name[MAX_FB_NAME_LEN];
	struct fb_fix_screeninfo m_fixed_info;
	struct fb_var_screeninfo m_var_info;
	struct fb_cmap m_colormap;

	unsigned char* m_mem_start;	// 帧缓冲的起始地址
	long m_mem_len;			// 帧缓冲的长度
	unsigned char* m_screen_mem_start;//当前帧的起始地址
	long m_screen_mem_len;          // 单屏帧缓冲的大小
} fb_device_t;

/**
 * 打开并初始化指定的设备
 */
extern int fb_open(fb_device_t* dev,char* name);
extern int fb_set_screen_count(fb_device_t* dev, int screen);
extern int fb_get_width(fb_device_t* dev);
extern int fb_get_height(fb_device_t* dev);

/**
 * 关闭设备
 */
extern int fb_close(fb_device_t* dev);

/**
 * 切换屏幕
 */
extern int fb_switch_to_screen(fb_device_t* dev, int screen);
/**
 * 放置像素到指定位置
 */
extern int fb_put_pixel(fb_device_t* dev,int x, int y,unsigned int pixel);
extern int fb_get_pixel(fb_device_t* dev,int x, int y,unsigned int* pixel);

/**
 * 显示指定的屏幕内容
 */
extern int fb_show_screen(fb_device_t* dev, int screen);

/**
 * 上下滚屏
 * @param offset 为正数表示向下滚,为负数表示向上滚,单位为像素点
 *
 */
extern int fb_scroll_screen(fb_device_t* dev, int offset);



#ifdef __cplusplus
};
#endif

#endif
