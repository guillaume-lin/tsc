/**
 * cpld_fifo.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-08-08 18:24:30 �й���׼ʱ��
 * 
 * $log$
 *
 */
#define MODULE
#include <linux/version.h>
#include <linux/module.h>
#include "cpld_fifo.h"

/**
 * ������
 * 	self	���ζ���
 * ���أ�
 * 	��
 * ˵����
 *	��ʼ�����ζ���
 */
void cpld_fifo_init(cpld_fifo_t* self)
{
	self->m_read_p = self->m_write_p = 0;
}

/**
 * ����:
 * 	self ���ζ���
 * 	buf  ��Ŷ������ֽڵĻ�����
 * 	len ����������
 * ���أ�
 * 	�ӻ��ζ����ж������ֽ���
 * ˵����
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
 * ����:
 * 	self ���ζ���
 * 	buf  Ҫд����ֽڵĻ�����
 * 	len  ����������
 * ���أ�
 * 	д�뵽���ζ����е��ֽ���
 * ˵����
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
 * ������
 * 	self ���ζ���
 * 	ch   Ҫ���뻷�ζ��е��ַ�
 * ���أ�
 * 	��	
 * ˵����
 * 	���ַ����뻷�ζ���
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
 * ������
 * 	self ���ζ���
 * ����:
 * 	����ͷ���ַ�
 * ˵��:
 * 	��ȡ����ͷ���ַ�
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
 * ����:
 * 	self ���ζ���
 * ���أ�
 * 	������н�Ҫ���Ļ������ط�0�����򷵻�0
 * ˵����
 * 	�ж϶������Ƿ�ֻʣ��һ����λ
 */
int cpld_fifo_is_will_full(cpld_fifo_t* self)
{
	if((self->m_write_p + 2)%MAX_BUFFER_LEN == self->m_read_p)
		return 1;
	else
		return 0;

}

/**
 * ������
 * 	self ���ζ���
 * ���أ�
 * 	����������ˣ����ط�0�����򷵻�0
 * ˵����
 * 	�ж϶����Ƿ��Ѿ�����
 */
int cpld_fifo_is_full(cpld_fifo_t* self)
{
	if((self->m_write_p + 1)%MAX_BUFFER_LEN  == self->m_read_p)
		return 1;
	else
		return 0;

}

/**
 * ������
 * 	self ���ζ���
 * ���أ�
 * 	�������Ϊ�գ����ط�0�����򷵻�0
 * ˵����
 *
 */
int cpld_fifo_is_empty(cpld_fifo_t* self)
{
	if(self->m_read_p == self->m_write_p)
		return 1;
	else
		return 0;

}

