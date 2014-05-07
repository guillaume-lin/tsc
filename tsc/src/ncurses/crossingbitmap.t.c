/* @(#)crossingbitmap.t.c
 */


#include "crossingbitmap.h"
#include "fb_canvas.h"


int main(int argc, char* argv[])
{

	int count,i;
	bitmap_t bitmap;
	fb_bitmap_t fb;
	crossing_bitmap_t crossing;
	crossing_lamp_t lamp;
	int car_on;
	fb_bitmap_init(&fb,&bitmap);

	crossing_bitmap_init(&crossing,&bitmap,"/bin/crossing.txt");
	for(i=0; i<count; i++)
		crossing_bitmap_set_lamp_status(&crossing,i+1,2);
	while(1){
		sleep(1);
		for(i=0; i<2; i++)
			crossing_bitmap_set_detector_status(&crossing,i+1,car_on);
		car_on = !car_on;
		crossing_bitmap_update(&crossing);
	}

	crossing_bitmap_fini(&crossing);
	return 0;
}
