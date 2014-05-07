/**
 *  lockfb.c
 *  lock the framebuffer console , to prevent others to write
 *  to the console, and make the console chaos
 *  
 *  Author: ljx (jingxian@gmail.com)
 *  Date: 2014-4-30
 *
 */
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/kd.h>

int set_tty_mode(char* tty, int mode)
{
	// switch to graphics mode to lock the framebuffer
	int fd = open(tty,O_RDWR);
	if(fd < 0){
		perror("open");
		return -1;
	}
	int ret = 0;
	if(mode == 1){
		ret = ioctl(fd,KDSETMODE, KD_GRAPHICS);
	}else{
		ret = ioctl(fd,KDSETMODE, KD_TEXT);
	}
	close(fd);
	if(ret != 0){
		perror("ioctl");
		return -1;
	}
	return 0;
	
}
int main(int argc, char* argv[])
{
	if(argc < 3){
		printf("Usage: %s <tty> <1|0>\n",argv[0]);
		return -1;
	}
	
	int mode = atoi(argv[2]);
	if(mode != 0 && mode != 1){
		printf("%s: unknown mode %d\n",argv[0],mode);
		return -1;
	}
	return set_tty_mode(argv[1],mode);
}
