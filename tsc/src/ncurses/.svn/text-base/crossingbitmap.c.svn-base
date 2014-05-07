/* @(#)crossingbitmap.c
 */
#include "simpleini.h"
#include "crossingbitmap.h"


int get_color(char* name,color_t* color)
{
	if(strncmp(name,"red",3)  == 0){
		*color = make_color(255,0,0);
		return 0;
	}
	if(strncmp(name,"green",5) == 0){
		*color = make_color(0,255,0);
		return 0;
	}
	if(strncmp(name,"yellow",6) == 0){
		*color = make_color(255,255,0);
		return 0;
	}
	if(strncmp(name,"black",5) == 0){
		*color = make_color(0,0,0);
		return 0;
	}
	return -1;
}
int crossing_bitmap_init(crossing_bitmap_t* self, bitmap_t* bm,char* cfg)
{
	int lamp_count,i;
	int detector_count;
	simple_ini_t ini;

	
	simple_ini_open(&ini,cfg);

	simple_ini_set_section(&ini,"general");
	lamp_count = simple_ini_get_int(&ini,"lamp_count");
	detector_count = simple_ini_get_int(&ini,"detector_count");
	for(i=1; i<= crossing_bitmap_get_lamp_count(self); i++){
		char* s;
		char section[20];
		
		crossing_lamp_t* lamp = &self->m_lamp[i-1];
		lamp->m_valid = 0; // initial unusable

		if(i > lamp_count)
			continue;
		sprintf(section,"lamp%d",i);
		simple_ini_set_section(&ini,section);
		lamp->m_x = simple_ini_get_int(&ini,"x");
		lamp->m_y = simple_ini_get_int(&ini,"y");
		lamp->m_r = simple_ini_get_int(&ini,"r");
		s = simple_ini_get_string(&ini,"color");
		if(s == NULL || get_color(s,&lamp->m_color) != 0){
			// not a valid lamp
			continue;
		}
		lamp->m_valid = 1;
		lamp->m_status = 0; // initial off
	}
	for(i=1; i<= crossing_bitmap_get_detector_count(self); i++){
		char* s;
		char section[20];
		
		crossing_detector_t* detector = &self->m_detector[i-1];
		detector->m_valid = 0; // initial unusable

		if(i > detector_count)
			continue;
		sprintf(section,"detector%d",i);
		simple_ini_set_section(&ini,section);
		detector->m_x = simple_ini_get_int(&ini,"x");
		detector->m_y = simple_ini_get_int(&ini,"y");
		detector->m_w = simple_ini_get_int(&ini,"w");
		detector->m_h = simple_ini_get_int(&ini,"h");
		s = simple_ini_get_string(&ini,"color");
		if(s == NULL || get_color(s,&detector->m_color) != 0){
			// not a valid detector
			continue;
		}
		detector->m_valid = 1;
		detector->m_status = 0; // initial no car
	}
	self->m_bitmap = bm;
	bitmap_load_from_file(bm,"crossing.bmp");
	simple_ini_close(&ini);
	return 0;
}
int crossing_bitmap_fini(crossing_bitmap_t* self)
{
	return 0;
}
int crossing_bitmap_get_lamp_count(crossing_bitmap_t* self)
{
	return 96;
}

int crossing_bitmap_set_lamp(crossing_bitmap_t* self,int no,
				     crossing_lamp_t lamp)
{
	if(no < 1 || no > 96)
		return -1;
	self->m_lamp[no-1] = lamp;
	return 0;
}
int crossing_bitmap_get_lamp(crossing_bitmap_t* self,int no,
				     crossing_lamp_t* lamp)
{
	if(no < 1 || no > 96)
		return -1;
	*lamp = self->m_lamp[no-1];
	return 0;
}

int crossing_bitmap_set_lamp_status(crossing_bitmap_t* self,
				     int no, int status)
{
	if(no < 1 || no > 96)
		return -1;
	self->m_lamp[no-1].m_status = status;
	return 0;
}

int crossing_bitmap_get_detector_count(crossing_bitmap_t* self)
{
	return 32;
}
int corssing_bitmap_set_detector(crossing_bitmap_t* self, int no,
				 crossing_detector_t detector)
{
	if(no < 1 || no > 32)
		return -1;
	self->m_detector[no-1] = detector;
	return 0;
}
int crossing_bitmap_get_detector(crossing_bitmap_t* self, int no,
				 crossing_detector_t* detector)
{
	if(no < 1 || no > 32)
		return -1;
	*detector = self->m_detector[no-1];
	return 0;
}
int crossing_bitmap_set_detector_status(crossing_bitmap_t* self,
					int no, int status)
{
	if(no < 1 || no > 32)
		return -1;
	self->m_detector[no-1].m_status = status;
	return 0;
}

int crossing_bitmap_update(crossing_bitmap_t* self)
{
	int i;
	static color_t white = {255,255,255};

	self->m_flash = !self->m_flash;
	for(i=1; i<=crossing_bitmap_get_lamp_count(self); i++){
		// draw lamp
		crossing_lamp_t* lamp = &self->m_lamp[i-1];
		if(!lamp->m_valid)
			continue;
		if(lamp->m_status == 0 ||
		   (lamp->m_status == 2 && !self->m_flash)){
			self->m_bitmap->m_fg = white;
		}else{
			self->m_bitmap->m_fg = lamp->m_color;
		}

		bitmap_fill_circle(self->m_bitmap,
				   lamp->m_x,lamp->m_y,lamp->m_r);
		self->m_bitmap->m_fg = make_color(0,0,0);
		bitmap_draw_circle(self->m_bitmap,
				   lamp->m_x,lamp->m_y,lamp->m_r);
	}

	for(i=1; i<=crossing_bitmap_get_detector_count(self); i++){
		//draw detector
		crossing_detector_t* detector = &self->m_detector[i-1];
		if(!detector->m_valid)
			continue;
		if(detector->m_status == 0) // no car
			self->m_bitmap->m_fg = white;
		else
			self->m_bitmap->m_fg = detector->m_color;

		bitmap_fill_rect(self->m_bitmap,
				 detector->m_x,detector->m_y,
				 detector->m_w,detector->m_h);
		self->m_bitmap->m_fg = make_color(0,0,0);
		bitmap_draw_rect(self->m_bitmap,
				 detector->m_x,detector->m_y,
				 detector->m_w,detector->m_h);
	}
	return 0;
}

