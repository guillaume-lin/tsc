/**
 * cpld_fifo.h
 *
 * fifo queue to process input output
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-08-08 17:53:58 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_CPLD_FIFO_H
#define INCLUDED_CPLD_FIFO_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BUFFER_LEN 1025


typedef struct {
	// 读指针
	int m_read_p;
	// 写指针
	int m_write_p;
	// 缓冲区
	unsigned char m_buffer[MAX_BUFFER_LEN];
}cpld_fifo_t;

extern void cpld_fifo_init(cpld_fifo_t* self);
extern int cpld_fifo_read(cpld_fifo_t* self,unsigned char* buf, int len);
extern int cpld_fifo_write(cpld_fifo_t* self, unsigned char* buf, int len);

extern void cpld_fifo_putchar(cpld_fifo_t* self,unsigned char ch);
extern unsigned char cpld_fifo_getchar(cpld_fifo_t* self);

extern int cpld_fifo_is_will_full(cpld_fifo_t* self);
extern int cpld_fifo_is_full(cpld_fifo_t* self);
extern int cpld_fifo_is_empty(cpld_fifo_t* self);
extern int cpld_fifo_capacity(cpld_fifo_t* self);

#ifdef __cplusplus
};
#endif

#endif
