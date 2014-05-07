/**
 * cpld.h
 *
 * header file for device cpld
 *
 * define ioctl command for device cpld
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-08-09 17:54:30 �й���׼ʱ��
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_CPLD_H
#define INCLUDED_CPLD_H
#ifdef __cplusplus
extern "C" {
#endif

#include <linux/ioctl.h>

#define CPLD_IOC_MAGIC	'k'

/* ����˿�ѡ������ */
#define CPLD_IOC_PORT_SELECT _IOW(CPLD_IOC_MAGIC,1,int)


#ifdef __cplusplus
}
#endif
#endif
