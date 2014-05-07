/**
 * cpld_fifo.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-08-08 18:24:30 中国标准时间
 * 
 * $log$
 *
 */
#define MODULE
#include <linux/version.h>
#include <linux/module.h>
#include "cpld_fifo.h"

/**
 * 参数：
 * 	self	环形队列
 * 返回：
 * 	无
 * 说明：
 *	初始化环形队列
 */
void cpld_fifo_init(cpld_fifo_t* self)
{
	self->m_read_p = self->m_write_p = 0;
}

/**
 * 参数:
 * 	self 环形队列
 * 	buf  存放读到的字节的缓冲区
 * 	len 缓冲区长度
 * 返回：
 * 	从环形队列中读到的字节数
 * 说明：
 */
int cpld_fifo_read(cpld_fifo_t* self,unsigned char* buf, int len)
{
	int count = 0;
	while(count < len && !cpld_fifo_is_empty(self)){
		buf[count++] = cpld_fifo_getchar(self);
	}
	return count;
}

/**
 * 参数:
 * 	self 环形队列
 * 	buf  要写入的字节的缓冲区
 * 	len  缓冲区长度
 * 返回：
 * 	写入到环形队列中的字节数
 * 说明：
 * 	
 */

int cpld_fifo_write(cpld_fifo_t* self, unsigned char* buf, int len)
{
	int count = 0;
	while(count < len && !cpld_fifo_is_full(self)){
		cpld_fifo_putchar(self,buf[count++]);
	}
	return count;
}

/**
 * 参数：
 * 	self 环形队列
 * 	ch   要放入环形队列的字符
 * 返回：
 * 	无	
 * 说明：
 * 	把字符放入环形队列
 */
void cpld_fifo_putchar(cpld_fifo_t* self,unsigned char ch)
{
	self->m_buffer[self->m_write_p++] = ch;
	if(self->m_write_p == MAX_BUFFER_LEN)
		self->m_write_p = 0;
#ifndef NDEBUG
	printk(KERN_ERR "putchar: m_write_p = %d\n",self->m_write_p);
	printk(KERN_ERR "putchar: m_read_p = %d\n",self->m_read_p);
#endif
}

/**
 * 参数：
 * 	self 环形队列
 * 返回:
 * 	队列头的字符
 * 说明:
 * 	读取队列头的字符
 *
 */
unsigned char cpld_fifo_getchar(cpld_fifo_t* self)
{
	unsigned char ch = self->m_buffer[self->m_read_p++];
	if(self->m_read_p == MAX_BUFFER_LEN)
		self->m_read_p = 0;
#ifndef NDEBUG
	printk(KERN_ERR "getchar: m_write_p = %d\n",self->m_write_p);
	printk(KERN_ERR "getchar: m_read_p = %d\n",self->m_read_p);
#endif
	return ch;

}

/**
 * 参数:
 * 	self 环形队列
 * 返回：
 * 	如果队列将要满的话，返回非0，否则返回0
 * 说明：
 * 	判断队列中是否只剩下一个空位
 */
int cpld_fifo_is_will_full(cpld_fifo_t* self)
{
	if((self->m_write_p + 2)%MAX_BUFFER_LEN == self->m_read_p)
		return 1;
	else
		return 0;

}

/**
 * 参数：
 * 	self 环形队列
 * 返回：
 * 	如果队列满了，返回非0，否则返回0
 * 说明：
 * 	判断队列是否已经满了
 */
int cpld_fifo_is_full(cpld_fifo_t* self)
{
	if((self->m_write_p + 1)%MAX_BUFFER_LEN  == self->m_read_p)
		return 1;
	else
		return 0;

}

/**
 * 参数：
 * 	self 环形队列
 * 返回：
 * 	如果队列为空，返回非0，否则返回0
 * 说明：
 *
 */
int cpld_fifo_is_empty(cpld_fifo_t* self)
{
	if(self->m_read_p == self->m_write_p)
		return 1;
	else
		return 0;

}

