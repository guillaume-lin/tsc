/**
 * util.h
 *
 * utiltiy functions
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2005-07-13 19:17:07 �й���׼ʱ��
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_UTIL_H
#define INCLUDED_UTIL_H
#include "datastruct.h"
#include "detector_set.h"

#ifdef __cplusplus
extern "C" {
#endif

//0921 int init_special_schedule_data();

int decode_term_status(lamp_status_t* phase, int term);
int encode_term_status(lamp_status_t* phase, int term, byte status);
int get_lamp_group_detector(int group, detector_set_t* set);
int is_term_connect_to_green(int terminal);

int is_detector_enable(int detector);
int is_detector_in_upstream(int detector);
int get_detector_sensitivity(int detector);

int get_phase_detector(lamp_status_t* phase,detector_set_t* set);

/**
 * ��ȡָ��λ��ֵ
 *
 * @param bytearry ��������λ������
 * @param num	   Ҫ��ȡ��λ������λ�еķ�����Ҫ����1��ʼ��
 * @param base     λ�����С,Ŀǰֻ֧��1,2,4,8�ķ��飬��������Ľ��δ����
 */
int get_bit_value(unsigned char* bytearray,int num, int base);

/**
 * ����ָ��λ��ֵ
 * @param bytearry ��������λ������
 * @param num	   Ҫ��ȡ��λ������λ�еķ�����Ҫ����1��ʼ��
 * @param base     λ�����С,Ŀǰֻ֧��1,2,4,8�ķ��飬��������Ľ��δ����
 */
int set_bit_value(unsigned char* bytearray,int num, int base, int value);

#ifdef __cplusplus
}
#endif

#endif
