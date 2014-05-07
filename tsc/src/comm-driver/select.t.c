/**
 * select.t.c
 * 
 * test select system call on /dev/cpld
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-06-14 15:32:36 中国标准时间
 * 
 * $log$
 *
 */

#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char* argv[])
{
	int fd = open(argv[1],O_RDWR|O_NOCTTY);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}


	while(1)
	{
		int ret;
		fd_set rfd;
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 500*1000;
		FD_SET(fd,&rfd);
		ret = select(fd+1,&rfd,NULL,NULL,&tv);
		if(ret < 0)
		{
			perror("select");
			return -1;
		}
		else if(ret == 0)
		{
			printf("timeout.\n");
		}
		else
		{
			int i;
			char buf[1024];
			printf("data available.\n");
			ret = read(fd,buf,1024);
			for(i=0;i<ret;i++)
				printf("%x ",buf[i]);
			printf("\n");
		}
	}
}
