#ifndef INCLUDED_CANVAS_H
#define INCLUDED_CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int m_r;
	int m_g;
	int m_b;
} color_t;

typedef struct {	
	void *m_private_data;
	//win_dc_get_height get_height;
 //   win_dc_get_width  get_width;
	//win_dc_get_pixel  get_pixel;
 //   win_dc_put_pixel  put_pixel;
	void * get_height;
	void * get_width;
	void * get_pixel;
	void * put_pixel;
	color_t  m_fg;
}bitmap_t;

typedef struct {
	int m_width;
	int m_height;
	unsigned char* m_data_p;
} mem_bitmap_t;

//typedef struct {
//	CDC* m_dc;
//} win_dc_bitmap_t;

//int (*win_dc_get_height)(void*) ;
//int (*win_dc_get_width) (void*)  ;
//int (*win_dc_get_pixel) (void*,int,int,color_t*) ;
//int (*win_dc_put_pixel) (void*,int,int,color_t) ;


int mem_bitmap_init(mem_bitmap_t *bm, int nwidth, int nheight, void* pd);
int mem_bitmap_get_width(void* self);
int mem_bitmap_get_height(void* self);
int mem_bitmap_get_pixel(void* self, int x, int y, color_t	p);
int mem_bitmap_put_pixel(void* self, int x, int y, color_t* p);
int bitmap_load_from_file(bitmap_t *bm, const char* szfilename);
int bitmap_paint_bitmap(bitmap_t *bm,bitmap_t *sbm, int nwidth, int nheight);
int mem_bitmap_fini(mem_bitmap_t * bm);
int bitmap_fill_circle(bitmap_t *bm, int x, int y, int r);
int bitmap_draw_circle(bitmap_t *bm, int x, int y, int r);
int bitmap_fill_rect(bitmap_t *bm, int x, int y, int w, int h);
int bitmap_draw_rect(bitmap_t *bm, int x, int y, int w, int h);
color_t make_color(unsigned char r,unsigned char g,unsigned char b); 

#ifdef __cplusplus
};
#endif

#endif //#ifndef INCLUDED_CANVAS_H
