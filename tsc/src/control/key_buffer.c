/**
 * key_buffer.c
 * 
 * 按键的环形缓冲区
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-28 09:42:40 中国标准时间
 * 
 * $log$
 *
 */
#include "key_buffer.h"

int init_key_buffer(key_buffer_t* self)
{
	self->m_read_p = 0;
	self->m_write_p = 0;
	return 0;
}
// 读取按键
int get_buffer_key(key_buffer_t* self)
{
	int key = self->m_buffer[self->m_read_p++];
	if(self->m_read_p == MAX_KEY_BUFFER_SIZE)
		self->m_read_p = 0;
	return key;
}
// 放入按键
int put_buffer_key(key_buffer_t* self,int key)
{
	self->m_buffer[self->m_write_p++] = key;
	if(self->m_write_p == MAX_KEY_BUFFER_SIZE)
		self->m_write_p = 0;
	return 0;
}
// 判断缓冲区是否满
int is_buffer_full(key_buffer_t* self)
{
	if(((self->m_write_p + 1)% MAX_KEY_BUFFER_SIZE) == self->m_read_p)
		return 1;
	else
		return 0;
}
// 判断缓冲区是否空
int is_buffer_empty(key_buffer_t* self)
{
	if(self->m_read_p == self->m_write_p)
		return 1;
	else
		return 0;
}
// 清空缓冲区
int empty_key_buffer(key_buffer_t* self)
{
	return init_key_buffer(self);
}

