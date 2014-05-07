#include <stdio.h>
#include "fb_util.h"


int main(int argc, char* argv[])
{
	if(argc < 3){
		printf("Usage: %s <fb device> <screen count>\n",argv[0]);
		return -1;
	}
	int count = atoi(argv[2]);
	if(count < 1 || count > 3){
		printf("only support 1 - 3 screen\n");
		return -1;
	}

	fb_device_t dev;
	fb_open(&dev,argv[1]);
	fb_set_screen_count(&dev,count);
	fb_close(&dev);
	return 0;
}

