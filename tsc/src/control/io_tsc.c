#include <termio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "io.h"

/**
 * 初始化串口
 */
static int init(int fd)
{
	struct termios ios;
	speed_t baud = B9600;
	// 设置串口属性
	ios.c_iflag = 0;
	ios.c_oflag = 0;
	ios.c_cflag = 0;
	ios.c_lflag = 0;

	ios.c_iflag |= (IGNBRK | IGNPAR) ;
	// ios.c_oflag |= OPOST;
	ios.c_cflag |= (CS8 | CREAD | CLOCAL) ;
	// ios.lflag
	
	if(cfsetispeed(&ios,baud))
		return -1;
	if(cfsetospeed(&ios,baud))
		return -1;

	return tcsetattr(fd,TCSANOW,&ios);
}
/**
 *  打开指定的通信端口
 */
int open_comm(int port)
{
	char name[20];
	sprintf(name,"/dev/ttyS%01d",port-1);

	int fd = open(name,O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(init(fd) != 0)
		return -1;
	else
		return fd;
}

int read_comm(int fd, byte* buf, int len)
{
	return read(fd,buf,len);	
}

int write_comm(int fd, byte* buf, int len)
{
	return write(fd,buf,len);
}

int close_comm(int fd)
{
	return close(fd);
}

int wait_for_100ms()
{
	usleep(100000);
	return 0;
}
