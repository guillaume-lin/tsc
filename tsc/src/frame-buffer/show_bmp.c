/**
 * show_jpeg.c
 * 
 * use jpeg lib to show jpeg file on frame buffer
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-25 18:01:29 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <assert.h>
#include <errno.h>
#include "show_bmp.h"
int BGRFromRGB(unsigned char *buf, unsigned int  widthPix, unsigned int height)
{
	if (!buf)
		return -1;

	unsigned int col, row;
	for (row=0;row<height;row++)
	{
		for (col=0;col<widthPix;col++) 
		{
			unsigned char * pRed;
			unsigned char * pGrn;
			unsigned char * pBlu;
			pRed = buf + row * widthPix * 3 + col * 3;
			pGrn = buf + row * widthPix * 3 + col * 3 + 1;
			pBlu = buf + row * widthPix * 3 + col * 3 + 2;

			// swap red and blue
			unsigned char tmp;
			tmp = *pRed;
			*pRed = *pBlu;
			*pBlu = tmp;
		}
	}
	return 0;
}
int load_bmp_file(const char* file,s_ImageInfo* image)
{
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;	
	FILE * infile;
	int    nSize;
	if((infile = fopen(file, "rb")) == NULL)
	{
		// can not open file
		printf("use rb mode open %s file fail，errorNo:%d",file,errno);
		return -1;
	}
	printf("unsigned long=%d,unsigned short=%d,long=%d\n\r",	
				 sizeof(unsigned long),sizeof(unsigned short),sizeof(long));
	printf("sizeof(BITMAPFILEHEADER)=%d,sizeof(BITMAPINFOHEADER)=%d\n\r",
					sizeof(BITMAPFILEHEADER),
					sizeof(BITMAPINFOHEADER));
	fread((void*)&fh,14,1,infile);	
	fread((void*)&ih,sizeof(BITMAPINFOHEADER),1,infile);	
	printf("Height=%ld,Width=%ld\n\r",ih.biHeight,ih.biWidth);
	if(fh.bfType != 0x4d42)	// BM
	{
		printf("%s not is  bmp file，DataTiltle not 0x4d42 flag",file);
		fclose(infile);
		return -1;
	}	
	if(ih.biBitCount != 24 || ih.biCompression != 0)
	{
		// unsupported file format
		printf("%s file is not 24bit bmp format\n",file);
		printf("biBitCount=%x,biCompression=%x\n",
					 ih.biBitCount,ih.biCompression);

		fclose(infile);
		return -1;
	}
	nSize = ih.biWidth * ih.biHeight * 3;
	image->ldata_len=nSize;
	image->nHeight=ih.biHeight;
	image->nWidth=ih.biWidth;
	image->pData = (unsigned char *)malloc(nSize);
	if(image->pData == NULL)
	{
		printf("load_bmp_file menory fail,errorNo:%d",errno);
		fclose(infile);
		return -1;
	}
	fread((void*)image->pData,sizeof(unsigned char)*nSize,1,infile);
	fclose(infile);
	BGRFromRGB(image->pData,image->nWidth,image->nHeight);
	printf("read bmp file %s success!!",file);	
	return 0;
}
short rgb555_to_rgb565(short rgb555)
{
	short r = (rgb555 >> 10) & 0x1F;
	short g = (rgb555 >> 5) & 0x1F;
	short b = (rgb555 >> 0) & 0x1F;
	short rgb565 = (r << 11) | ( g << 5 ) | b;
	return rgb565;
 
}
int fb_bitblt_16bit(fb_device_t *pDev,s_ImageInfo16Bit *pImageInfo)
{
	int i=0;
	int j=0;
	int nWidth=0;
	int nHeight=0;
	unsigned char uHigh;
	unsigned char uLow;

	int which;
	if(fb_get_width(pDev)>pImageInfo->nWidth)
			nWidth=pImageInfo->nWidth;
	else
		  nWidth=fb_get_width(pDev);
	if(fb_get_height(pDev)>pImageInfo->nHeight)
			nHeight=pImageInfo->nHeight;
	else
		  nHeight=fb_get_height(pDev);

	for(i=0;i<nHeight;i++)
	{
		for(j=0;j<nWidth;j++)
		{
			which= (i*pImageInfo->nWidth +j)*2;
			uHigh=pImageInfo->pData[which];
			uLow=pImageInfo->pData[which+1];
			short pixel = uLow;
			pixel = pixel << 8 ;
			pixel |= uHigh;
			pixel = rgb555_to_rgb565(pixel);
			int ny=abs(i-nHeight);
			fb_put_pixel(pDev,j,ny,pixel);
		}
	}
	return 0;
}	
int fb_bitblt(fb_device_t *pDev,s_ImageInfo *pImageInfo)
{
	int i=0;
	int j=0;
	int nWidth=0;
	int nHeight=0;
	unsigned char uRed;
	unsigned char uBlue;
	unsigned char uYellow;
	int which;
	if(fb_get_width(pDev)>pImageInfo->nWidth)
			nWidth=pImageInfo->nWidth;
	else
		  nWidth=fb_get_width(pDev);
	if(fb_get_height(pDev)>pImageInfo->nHeight)
			nHeight=pImageInfo->nHeight;
	else
		  nHeight=fb_get_height(pDev);
	for(i=0;i<nHeight;i++)
	{
		for(j=0;j<nWidth;j++)
		{
			which= (i*pImageInfo->nWidth +j)*3;
			uRed=pImageInfo->pData[which];
			uYellow=pImageInfo->pData[which+1];
			uBlue=pImageInfo->pData[which+2];
			//uRed = uRed >>3;
                        //uYellow = uYellow>>2;
			//uBlue =uBlue >>3;
			short pixel;
			Encode565(MakeColor(uRed,uYellow,uBlue),&pixel);
			int ny=abs(i-nHeight);
			fb_put_pixel(pDev,j,ny,pixel);
		}
	}
	return 0;
}	

static void dumpfh(BITMAPFILEHEADER* fh)
{
	printf("bitmap file header:\n");
	printf("bfType:      %d\n",fh->bfType);
	printf("bfSize:      %d\n",fh->bfSize);
	printf("bfReserved1: %d\n",fh->bfReserved1);
	printf("bfReserved2: %d\n",fh->bfReserved2);
	printf("bfOffBits:   %d\n",fh->bfOffBits);
}

static void dumpih(BITMAPINFOHEADER* ih)
{
	printf("bitmap info header:\n");
	printf("biSize:           %d\n",ih->biSize);
	printf("biWidth:          %d\n",ih->biWidth);
        printf("biHeight:         %d\n",ih->biHeight);
        printf("biPlanes:         %d\n",ih->biPlanes);
        printf("biBitCount:       %d\n",ih->biBitCount);
        printf("biCompression:    %d\n",ih->biCompression);
        printf("biSizeImage:      %d\n",ih->biSizeImage);
        printf("biXPelsPerMeter:  %d\n",ih->biXPelsPerMeter);
        printf("biYPelsPerMeter:  %d\n",ih->biYPelsPerMeter);
        printf("biClrUsed:        %d\n",ih->biClrUsed);
        printf("biClrImportant:   %d\n",ih->biClrImportant);
}

int load_bmp_file_16bit(const char* file,s_ImageInfo16Bit* image)
{
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;	
	FILE * infile;
	int    nSize;
	if((infile = fopen(file, "rb")) == NULL)
	{
		// can not open file
		printf("use rb mode open %s file fail，errorNo:%d",file,errno);
		return -1;
	}
	printf("unsigned long=%d,unsigned short=%d,long=%d\n",	
	 sizeof(unsigned long),sizeof(unsigned short),sizeof(long));
	printf("sizeof(BITMAPFILEHEADER)=%d,sizeof(BITMAPINFOHEADER)=%d\n",
					sizeof(BITMAPFILEHEADER),
					sizeof(BITMAPINFOHEADER));
	fread((void*)&fh,14,1,infile);	
	dumpfh(&fh);
	fread((void*)&ih,sizeof(BITMAPINFOHEADER),1,infile);	
	dumpih(&ih);
	if(fh.bfType != 0x4d42)	// BM
	{
		printf("%s not is  bmp file，DataTitle not 0x4d42 flag",file);
		fclose(infile);
		return -1;
	}	
	if(ih.biBitCount != 16 || ih.biCompression != 0)
	{
		// unsupported file format
		printf("%s file is not 16bit bmp format\n",file);
		printf("biBitCount=%x,biCompression=%x\n",
					 ih.biBitCount,ih.biCompression);

		fclose(infile);
		return -1;
	}
	nSize = ih.biWidth * ih.biHeight * 2;
	image->ldata_len=nSize;
	image->nHeight=ih.biHeight;
	image->nWidth=ih.biWidth;
	image->pData = (unsigned char *)malloc(nSize);
	if(image->pData == NULL)
	{
		printf("load_bmp_file menory fail,errorNo:%d",errno);
		fclose(infile);
		return -1;
	}
	fread((void*)image->pData,sizeof(unsigned char)*nSize,1,infile);
	fclose(infile);
	printf("read bmp file %s success!\n",file);	
	return 0;
}

int show_bmp_16bit(char* fb, int secs, const char* strImageName)
{
	fb_device_t dev;
	s_ImageInfo16Bit ImageInfo;
	if(fb_open(&dev,fb)!=0)
		return -1;
	if(load_bmp_file_16bit(strImageName,&ImageInfo)!=0)
	{
		fb_close(&dev);	
		return -1;
	}
	fb_switch_to_screen(&dev,2);	
	fb_bitblt_16bit(&dev,&ImageInfo);
	fb_show_screen(&dev,2);
	sleep(secs);
	fb_show_screen(&dev,0);
	fb_close(&dev);
	free(ImageInfo.pData);
	return 0;
}
int show_bmp(char* fb, int secs, const char* strImageName)
{
	fb_device_t dev;
	s_ImageInfo ImageInfo;
	if(fb_open(&dev,fb)!=0)
		return -1;
	if(load_bmp_file(strImageName,&ImageInfo)!=0)
	{
		fb_close(&dev);	
		return -1;
	}
	fb_switch_to_screen(&dev,2);	
	fb_bitblt(&dev,&ImageInfo);
	fb_show_screen(&dev,2);
	sleep(secs);
	fb_show_screen(&dev,0);
	fb_close(&dev);
	free(ImageInfo.pData);
	return 0;
}
