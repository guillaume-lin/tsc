/**
 * actuate_control.h
 *
 * 感应控制
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2005-07-14 15:25:25 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_ACTUATE_CONTROL_H
#define INCLUDED_ACTUATE_CONTROL_H

int actuate_control(int switch_mode,int timer);
extern unsigned char  f_return_actuate_control;							// 从手动返回感应控制标志
#endif
