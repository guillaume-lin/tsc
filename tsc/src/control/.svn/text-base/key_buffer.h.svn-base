/**
 * key_buffer.h
 *
 * 保存单片机按键的缓冲区
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2005-07-28 09:35:06 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_KEY_BUFFER_H
#define INCLUDED_KEY_BUFFER_H
#ifdef __cplusplus
extern "C" {
#endif


#define MAX_KEY_BUFFER_SIZE  10
typedef struct {
	// 读指针
	int m_read_p;
	// 写指针
	int m_write_p;
	int m_buffer[MAX_KEY_BUFFER_SIZE];
} key_buffer_t;

int init_key_buffer(key_buffer_t* self);
// 读取按键
int get_buffer_key(key_buffer_t* self);
// 放入按键
int put_buffer_key(key_buffer_t* self,int key);
// 判断缓冲区是否满
int is_buffer_full(key_buffer_t* self);
// 判断缓冲区是否空
int is_buffer_empty(key_buffer_t* self);
// 清空缓冲区
int empty_key_buffer(key_buffer_t* self);

#ifdef __cplusplus
}
#endif

#endif
