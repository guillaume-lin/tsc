/**
 * io_mfc.h
 * 
 * 实现底层通信
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-02 11:10:24 中国标准时间
 * 
 * $log$
 *
 */
#ifndef INCLUDED_IO_MFC_H
#define INCLUDED_IO_MFC_H
#include "datastruct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  打开指定的通信端口
 */
int init_sock(char* ip,int port);
int open_comm(int port);
int set_comm_baud(int fd,int baud);
int read_comm(int fd, byte* buf, int len);
int write_comm(int fd, byte* buf, int len);
int close_comm(int fd);
int wait_for_100ms();

#ifdef __cplusplus
}
#endif

#endif
