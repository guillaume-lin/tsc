/**
 * io.h
 *
 * �Ͳ�Ӳ����ص��������
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-02 10:57:10 �й���׼ʱ��
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

// ��ʼ��
int initialize();
/**
 *  IO ��غ���
 */
// ���ö���״̬
int set_term_status(lamp_status_t* phase);
// ��ȡ�ƵĹ�����Ϣ
int get_lamp_health(lamp_health_t* health);
// ��ȡ������ļ����Ϣ
int get_detector_status(detector_status_t* status);
// ��ȡ������Ĺ�����Ϣ
int get_detector_health(detector_health_t* health);
// ��ȡ���������ֵ
int get_detector_counter(detector_counter_t* counter);
// ��ȡ�¶ȣ���λΪ0.1���϶�
int get_temperature();
// ���Ƿ�Ƿ���
int is_door_illegal_open();
// ��ȡ�Ƿ�����ϵı�־
int is_detect_health_flag();
// ���ָ��������ļ���ֵ
int clear_detector_counter(int detector);

// �ȴ�100ms
int wait_for_100ms();
// ����ϵͳ���ò���
int load_config_data();
// ����ϵͳ���ò���
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
//�õ�·�ڻ�������״̬
int get_run_mode();
//�������״̬
int clear_run_mode();
// �Ƿ����ֶ�����״̬
int is_manual_control_state();
// ǰ����趨�Ŀ���ģʽ
int get_front_panel_control_mode();
// �Ƿ��˲�����ť
int is_step_forward_button_pressed();
// ���������ť״̬
int clear_step_forward_button_state();
//����Ƿ�Ҫ�ı䵱ǰ�Ľ׶κŵı�־
int clear_need_change_stage_flag();
//�õ��¸��׶κ�
int get_manual_next_stage_count();
//�Ƿ���Ҫ�ı䵱ǰ�Ľ׶κ�
int is_need_change_stage();
// ��ʾ�źŻ�����״̬
//wint display_status()
// ����ϵͳʱ��
int set_system_time(date_time_t* dt);
void display_status(unsigned char ctemp1,unsigned char ctemp2); 
// ����һ��fd
int open_comm(int port);
// ��ȡ����
int read_comm(int fd,byte*  in_buffer, int size);
// д������
int write_comm(int fd,byte* out_buffer, int size);
// �ر�fd��Ӧ��ͨ�Ŷ˿�
int close_comm(int fd);

#ifdef __cplusplus
}
#endif

#endif


