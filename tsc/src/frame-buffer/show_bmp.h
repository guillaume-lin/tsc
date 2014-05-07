#ifndef INCLUDED_SHOW_MYBMP_H
#define INCLUDED_SHOW_MYBMP_H

#ifdef __cplusplus
	extern "C" {
#else
	#define bool int
	#define true 1
	#define false 0
#endif
#include "fb_util.h"
#include "gdi.h"
typedef struct tagBITMAPFILEHEADER {
        unsigned short   bfType;
        unsigned long    bfSize;
        unsigned short   bfReserved1;
        unsigned short   bfReserved2;
        unsigned long    bfOffBits;
}BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER{
        unsigned long      biSize;
        long               biWidth;
        long  				     biHeight;
        unsigned short  	 biPlanes;
        unsigned short     biBitCount;
        unsigned long      biCompression;
        unsigned long      biSizeImage;
        long               biXPelsPerMeter;
        long               biYPelsPerMeter;
        unsigned long      biClrUsed;
        unsigned long      biClrImportant;
}BITMAPINFOHEADER;
typedef struct tagImageInfo
{
	int nWidth;
	int nHeight;
	unsigned char* pData;  // RGB图像数据
	long        	ldata_len;//nWidth*nHeight*3(24bit色)
}s_ImageInfo;
typedef struct tagImageInfo16Bit
{
	int nWidth;
	int nHeight;
	unsigned char* pData;  // RGB图像数据
	long        	ldata_len;//nWidth*nHeight*2(16bit色)
}s_ImageInfo16Bit;


int load_bmp_file(const char* file,s_ImageInfo* image);
int load_bmp_file_16bit(const char* file,s_ImageInfo16Bit* image);
int fb_bitblt(fb_device_t *pDev,s_ImageInfo *pImageInfo);
int fb_bitblt_16bit(fb_device_t *pDev,s_ImageInfo16Bit *pImageInfo);
int show_bmp(char* fbdev, int secs, const char* strImageName);
int show_bmp_16bit(char* fbdev, int secs, const char* strImageName);
#ifdef __cplusplus
	};
#endif

#endif
