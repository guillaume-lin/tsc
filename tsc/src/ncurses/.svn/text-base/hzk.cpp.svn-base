/* @(#)hzk.cpp
 */


#include "hzk.hpp"
#include "../frame-buffer/bpsf.h"

#define STATIC_MEM_ITEM_COUNT 23
typedef struct {
	int m_size;
	int m_used;
	char m_data[48*48*sizeof(color_t)];
} static_mem_item_t;

static_mem_item_t static_mem[STATIC_MEM_ITEM_COUNT];
void static_init()
{
	for(int i=0; i<STATIC_MEM_ITEM_COUNT; i++){
		static_mem[i].m_size = 48*48*sizeof(color_t);
		static_mem[i].m_used = 0;
	}
}
void* static_malloc(size_t size)
{
	for(int i=0; i<STATIC_MEM_ITEM_COUNT; i++){
		if(static_mem[i].m_used == 0){
			static_mem[i].m_used = 1;
			return static_mem[i].m_data;
		}
	}
	return NULL;
}
void static_free(void* ptr)
{
	for(int i=0; i<STATIC_MEM_ITEM_COUNT; i++){
		if(static_mem[i].m_data == ptr){
			static_mem[i].m_used = 0;
		}
	}
}

static mem_allocator_t allocator;
CHzk::CHzk()
	:m_ascii_counter(0),m_chinese_counter(0)
{
	allocator.malloc = static_malloc;
	allocator.free = static_free;
	
	for(int i=0; i<MAX_CHINESE_CHARACTER; i++){
		if(mem_bitmap_init(&m_chinese[i],48,48,&allocator) != 0)
			throw 1;
		m_chinese_index[i] = 0;
	}

	for(int i=0; i<MAX_ASCII_CHARACTER; i++){
		if(mem_bitmap_init(&m_ascii[i],24,48,&allocator) != 0)
			throw 2;
		m_ascii_index[i] = 0;
	}
}

CHzk::~CHzk()
{
	for(int i=0; i<MAX_ASCII_CHARACTER; i++){
		mem_bitmap_fini(&m_ascii[i]);
	}
	for(int i=0; i<MAX_CHINESE_CHARACTER; i++){
		mem_bitmap_fini(&m_chinese[i]);
	}
}

int CHzk::loadChineseFont(char* fontfile)
{

	m_chinese_font = fontfile;
	bpsf_t bpsf;
	bitmap_t bitmap;
	bitmap.m_fg = make_color(0,0,0);
	bitmap.m_bg = make_color(255,255,255);
	bitmap.get_width = mem_bitmap_get_width;
	bitmap.get_height = mem_bitmap_get_height;
	bitmap.get_pixel = mem_bitmap_get_pixel;
	bitmap.put_pixel = mem_bitmap_put_pixel;

	bpsf_open(&bpsf,fontfile);
	for(int i=0; i<MAX_CHINESE_CHARACTER; i++){
		bitmap.m_private_data = &m_chinese[i];
		char c1, c2;
		if(get_char_code(i,c1,c2) == 0){
			if(bpsf_get_char(&bpsf,c1,c2,&bitmap) == 0){
#if 0
				char s[10];
				s[0] = c1;
				s[1] = c2;
				s[2] = '\0';
				printf("load chinese %s\n",s);
				s[2] = '.';
				s[3] = 'b';
				s[4] = 'm';
				s[5] = 'p';
				s[6] = '\0';
				bitmap_save_to_file(&bitmap,s);
#endif
			}
		}
	}
	bpsf_close(&bpsf);
	return 0;
}

int CHzk::loadAsciiFont(char* fontfile)
{
	m_ascii_font = fontfile;
	bpsf_t bpsf;
	bitmap_t bitmap;
	bitmap.m_fg = make_color(0,0,0);
	bitmap.m_bg = make_color(255,255,255);
	bitmap.get_width = mem_bitmap_get_width;
	bitmap.get_height = mem_bitmap_get_height;
	bitmap.get_pixel = mem_bitmap_get_pixel;
	bitmap.put_pixel = mem_bitmap_put_pixel;

	bpsf_open(&bpsf,fontfile);
	for(int i=0; i<MAX_ASCII_CHARACTER; i++){
		bitmap.m_private_data = &m_ascii[i];
		char c;
		if(get_char_code(i,c) == 0){
			if(bpsf_get_ascii(&bpsf,c,&bitmap) == 0){
#if 0
				char buf[10];
				printf("load ascii %c\n",c);
				sprintf(buf,"%c.bmp",c);
				bitmap_save_to_file(&bitmap,buf);
#endif
			}
		}
	}
	bpsf_close(&bpsf);
	return 0;
}

int CHzk::get_char_code(int i, char& c1, char& c2)
{
	if(i >= MAX_CHINESE_CHARACTER || i < 0)
		return -1;
	int index = m_chinese_index[i];
	c2 = index & 0xff;
	c1 = (index >> 8) & 0xff;
	return 0;
}
int CHzk::get_char_code(int i, char& c)
{
	if(i >= MAX_ASCII_CHARACTER || i < 0)
		return -1;
	c = m_ascii_index[i] & 0xff;
	return 0;
}
static int make_index(char c1, char c2)
{
	unsigned int i;
	i = (unsigned char)c1;
	i<<= 8;
	i |= (unsigned char)c2;
	return i;
}
int CHzk::addChar(char c1, char c2)
{
	if(m_chinese_counter >= MAX_CHINESE_CHARACTER)
		return -1;
	m_chinese_index[m_chinese_counter++] = make_index(c1,c2);
#if 0
	char s[3];
	s[0] = c1;
	s[1] = c2;
	s[2] = '\0';
	printf("add %d : %s\n",m_chinese_counter-1,s);
#endif 
	return 0;
}

int CHzk::addChar(char c)
{
	if(m_ascii_counter >= MAX_ASCII_CHARACTER)
		return -1;
	m_ascii_index[m_ascii_counter++] = make_index(0,c);
#if 0
	char s[2];
	s[0] = c;
	s[1] = '\0';
	printf("add %d : %s\n",m_ascii_counter-1,s);
#endif
	return 0;
}

mem_bitmap_t* CHzk::getBitmap(char c1, char c2)
{
	int index = make_index(c1,c2);
	for(int i=0; i<MAX_CHINESE_CHARACTER; i++)
		if(m_chinese_index[i] == index)
			return &m_chinese[i];
	return NULL;

}

mem_bitmap_t* CHzk::getBitmap(char c)
{
	int index = make_index(0,c);
	for(int i=0; i<MAX_ASCII_CHARACTER; i++)
		if(m_ascii_index[i] == index)
			return &m_ascii[i];
	return NULL;

}

