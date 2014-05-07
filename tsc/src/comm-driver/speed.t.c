/**
 * speed.t.c
 * 
 * test the transfer speed
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-06-09 15:15:04 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char* argv[])
{

	long s,us;
	int fd;
	unsigned char buf[10240];
	struct timeval tv1,tv2;
	int count = 0;
	fd = open("/dev/cpld",O_NOCTTY|O_SYNC);
	gettimeofday(&tv1,NULL);
	while(count != 10240)
	{
		int ret =read(fd,buf+count,10240-count);
		if(ret >= 0)
			count+= ret;
	}
	gettimeofday(&tv2,NULL);

        s = tv2.tv_sec - tv1.tv_sec;
	us = tv2.tv_usec - tv1.tv_usec;
	printf("10k for %02d:%06d\n",s,us);

	return 0;
}

