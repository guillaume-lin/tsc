/**
 * detector_set.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-18 16:37:25 中国标准时间
 * 
 * $log$
 *
 */
#include <assert.h>
#include <string.h>
#include "detector_set.h"

// 清空检测器集合
int empty_set(detector_set_t* set)
{
	memset(set->m_bits,0,sizeof(detector_set_t));
	return 0;
}
// 新增一个检测器到集合
int add_set(int detector,detector_set_t* set)
{
	unsigned char b,off;
	assert(detector >= 1 && detector <= MAX_SET_ELEMENT_COUNT);
	b = (detector-1) / 8;
	off = (detector-1) % 8;
	set->m_bits[b] |= 0x80 >> off;
	return 0;
}
// 从集合中删除一个检测器
int del_set(int detector,detector_set_t* set)
{
	unsigned char b,off;
	assert(detector >= 1 && detector <= MAX_SET_ELEMENT_COUNT);
	b = (detector-1) / 8;
	off = (detector-1) % 8;
	set->m_bits[b] &= ~ (0x80 >> off);
	return 0;
}
// 判断检测器是否在集合中
int is_in_set(int detector,detector_set_t* set)
{	
	unsigned char b,off;
	assert(detector >= 1 && detector <= MAX_SET_ELEMENT_COUNT);
	b = (detector-1) / 8;
	off = (detector-1) % 8;
	return set->m_bits[b] & (0x80 >> off);
}

