/**
 * util.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-13 19:17:34 中国标准时间
 * 
 * $log$
 *
 */
#include <assert.h>
#include "io.h"
#include "kernel.h"
#include "datastruct.h"
#include "detector_set.h"

/**
 *  获取指定端子的状态
 */
int decode_term_status(lamp_status_t* phase, int term)
{
	byte b,off;
	if(term < 1 || term > MAX_TERMINAL_COUNT)
		return -1;
	b = (term - 1) / 4;
	off = (term - 1) % 4;
	return (phase->m_output[b] >> ((3 - off)*2)) & 0x03;
}

//w	/**
//w	 *  设置指定端子的状态
//w	 *  0 灭
//w	 *  1 亮
//w	 *  2 闪
//w	 */
//w	int encode_term_status(phase_t* phase, int term, byte status)
//w	{
//w		byte b,off;
//w		if(term < 1 || term > MAX_TERMINAL_COUNT)
//w			return -1;
//w		b = (term - 1) / 4;
//w		off = (term - 1) % 4;
//w	
//w		// 先清零
//w		phase->m_output[b] &= ~( 0xc0 >> 2*off );
//w		// 设置
//w		phase->m_output[b] |= ( (status & 0x03) << 2*(3 - off) ); 
//w		return 0;
//w		}

/**
 * 对应指定灯组的检测器
 */
int get_lamp_group_detector(int group, detector_set_t* set)
{
	int i;
	for(i=0; i<MAX_DETECTOR_COUNT; i++){
		if(g_config.m_detector_setting.m_lamp_group[i] == group)
			add_set(i+1,set);
	}
	return 0;
}
/**
 * 判断指定的端子是否连接到绿灯
 * 如果是的话返回绿灯所在的灯组编号
 * 否则返回0
 */
int is_term_connect_to_green(int terminal)
{
	int i;
	for(i=0; i<MAX_LAMP_GROUP_COUNT; i++){
		if(g_config.m_lamp_group_setting.m_lamp_group[i][GREEN] == terminal)
			return (i+1);
	}
	return 0;
}

int is_term_connect_to_yellow(int terminal)
{
	int i;
	for(i=0; i<MAX_LAMP_GROUP_COUNT; i++){
		if(g_config.m_lamp_group_setting.m_lamp_group[i][YELLOW] == terminal)
			return (i+1);
	}
	return 0;
}

//w	/**
//w	 *  判断指定的检测器是否启用
//w	 */
//w	int is_detector_enable(int detector)
//w	{
//w		assert(detector >= 1 && detector <= MAX_DETECTOR_COUNT);
//w		return g_config.m_detector_setting.m_attribute[detector-1] & 0x40;
//w	}
//w	/**
//w	 * 判断指定的检测器是否是上游检测器（即离路口远的检测器）
//w	 */
//w	int is_detector_in_upstream(int detector)
//w	{
//w		assert(detector >= 1 && detector <= MAX_DETECTOR_COUNT);
//w		return g_config.m_detector_setting.m_attribute[detector-1] & 0x80;
//w	}
//w	/**
//w	 * 获取检测器的灵敏度
//w	 */
//w	int get_detector_sensitivity(int detector)
//w	{
//w		assert(detector >= 1 && detector <= MAX_DETECTOR_COUNT);
//w		return g_config.m_detector_setting.m_attribute[detector-1] & 0x03;
//w	}
/**
 * 获取相位相关检测器
 *
 * 所亮绿灯所在灯组所对应的检测器
 *
 * 输入的步伐必须是相位步不能是过渡步,否则结果为未定义
 */
int get_phase_detector(lamp_status_t* phase,detector_set_t* set)
{
	int i;
	byte group;
	for(i=0; i<MAX_TERMINAL_COUNT; i++){
		if(decode_term_status(phase,i+1) != ON)
			continue;
		// 端子现在为亮
		group = is_term_connect_to_green(i+1);
		if(group == 0)
			continue;
		//端子连接到绿灯

		//获取与灯组关联的检测器
		get_lamp_group_detector(group,set);
	
	}
	return 0;
}

/**
 * bytearray 指向字节数组的指针
 * num 位值的编号
 * base 组成位值的位的个数
 */
int get_bit_value(unsigned char* bytearray,int num, int base)
{
	int b;
	unsigned char off;
	unsigned char mask;

	b = (num - 1) / (8 / base);
	off = (num - 1) % ( 8 / base);
	mask = 0xff << (8 - base);

	return (bytearray[b] & (mask >> off*base)) >> ( 8 - base*(off+1) );
}
//w	/**
//w	 * bytearray 指向字节数组的指针
//w	 * num 位值的编号
//w	 * base 组成位值的位的个数
//w	 * value 要设置的值
//w	 */
//w	int set_bit_value(unsigned char* bytearray,int num, int base, int value)
//w	{
//w		int b;
//w		unsigned char off;
//w		unsigned char mask;
//w	
//w		b = (num - 1) / (8 / base);
//w		off = (num - 1) % ( 8 / base);
//w		mask = 0xff << (8 - base);
//w	
//w		bytearray[b] &= ~ (mask >> off*base);	// 清空
//w		bytearray[b] |= (value << (8 - base)) >> off*base;			// 设置
//w		return 0;
//w	}

