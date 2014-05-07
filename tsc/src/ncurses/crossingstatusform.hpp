/* @(#)crossingstatusform.hpp
 */

#ifndef _CROSSINGSTATUSFORM_H
#define _CROSSINGSTATUSFORM_H 1


#include <stddef.h>
#include "baseform.hpp"
#include "hzk.hpp"
#include "crossingbitmap.h"
#include "../frame-buffer/canvas.h"
#include "../frame-buffer/fb_canvas.h"

class CCrossingStatusForm : public CBaseForm {
public:
	CCrossingStatusForm();
	~CCrossingStatusForm();

	int virtualize(int c);
	int TextOut(int x, int y, char* text);
private:
	int initHzk();
	int initBackground();
	fb_bitmap_t m_fb;
	bitmap_t m_bitmap;
	crossing_bitmap_t m_crossing;
	lamp_group_setting_t m_lamp_group_setting;
	CHzk m_hzk;
	bitmap_t m_hzbmp;
};

#endif /* _CROSSINGSTATUSFORM_H */

