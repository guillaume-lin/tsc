/**
 * vm.h
 *
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2005-07-07 15:40:26 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_VM_H
#define INCLUDED_VM_H
#include "datastruct.h"
#include "codec.h"

#ifdef __cplusplus
extern "C" {		// C linkage
#endif

typedef struct {
	int m_fd;
	int m_try_count;	
	frame_t m_frame;
	frame_data_t m_frame_data;
} vm_t;

extern vm_t g_vm;

// 初始化虚拟机
extern int vm_init(vm_t* self);

extern int vm_set_beep_option(vm_t* self,int flag);
// 设置系统时间
extern int vm_set_system_time(vm_t* self, date_time_t* dt);
// 设置系统参数
extern int vm_set_system_param(vm_t* self, system_param_t* sp);
// 设置信号灯状态
extern int vm_set_lamp_setting(vm_t* self, lamp_setting_t* ls);
// 设置灯组参数
extern int vm_set_lamp_group_setting(vm_t* self, lamp_group_setting_t* lgs);
// 设置检测器参数
extern int vm_set_detector_setting(vm_t* self, detector_setting_t* ds);
// 设置相位方案
	extern int vm_set_phase_plan(vm_t* self,int planNo,int step, int step_count,stage_t* phase);
// 设置配时方案
extern int vm_set_timming_plan(vm_t* self, timming_plan_t* plan,int plan_no);
// 设置时段方案
extern int vm_set_schedule_plan(vm_t* self, schedule_plan_t* plan,int plan_no);
// 设置特殊日
extern int vm_set_special_day_table(vm_t* self, special_day_table_t* day);
// 设置通信参数
extern int vm_set_comm_param(vm_t* self, int port_no,comm_param_t* param);
// 设置多时段控制参数
extern int vm_set_multiple_schedule_control_param(vm_t* self, multiple_schedule_control_param_t* param);
// 设置感应控制参数
extern int vm_set_actuate_control_param(vm_t* self, actuate_control_param_t* param);
// 设置无线缆协调控制参数
extern int vm_set_cordless_coordinate_control_param(vm_t* self, cordless_coordinate_control_param_t* param);
// 设置当前运行的控制模式
extern int vm_set_current_control_mode(vm_t* self, control_mode_selection_t* mode);
// 设置系统启动时的控制模式
extern int vm_set_setup_control_mode(vm_t* self, control_mode_selection_t* mode);
// 取系统时间
extern int vm_get_system_time(vm_t* self, date_time_t* dt);
// 取系统参数
extern int vm_get_system_param(vm_t* self, system_param_t* sp);
// 取灯组参数
extern int vm_get_lamp_setting(vm_t* self, lamp_setting_t* ls);
// 取灯组参数
extern int vm_get_lamp_group_setting(vm_t* self, lamp_group_setting_t* lgs);
// 取检测器参数
extern int vm_get_detector_setting(vm_t* self, detector_setting_t* ds);
// 取相位方案
extern int vm_get_phase_plan(vm_t* self,int plan_no,int step, int* step_count, stage_t* phase);

// 取配时方案
extern int vm_get_timming_plan(vm_t* self,int plan_no, timming_plan_t* plan);
// 取时段方案
extern int vm_get_schedule_plan(vm_t* self,int plan_no, schedule_plan_t* plan);

// 取特殊日
extern int vm_get_special_day_table(vm_t* self,special_day_table_t* day);

// 取通信参数
extern int vm_get_comm_param(vm_t* self,int port_no, comm_param_t* param);

// 取多时段控制参数
extern int vm_get_multiple_schedule_control_param(vm_t* self, multiple_schedule_control_param_t* param);
// 取感应控制参数
extern int vm_get_actuate_control_param(vm_t* self, actuate_control_param_t* param);
// 取无线缆协调控制参数
extern int vm_get_cordless_coordinate_control_param(vm_t* self, cordless_coordinate_control_param_t* param);
// 取当前运行的控制模式
extern int vm_get_current_control_mode(vm_t* self, control_mode_selection_t* mode);
// 取系统启动时的控制模式
extern int vm_get_setup_control_mode(vm_t* self, control_mode_selection_t* mode);
// 取信号机当前状态
extern int vm_get_machine_status(vm_t* self, machine_status_t* status);
// 检测器计数值
extern int vm_get_detector_counter(vm_t* self, detector_counter_t* counter);
//取车流量，占有率信息
extern int vm_get_volume_occupy(vm_t* self,volume_occupy_time_t votime, volume_occupy_t* vo);
// 保持
extern int vm_do_step_keep(vm_t* self);
// 步进
extern int vm_do_step_forward(vm_t* self);
// 强制灯色
extern int vm_do_lamp_status(vm_t* self,lamp_status_t* phase);
// 单片机给pc104发送按键
extern int vm_mcu_send_key(vm_t* self, byte key);
extern int vm_mcu_ping(vm_t* self, byte echo);
extern int vm_get_key_val(vm_t* self, byte* key);
//绿冲突设置
int vm_get_green_conflict(vm_t* self, green_conflict_t* conflict);
int vm_set_green_conflict(vm_t* self, green_conflict_t* conflict);
// 中转信息
int vm_set_checkpack_param(vm_t* self,checkpack_t* sp);
//联机命令
extern int vm_send_online_command(vm_t* self,online_control_command_t* sp);
//得到当前版本信息
extern int vm_get_version(vm_t *self,int *version);
#ifdef __cplusplus
};
#endif

#endif
