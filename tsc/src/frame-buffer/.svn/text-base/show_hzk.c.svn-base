/**
 * show_hzk.c
 * 
 * display each character in bpsf file
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-09-20 11:14:52 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "fb_util.h"



typedef struct {
	int m_width;
	int m_height;
	int m_char_count;
	int m_char_byte_len;
	int m_data_offset;
	FILE* m_file;
} bpsf_t;

int bpsf_open(bpsf_t* bpsf,char* file)
{
	unsigned char ch;
	unsigned char b1,b2,b3,b4;
	bpsf->m_file = fopen(file,"r+b");
	if(bpsf->m_file == NULL)
		return -1;

	assert(fread(&ch,1,1,bpsf->m_file) == 1);
	assert(ch == 0x36);
	assert(fread(&ch,1,1,bpsf->m_file) == 1);
	assert(ch == 0x04);
	assert(fread(&ch,1,1,bpsf->m_file) == 1);
	assert(ch == 0x04);
	assert(fread(&ch,1,1,bpsf->m_file) == 1);
	bpsf->m_height = ch;
	assert(fread(&ch,1,1,bpsf->m_file) == 1);
	bpsf->m_width = ch;

	assert(fread(&ch,1,1,bpsf->m_file) == 1);
	b1=ch;
	assert(fread(&ch,1,1,bpsf->m_file) == 1);
	b2=ch;
	assert(fread(&ch,1,1,bpsf->m_file) == 1);
	b3=ch;
	assert(fread(&ch,1,1,bpsf->m_file) == 1);
	b4=ch;

	bpsf->m_char_count = 0;
	bpsf->m_char_count |= b4;
	bpsf->m_char_count <<= 8;
	bpsf->m_char_count |= b3;
	bpsf->m_char_count <<= 8;
	bpsf->m_char_count |= b2;
	bpsf->m_char_count <<= 8;
	bpsf->m_char_count |= b1;

	bpsf->m_data_offset = 9;
	bpsf->m_char_byte_len = (bpsf->m_width+7)/8*bpsf->m_height;
	return 0;
}
typedef struct {
	int m_width;
	int m_height;
	unsigned char* m_bitmap_p;
} hz_bitmap_t;

// read next character info
int bpsf_read(bpsf_t* bpsf,hz_bitmap_t* hz)
{
	static count = -1;
	int size;
	size = (bpsf->m_width+7)/8*bpsf->m_height;
	count++;
	if(fread(hz->m_bitmap_p,size,1,bpsf->m_file) != 1){
		printf("read %d character(s)\n",count);
		count = -1;
		return -1;
	}
	else
		return 0;
}

int bpsf_get_char(bpsf_t* bpsf,unsigned char c1,unsigned char c2,hz_bitmap_t* hz)
{
	long pos,index;
	// get current position
	pos = ftell(bpsf->m_file);
	
	index = (c1-0xa1)*94+c2-0xa1;
	printf("c1=%02x c2=%02x index=%d char_byte_len=%d\n",
	       c1,c2,index,bpsf->m_char_byte_len);
	if(fseek(bpsf->m_file,bpsf->m_data_offset+index*bpsf->m_char_byte_len,SEEK_SET) != 0)
		goto anormal_out;
	if(bpsf_read(bpsf,hz) != 0)
		goto anormal_out;

	// restore position
	fseek(bpsf->m_file,pos,SEEK_SET);
	return 0;

anormal_out:
	// restore position
	fseek(bpsf->m_file,pos,SEEK_SET);
	return -1;
}
int bpsf_get_ascii(bpsf_t* bpsf,char ch,hz_bitmap_t* hz)
{
	long pos,index;
	// get current position
	pos = ftell(bpsf->m_file);
	
	index = ch;
	printf("c=%c index=%d char_byte_len=%d\n",
	       ch,index,bpsf->m_char_byte_len);
	if(fseek(bpsf->m_file,bpsf->m_data_offset+index*bpsf->m_char_byte_len,SEEK_SET) != 0)
		goto anormal_out;
	if(bpsf_read(bpsf,hz) != 0)
		goto anormal_out;

	// restore position
	fseek(bpsf->m_file,pos,SEEK_SET);
	return 0;

anormal_out:
	// restore position
	fseek(bpsf->m_file,pos,SEEK_SET);
	return -1;
}
int bpsf_close(bpsf_t* bpsf)
{
	return fclose(bpsf->m_file);
}
unsigned char pixel_mask[]=
{
	0x80,
	0x40,
	0x20,
	0x10,
	0x08,
	0x04,
	0x02,
	0x01
};
int draw_character(fb_device_t* dev,int x, int y, hz_bitmap_t* hz)
{
	unsigned char* ch;
	int i,j;
	int row_span;
	int x0 = x;


	y -= hz->m_height;
	row_span = (hz->m_width+7)/8;

	for(i=0; i<hz->m_height;i++){
		ch = hz->m_bitmap_p + i*row_span;

		for(j=0;j<hz->m_width;j++){
			unsigned char c = *(ch+(j/8));
			if(c & pixel_mask[j%8])
				fb_put_pixel(dev,x,y,255);
			else
				fb_put_pixel(dev,x,y,0);
			x++;
		}
		x = x0;
		y++;
	}
	return 0;
}
int main(int argc, char* argv[])
{
	bpsf_t bpsf;
	hz_bitmap_t hz;
	fb_device_t dev;
	int i;
	char* wo="我";
	assert(fb_open(&dev,"/dev/fb0") == 0);

	assert(bpsf_open(&bpsf,argv[1]) == 0);
	hz.m_width = bpsf.m_width;
	hz.m_height = bpsf.m_height;
	hz.m_bitmap_p = malloc((hz.m_width+7)/8*hz.m_height);

	printf("width=%d\nheight=%d\nchar_count=%d\n",
			bpsf.m_width,bpsf.m_height,bpsf.m_char_count);

	printf("%s c1=%02x c2=%02x\n",wo,(unsigned char)wo[0],(unsigned char)wo[1]);
	bpsf_get_char(&bpsf,wo[0],wo[1],&hz);
	draw_character(&dev,200,200,&hz);
	bpsf_get_ascii(&bpsf,'8',&hz);
	draw_character(&dev,300,300,&hz);
	// read character and display it
	for(i=0; i<bpsf.m_char_count; i++){
		assert(bpsf_read(&bpsf,&hz) ==0);
		assert(draw_character(&dev,100,100,&hz) == 0);
		usleep(100000);
	}

	bpsf_close(&bpsf);
	free(hz.m_bitmap_p);
	fb_close(&dev);
	return 0;
}
