/**
 * detector_set.h
 *
 * ��������ϵĲ���
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2005-07-18 16:34:03 �й���׼ʱ��
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_DETECTOR_SET_H
#define INCLUDED_DETECTOR_SET_H
#define MAX_SET_ELEMENT_COUNT 48
// �������
typedef struct {
	unsigned char m_bits[6];	// 48λ��Ŀǰ֧��48�������
} detector_set_t;

// ��ռ��������
int empty_set(detector_set_t* set);
// ����һ�������������
int add_set(int detector, detector_set_t* set);
// �Ӽ�����ɾ��һ�������
int del_set(int detector, detector_set_t* set);
// �жϼ�����Ƿ��ڼ�����
int is_in_set(int detector, detector_set_t* set);

#endif
