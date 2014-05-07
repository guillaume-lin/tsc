/**
 * command_reactor.h
 *
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2005-07-08 09:46:45 �й���׼ʱ��
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_COMMAND_REACTOR_H
#define INCLUDED_COMMAND_REACTOR_H
#include "key_buffer.h"
#include "codec.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
	int m_fd;
	frame_t m_frame;
	frame_data_t m_frame_data;
	key_buffer_t m_key_buffer;	// ��Ž��յ��İ���
}command_reactor_t; 

extern unsigned char key_buf[4],key_count;

int init_command_reactor(command_reactor_t* reactor);
int reactor_handle_command(command_reactor_t* reactor);

#ifdef __cplusplus
}
#endif
#endif
