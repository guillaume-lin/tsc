#include <unistd.h>
#include <curses.h>
#include "refresh_console.hpp"
#include "Log.hpp"

int refresh_console()
{
	static int fd = -1;
	if(fd < 0)
		fd = open("/dev/tty1",O_RDWR);
	if(fd < 0){
		Log::d("refresh_console", "open /dev/tty1");
		return -1;
	}
	int ret = write(fd,"\E[H\E[J",6); // clear tty1
	if(ret != 6){
		Log::d("refresh_console", "write /dev/tty1");
		return -1;
	}
	::clear();
	::refresh();
	return 0;
}
