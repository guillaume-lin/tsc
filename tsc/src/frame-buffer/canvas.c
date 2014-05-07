#include "canvas.h"

int mem_bitmap_init(mem_bitmap_t *bm, int nwidth, int nheight, void* pd)
{
	return 0;
}

int mem_bitmap_get_width(void* self)
{
	return 0;
}

int mem_bitmap_get_height(void* self)
{
	return 0;
}

int mem_bitmap_get_pixel(void* self, int x, int y, color_t	p)
{
	return 0;
}

int mem_bitmap_put_pixel(void* self, int x, int y, color_t* p)
{
	return 0;
}

int bitmap_load_from_file(bitmap_t *bm, const char* szfilename)
{
	return 0;
}

int bitmap_paint_bitmap(bitmap_t *bm,bitmap_t *sbm, int nwidth, int nheight)
{
	return 0;
}

int mem_bitmap_fini(mem_bitmap_t * bm)
{
	return 0;
}

color_t  make_color(unsigned char r,unsigned char g,unsigned char b)
{
	color_t  tmp;
	tmp.m_b = b;
	tmp.m_g = g;
	tmp.m_r = r;

	return tmp;
}
int bitmap_fill_circle(bitmap_t *bm, int x, int y, int r)
{
	return 0;
}
int bitmap_draw_circle(bitmap_t *bm, int x, int y, int r)
{
	return 0;
}
int bitmap_fill_rect(bitmap_t *bm, int x, int y, int w, int h)
{
	return 0;
}
int bitmap_draw_rect(bitmap_t *bm, int x, int y, int w, int h)
{
	return 0;
}
