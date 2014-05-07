/**
 * cpld_fifo.t.c
 * 
 * test program for cpld_fifo.h cpld_fifo.c
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-08-09 09:09:40 中国标准时间
 * 
 * $log$
 *
 */
#include <stdio.h>
#include <assert.h>
#include "cpld_fifo.h"

int main(int argc, char* argv[])
{

	cpld_fifo_t fifo;

	cpld_fifo_init(&fifo);
	unsigned  char count = 0;
	while(!cpld_fifo_is_full(&fifo)){
		cpld_fifo_putchar(&fifo,count);
		count++;
	}
	printf("fifo's capacity is: %d\n",count);

	count = 0;
	while(!cpld_fifo_is_empty(&fifo)){
		unsigned char ch = cpld_fifo_getchar(&fifo);
		count++;
	}
	printf("get %d bytes\n",count);

	return 0;
}
