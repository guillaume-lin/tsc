/**
 * io.h
 *
 * 低层硬件相关的输入输出
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-02 10:57:10 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_IO_H
#define INCLUDED_IO_H
#include "datastruct.h"
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

// 初始化
int initialize();
/**
 *  IO 相关函数
 */
// 设置端子状态
int set_term_status(lamp_status_t* phase);
// 获取灯的故障信息
int get_lamp_health(lamp_health_t* health);
// 获取检测器的检测信息
int get_detector_status(detector_status_t* status);
// 获取检测器的故障信息
int get_detector_health(detector_health_t* health);
// 获取检测器计数值
int get_detector_counter(detector_counter_t* counter);
// 获取温度，单位为0.1摄氏度
int get_temperature();
// 门是否非法打开
int is_door_illegal_open();
// 获取是否检测故障的标志
int is_detect_health_flag();
// 清除指定检测器的计数值
int clear_detector_counter(int detector);

// 等待100ms
int wait_for_100ms();
// 载入系统配置参数
int load_config_data();
// 保存系统配置参数
int save_config_data();
int save_system_param();
int save_lamp_setting();
int save_lamp_group_setting();
int save_detector_setting();
int save_phase_plan(int plan_no,int step);
int save_timming_plan(int plan_no);
int save_schedule_plan(int plan_no);
int save_special_day();
int save_multiple_schedule_control_param();
int save_actuate_control_param();
int save_cordless_coordinate_control_param();
int save_comm_param(int param_no);
int save_control_mode_selection();
//得到路口机的运行状态
int get_run_mode();
//清除运行状态
int clear_run_mode();
// 是否处于手动控制状态
int is_manual_control_state();
// 前面板设定的控制模式
int get_front_panel_control_mode();
// 是否按了步进按钮
int is_step_forward_button_pressed();
// 清除步进按钮状态
int clear_step_forward_button_state();
//清除是否要改变当前的阶段号的标志
int clear_need_change_stage_flag();
//得到下个阶段号
int get_manual_next_stage_count();
//是否需要改变当前的阶段号
int is_need_change_stage();
// 显示信号机运行状态
//wint display_status()
// 设置系统时间
int set_system_time(date_time_t* dt);
void display_status(unsigned char ctemp1,unsigned char ctemp2); 
// 返回一个fd
int open_comm(int port);
// 读取数据
int read_comm(int fd,byte*  in_buffer, int size);
// 写入数据
int write_comm(int fd,byte* out_buffer, int size);
// 关闭fd对应的通信端口
int close_comm(int fd);

#ifdef __cplusplus
}
#endif

#endif


