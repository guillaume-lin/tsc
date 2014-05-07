/**
 * detector_set.h
 *
 * 检测器集合的操作
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2005-07-18 16:34:03 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_DETECTOR_SET_H
#define INCLUDED_DETECTOR_SET_H
#define MAX_SET_ELEMENT_COUNT 48
// 检测器集
typedef struct {
	unsigned char m_bits[6];	// 48位，目前支持48个检测器
} detector_set_t;

// 清空检测器集合
int empty_set(detector_set_t* set);
// 新增一个检测器到集合
int add_set(int detector, detector_set_t* set);
// 从集合中删除一个检测器
int del_set(int detector, detector_set_t* set);
// 判断检测器是否在集合中
int is_in_set(int detector, detector_set_t* set);

#endif
