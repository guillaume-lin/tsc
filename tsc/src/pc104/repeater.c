/**
 * repeater.c
 * 
 * 在两个设备文件之间传递数据
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-08-10 17:22:11 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int transfer(int fd1, int fd2)
{
	int ret,count;
	unsigned char buf[1024];

	count = 0;
	do{
		ret = read(fd1,buf,1024);
		if(ret > 0){
			ret = write(fd2,buf,ret);
			if(ret > 0)
				count += ret;
		}
	}while(ret > 0);

	return count;
}
int main(int argc, char* argv[])
{
	int fd1,fd2,max_fd,ret;

	if(argc < 3){
		return -1;
	}

	fd1 = open(argv[1],O_RDWR|O_NONBLOCK);
	fd2 = open(argv[2],O_RDWR|O_NONBLOCK);

	if(fd1 < 0 || fd2 < 0){
		return -1;
	}

	max_fd = fd1 > fd2 ? fd1+1 : fd2+1;
	
	while(1){
		fd_set rfds;
		struct timeval tv;

		FD_ZERO(&rfds);
		FD_SET(fd1,&rfds);
		FD_SET(fd2,&rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 100000;

		ret = select(max_fd,&rfds,NULL,NULL,&tv);
		if(ret > 0){
			if(FD_ISSET(fd1,&rfds)){
				transfer(fd1,fd2);
			}
			if(FD_ISSET(fd2,&rfds)){
				transfer(fd2,fd1);
			}
		}
	}

	return 0;
}
