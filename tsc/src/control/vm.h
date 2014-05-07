/**
 * vm.h
 *
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:   cckk_lin@yahoo.ie
 * Date:    2005-07-07 15:40:26 �й���׼ʱ��
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

// ��ʼ�������
extern int vm_init(vm_t* self);

extern int vm_set_beep_option(vm_t* self,int flag);
// ����ϵͳʱ��
extern int vm_set_system_time(vm_t* self, date_time_t* dt);
// ����ϵͳ����
extern int vm_set_system_param(vm_t* self, system_param_t* sp);
// �����źŵ�״̬
extern int vm_set_lamp_setting(vm_t* self, lamp_setting_t* ls);
// ���õ������
extern int vm_set_lamp_group_setting(vm_t* self, lamp_group_setting_t* lgs);
// ���ü��������
extern int vm_set_detector_setting(vm_t* self, detector_setting_t* ds);
// ������λ����
	extern int vm_set_phase_plan(vm_t* self,int planNo,int step, int step_count,stage_t* phase);
// ������ʱ����
extern int vm_set_timming_plan(vm_t* self, timming_plan_t* plan,int plan_no);
// ����ʱ�η���
extern int vm_set_schedule_plan(vm_t* self, schedule_plan_t* plan,int plan_no);
// ����������
extern int vm_set_special_day_table(vm_t* self, special_day_table_t* day);
// ����ͨ�Ų���
extern int vm_set_comm_param(vm_t* self, int port_no,comm_param_t* param);
// ���ö�ʱ�ο��Ʋ���
extern int vm_set_multiple_schedule_control_param(vm_t* self, multiple_schedule_control_param_t* param);
// ���ø�Ӧ���Ʋ���
extern int vm_set_actuate_control_param(vm_t* self, actuate_control_param_t* param);
// ����������Э�����Ʋ���
extern int vm_set_cordless_coordinate_control_param(vm_t* self, cordless_coordinate_control_param_t* param);
// ���õ�ǰ���еĿ���ģʽ
extern int vm_set_current_control_mode(vm_t* self, control_mode_selection_t* mode);
// ����ϵͳ����ʱ�Ŀ���ģʽ
extern int vm_set_setup_control_mode(vm_t* self, control_mode_selection_t* mode);
// ȡϵͳʱ��
extern int vm_get_system_time(vm_t* self, date_time_t* dt);
// ȡϵͳ����
extern int vm_get_system_param(vm_t* self, system_param_t* sp);
// ȡ�������
extern int vm_get_lamp_setting(vm_t* self, lamp_setting_t* ls);
// ȡ�������
extern int vm_get_lamp_group_setting(vm_t* self, lamp_group_setting_t* lgs);
// ȡ���������
extern int vm_get_detector_setting(vm_t* self, detector_setting_t* ds);
// ȡ��λ����
extern int vm_get_phase_plan(vm_t* self,int plan_no,int step, int* step_count, stage_t* phase);

// ȡ��ʱ����
extern int vm_get_timming_plan(vm_t* self,int plan_no, timming_plan_t* plan);
// ȡʱ�η���
extern int vm_get_schedule_plan(vm_t* self,int plan_no, schedule_plan_t* plan);

// ȡ������
extern int vm_get_special_day_table(vm_t* self,special_day_table_t* day);

// ȡͨ�Ų���
extern int vm_get_comm_param(vm_t* self,int port_no, comm_param_t* param);

// ȡ��ʱ�ο��Ʋ���
extern int vm_get_multiple_schedule_control_param(vm_t* self, multiple_schedule_control_param_t* param);
// ȡ��Ӧ���Ʋ���
extern int vm_get_actuate_control_param(vm_t* self, actuate_control_param_t* param);
// ȡ������Э�����Ʋ���
extern int vm_get_cordless_coordinate_control_param(vm_t* self, cordless_coordinate_control_param_t* param);
// ȡ��ǰ���еĿ���ģʽ
extern int vm_get_current_control_mode(vm_t* self, control_mode_selection_t* mode);
// ȡϵͳ����ʱ�Ŀ���ģʽ
extern int vm_get_setup_control_mode(vm_t* self, control_mode_selection_t* mode);
// ȡ�źŻ���ǰ״̬
extern int vm_get_machine_status(vm_t* self, machine_status_t* status);
// ���������ֵ
extern int vm_get_detector_counter(vm_t* self, detector_counter_t* counter);
//ȡ��������ռ������Ϣ
extern int vm_get_volume_occupy(vm_t* self,volume_occupy_time_t votime, volume_occupy_t* vo);
// ����
extern int vm_do_step_keep(vm_t* self);
// ����
extern int vm_do_step_forward(vm_t* self);
// ǿ�Ƶ�ɫ
extern int vm_do_lamp_status(vm_t* self,lamp_status_t* phase);
// ��Ƭ����pc104���Ͱ���
extern int vm_mcu_send_key(vm_t* self, byte key);
extern int vm_mcu_ping(vm_t* self, byte echo);
extern int vm_get_key_val(vm_t* self, byte* key);
//�̳�ͻ����
int vm_get_green_conflict(vm_t* self, green_conflict_t* conflict);
int vm_set_green_conflict(vm_t* self, green_conflict_t* conflict);
// ��ת��Ϣ
int vm_set_checkpack_param(vm_t* self,checkpack_t* sp);
//��������
extern int vm_send_online_command(vm_t* self,online_control_command_t* sp);
//�õ���ǰ�汾��Ϣ
extern int vm_get_version(vm_t *self,int *version);
#ifdef __cplusplus
};
#endif

#endif
