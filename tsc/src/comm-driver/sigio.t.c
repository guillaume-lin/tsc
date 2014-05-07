/**
 * sigio.t.c
 * 
 * test SIGIO ability for cpld file
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-06-13 14:21:12 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

typedef void(*SA_HANDLER)(int sig);

void input_handler(int sig)
{
	printf("signal %d got.\n",sig);
}

int install_sigio_handler(int fd,SA_HANDLER sh)
{
	struct sigaction sigact;
	int ret;
	sigact.sa_handler = sh;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;

	ret = fcntl(fd,F_SETOWN,getpid());
	if(ret != 0)
		return ret;
	ret = fcntl(fd,F_GETFL);
	ret = fcntl(fd,F_SETFL,ret|FASYNC);
	if(ret != 0)
		return ret;
	return sigaction(SIGIO,&sigact,NULL);
}


int main(int argc, char* argv[])
{
	int fd = open(argv[1],O_NOCTTY|O_RDWR);
	if(fd == -1)
		return -1;
	if(install_sigio_handler(fd,input_handler) != 0)
		return -1;
	while(1)
		sleep(1);
	return 0;
}
