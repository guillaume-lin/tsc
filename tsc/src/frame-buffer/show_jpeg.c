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
#include <setjmp.h>
#include <assert.h>
#include "show_jpeg.h"
#include "jpeglib.h"
#include "fb_util.h"
typedef struct 
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
} my_error_mgr;
static void my_error_exit(j_common_ptr cinfo)
{
	my_error_mgr* myerr_p = (my_error_mgr*)cinfo->err;

	(*cinfo->err->output_message)(cinfo);

	longjmp(myerr_p->setjmp_buffer,1);
}

static int loadFromJpegFile(const char* fileName,unsigned char* buf, int len)
{

	struct jpeg_decompress_struct cinfo;

	my_error_mgr jerr;

	FILE * infile;
	JSAMPARRAY buffer;
	int row_stride;

	if((infile = fopen(fileName, "rb")) == NULL)
	{
		// can not open file
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if(setjmp(jerr.setjmp_buffer))
	{
		// decompress failed
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return -1;
	}

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo,infile);

	(void)jpeg_read_header(&cinfo, TRUE);

	// customize output format
	
	cinfo.output_components = 3;
	cinfo.out_color_components = 3;
	cinfo.out_color_space = JCS_RGB;
	

	(void)jpeg_start_decompress(&cinfo);
	
	row_stride = cinfo.output_width * cinfo.output_components;
	if(row_stride > 640*3)
		return -1;
	else
		row_stride = 640*3;

	buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);


	while(cinfo.output_scanline < cinfo.output_height)
	{
		(void)jpeg_read_scanlines(&cinfo,buffer,1);
		memcpy(buf,buffer[0],row_stride);
		buf+= row_stride;
	}
	
	(void) jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	fclose(infile);
	
	return 0;
}

int show_image(const char* strImageName)
{
	fb_device_t dev;
	unsigned char buf[640*480*3];

	assert(fb_open(&dev,"/dev/fb0") == 0);
	assert(loadFromJpegFile(strImageName,buf,640*480*3) == 0);
	assert(fb_bitblt(&dev,0,0,640,480,buf,0,0,640,480,0) == 0);
	assert(fb_close(&dev) == 0);

	return 0;
}
