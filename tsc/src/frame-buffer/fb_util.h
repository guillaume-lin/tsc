/**
 * fb_util.h
 *
 * utility to operate on linux frame buffer
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-25 09:52:11 �й���׼ʱ��
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

	unsigned char* m_mem_start;	// ֡�������ʼ��ַ
	long m_mem_len;			// ֡����ĳ���
	unsigned char* m_screen_mem_start;//��ǰ֡����ʼ��ַ
	long m_screen_mem_len;          // ����֡����Ĵ�С
} fb_device_t;

/**
 * �򿪲���ʼ��ָ�����豸
 */
extern int fb_open(fb_device_t* dev,char* name);
extern int fb_set_screen_count(fb_device_t* dev, int screen);
extern int fb_get_width(fb_device_t* dev);
extern int fb_get_height(fb_device_t* dev);

/**
 * �ر��豸
 */
extern int fb_close(fb_device_t* dev);

/**
 * �л���Ļ
 */
extern int fb_switch_to_screen(fb_device_t* dev, int screen);
/**
 * �������ص�ָ��λ��
 */
extern int fb_put_pixel(fb_device_t* dev,int x, int y,unsigned int pixel);
extern int fb_get_pixel(fb_device_t* dev,int x, int y,unsigned int* pixel);

/**
 * ��ʾָ������Ļ����
 */
extern int fb_show_screen(fb_device_t* dev, int screen);

/**
 * ���¹���
 * @param offset Ϊ������ʾ���¹�,Ϊ������ʾ���Ϲ�,��λΪ���ص�
 *
 */
extern int fb_scroll_screen(fb_device_t* dev, int offset);



#ifdef __cplusplus
};
#endif

#endif
