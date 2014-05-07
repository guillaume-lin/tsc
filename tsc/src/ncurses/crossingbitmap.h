/* @(#)crossingbitmap.h
 *
 * 
 *
 * Company: Xiamen Uni-Creative Information Technology Corp.
 * Author:  Lin Jingxian(аж╬╟ом)
 * Email:   cckk_lin@yahoo.ie
 * Date:    2005-11-15 14:16:00
 *
 * $log$
 *
 */

#ifndef INCLUDED_CROSSINGBITMAP_H
#define INCLUDED_CROSSINGBITMAP_H

#include "../frame-buffer/canvas.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {
		int m_x;
		int m_y;
		int m_r;
	        color_t m_color;
		int m_status; // 0 - off, 1 - on, 2 - flash 
		int m_health; // 0 - fault 1 - ok
		int m_valid;
	} crossing_lamp_t;

	typedef struct {
		int m_x;
		int m_y;
		int m_w;
		int m_h;
		color_t m_color;
		int m_status;  // 0 - no car 1 - has car
		int m_health;
		int m_valid;
	} crossing_detector_t;
	typedef struct {
		crossing_lamp_t m_lamp[96];
		crossing_detector_t m_detector[32];
		bitmap_t * m_bitmap;
		int m_flash;
	} crossing_bitmap_t;

	int crossing_bitmap_init(crossing_bitmap_t* self, bitmap_t* bm,char* cfg);
	int crossing_bitmap_fini(crossing_bitmap_t* self);
	int crossing_bitmap_get_lamp_count(crossing_bitmap_t* self);
	int crossing_bitmap_set_lamp(crossing_bitmap_t* self,int no, 
				     crossing_lamp_t lamp);
	int crossing_bitmap_get_lamp(crossing_bitmap_t* self,int no,
				     crossing_lamp_t* lamp);
	int crossing_bitmap_set_lamp_status(crossing_bitmap_t* self,
				     int no, int status);

	int crossing_bitmap_get_detector_count(crossing_bitmap_t* self);
	int corssing_bitmap_set_detector(crossing_bitmap_t* self, int no,
				     crossing_detector_t detector);
	int crossing_bitmap_get_detector(crossing_bitmap_t* self, int no,
				     crossing_detector_t* detector);
	int crossing_bitmap_set_detector_status(crossing_bitmap_t* self,
						int no, int status);

	int crossing_bitmap_update(crossing_bitmap_t* self);
	
#ifdef __cplusplus
};
#endif

#endif /* INCLUDED_CROSSINGBITMAP_H */

