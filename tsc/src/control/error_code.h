/**
 * error_code.h
 *
 * �������
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2005-08-18 10:57:10 �й���׼ʱ��
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_ERROR_CODE_H
#define INCLUDED_ERROR_CODE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  hardware error
 */
#define LAMP_ERROR 	0x01
#define DETECTOR_ERROR	0x02
#define FRAME_ERROR	0x04     // ֡���մ���

/**
 * software error
 */
#define INVALID_TIMMING		0x01
#define INVALID_PHASE		0x02
#define INVALID_SCHEDULE	0x04
#define INVALID_CONTROL_PARAM 	0x08
#define INVALID_DETECTOR_SETTING	0x10
#define INVALID_LAMP_GROUP_SETTING 	0x20

extern void set_error(unsigned char err, unsigned char* b);
extern void clear_error(unsigned char err, unsigned char* b);

#ifdef __cplusplus
};
#endif

#endif
