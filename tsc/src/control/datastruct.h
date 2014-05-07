/**
 * datastruct.h
 *
 * data struct for signal controller
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-06-27 14:44:07 (�й���׼ʱ��)
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_DATA_STRUCT_H
#define INCLUDED_DATA_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

// ֧�ֵ����������Ӹ���
#define MAX_TERMINAL_COUNT 72 

// ��λ�����Ҫ���ֽ���
#define MAX_OUTPUT_COUNT 18	// MAX_TERMINAL_COUNT / 4

// ֧�ֵ����������
#define MAX_DETECTOR_COUNT 32

// ֧�ֵ������ո���
#define MAX_SPECIAL_DAY_COUNT 10

// �źŵ�״̬
#define OFF 0
#define ON  1
#define FLASH 2

// �źŵ���ɫ
#define RED 0
#define GREEN 1
#define YELLOW 2

// ֧�ֵ���������
#define MAX_LAMP_GROUP_COUNT 24 

// ֧�ֵ����Ĳ�����
#define MAX_STEP_COUNT 96	

// ֧�ֵ���λ������
#define MAX_PHASE_PLAN_COUNT 6

// ֧�ֵ���ʱ������
#define MAX_TIMMING_PLAN_COUNT 32

// ֧�ֵ�ʱ�η�����
#define MAX_SCHEDULE_PLAN_COUNT 20

// ÿ��ʱ�η����ɻ��ֵ�ʱ����
#define MAX_TIME_SEGMENT_COUNT 24

// ֧�ֵ�ͨ�Ŷ˿���
#define MAX_COMM_COUNT 6
// ���֧��8����λ�׶�
#define MAX_STAGE_COUNT 8     
// ���֧��ÿ����λ�׶�12��λ����
#define MAX_PHASE_COUNT 12
//��������ռ���ʱ���ĸ���
#define MAX_VOLUME_OCCCUPY_NUM 60
     
typedef unsigned char byte;
// ��������
typedef struct{
	// ��������
	byte m_lamp_group_count;
	byte m_lamp_group[MAX_LAMP_GROUP_COUNT][3];
} lamp_group_setting_t;
//�źŵ�����
typedef struct{
	//�Ƿ����ã�0 ��ʾ���ã�1��ʾ����
	byte  m_use[MAX_TERMINAL_COUNT/8];	// 9���ֽ�
} lamp_setting_t;
// ���������
typedef struct {
	// ���ԣ��������������ȣ��Ƿ�����
	// [7][6][5][4][3][2][1][0]
	// [7] - 0 ��ʾ���Σ�1��ʾ����
	// [6] - 0 ��ʾ���ã�1��ʾ����
	// [1][0] �������������
	//
	byte m_attribute[MAX_DETECTOR_COUNT];
	// ��Ӧ�ĵ���
	byte m_lamp_group[MAX_DETECTOR_COUNT];
	// �����μ�����ľ���
	byte m_distance;
} detector_setting_t;

// �̳�ͻ��
typedef struct {
	// ����������ӳ�ͻ������Ӧ��λ��Ϊ1
	byte m_conflict[(MAX_LAMP_GROUP_COUNT-1)*MAX_LAMP_GROUP_COUNT/2/8+1];
} green_conflict_t;

// ��λ
typedef struct {
	byte m_output[MAX_OUTPUT_COUNT];
} lamp_status_t;

// ��λ����
typedef struct {
	// ����ʱ��
	byte m_safe_guard_time;
	byte m_attr;
	// [7][6][5][4][3][2][1][0]
	// [7] 1�����ɱ䲽��0 �������ɲ�
	//
} phase_attr_t;

///////////////////////////////
//������λ����
// ������λ
#define PHASE_YELLOW_FLASH 253
// ȫ����λ
#define PHASE_ALL_RED      254
// �����λ
#define PHASE_ALL_OFF      255
// ������λ����ʱ
#define TIMMING_SPECIAL_PHASE 255

// ��λ����
//typedef struct {
//	byte m_step;
//	phase_t m_phase[MAX_STEP_COUNT];
//} phase_plan_t;
//�׶β���
typedef struct {
// 1- MAX_PHASE_COUNT ������Ӧ����λ���0����û����λ����
		byte m_phase[MAX_PHASE_COUNT]; 
} stage_t;
typedef struct {
		byte m_stage_count;
		stage_t m_stage[MAX_STAGE_COUNT];
}stage_plan_t;
// ��ʱ����
typedef struct {
	byte m_step;
	byte m_greentimming[MAX_STAGE_COUNT];
	byte m_greenflashtimming[MAX_STAGE_COUNT];
	byte m_yellowtimming[MAX_STAGE_COUNT];
	byte m_redtimming[MAX_STAGE_COUNT];
} timming_plan_t;

// ʱ�η���
typedef struct {
	// ��Чʱ�εĸ���
	byte m_time_segment_count;
	byte m_hour[MAX_TIME_SEGMENT_COUNT];
	byte m_minute[MAX_TIME_SEGMENT_COUNT];
	byte m_phase[MAX_TIME_SEGMENT_COUNT];
	byte m_timming[MAX_TIME_SEGMENT_COUNT];
} schedule_plan_t;


// ��ƿ���
#define ALL_OFF_CONTROL             1
// ȫ�����
#define ALL_RED_CONTROL             2
// ��������
#define YELLOW_FLASH_CONTROL        3
// ��ο���
#define MULTIPLE_SCHEDULE_CONTROL   4
// ��Ӧ����
#define ACTUATE_CONTROL        	    5
// ������Э������
#define CORDLESS_COORDINATE_CONTROL 6
// ����Э������
#define ONLINE_HARMONY_CONTROL      7
// �ֿؿ���
#define MANUAL_CONTROL		        8	

// ִ�л�����
#define EXECUTIVE_MACHINE_CONTROL   9

// �ƵĹ�����Ϣ
typedef struct {
	byte m_health[MAX_TERMINAL_COUNT/8];	// 9 ���ֽ�
} lamp_health_t;

// �����״̬(�г����޳�)
typedef struct {
	byte m_status[MAX_DETECTOR_COUNT/8];	// 4 ���ֽ�
} detector_status_t;

// �����������Ϣ
typedef struct {
	byte m_health[MAX_DETECTOR_COUNT/8];	// 4 ���ֽ�
} detector_health_t;

// ���������ֵ
typedef struct {
	byte m_counter[MAX_DETECTOR_COUNT];	// 32 ���ֽ�
}detector_counter_t;

// ʱ����Ϣ
typedef struct {
	byte m_year[2];// 20,05 ��һ���ֽڴ�����ͣ��ڶ����ֽڴ�����
	byte m_month;	// 1 - 12
	byte m_day;	// 1 - 31
	byte m_hour;	// 0 - 23
	byte m_minute;	// 0 - 59
	byte m_second;	// 0 - 59
	byte m_weekday;	// 0 - 6, 0 ����������
} date_time_t;

/**
 *  ����״̬
 */
typedef struct {
	byte m_is_door_illegal_open : 1;
	byte m_hard_error;	// Ӳ��������
	byte m_soft_error;	// ����������
} machine_error_status_t;
//��ת��Ϣ
typedef struct tagcheckpack 
{ 
	unsigned long userid; 
	unsigned long port; 
	char  szip[16]; 
}checkpack_t;
//������������
typedef struct{
	byte m_cmd;
	byte m_param1;
	byte m_param2;
}online_control_command_t;
typedef struct {
	byte m_year[2]; // 20,05 ��һ���ֽڴ�����ͣ��ڶ����ֽڴ�����
	byte m_month;
	byte m_day;
	byte m_hour;
	byte m_minute;
	// ������
	byte m_volume[MAX_DETECTOR_COUNT];
	// ռ����
	byte m_occupy[MAX_DETECTOR_COUNT];
} volume_occupy_t;
//��������ռ���ʵ�ʱ��
typedef struct{
	byte m_year[2]; // 20,05 ��һ���ֽڴ�����ͣ��ڶ����ֽڴ�����
	byte m_month;
	byte m_day;
	byte m_hour;
	byte m_minute;
}volume_occupy_time_t;
// ��������ռ������Ϣ��
typedef struct {
	volume_occupy_t vo[MAX_VOLUME_OCCCUPY_NUM];
	byte            m_pointer;
}volume_occupy_group_t;
typedef struct {
	byte m_is_degraded;	// ���ڽ�������״̬
	// ʱ��
	byte m_second;
	byte m_minute;
	byte m_hour;
	byte m_week;  // -1 ���ʱ��оƬ������
	byte m_day;
	byte m_month;
	byte m_year[2]; // 20,05 ��һ���ֽڴ�����ͣ��ڶ����ֽڴ�����

	// �趨�Ŀ���ģʽ
	byte m_designate_control_mode;
	// ʵ�����еĿ���ģʽ
	byte m_actual_control_mode;
	// ʱ�η���
	byte m_schedule;
	// ��ʱ����
	byte m_timming;
	// ��λ����
	byte m_phase;
	//�׶κţ���ǰ��
	byte m_stage;
	// ������
	byte m_step;
	// ��ǰ��������ʱ��
	byte m_elapse_time;
	// �趨�Ĳ���ʱ��
	byte m_assign_time;
	// ʣ��Ĳ���ʱ��
	byte m_remain_time;
	//���н���״̬�Ĳ���ʱ��
	byte m_degraded_time;

	// ��ǰ��ɫ
	lamp_status_t 		m_current_lamp_status;
	detector_status_t 	m_current_detector_status;
	detector_counter_t 	m_current_detector_counter;
	detector_health_t 	m_detector_health;
	lamp_health_t 		m_lamp_health;

	// ��ǰ��������
	byte m_command;

	// �¶ȵ���������
	byte m_temperature_integer;
	// �¶ȵ�С������
	byte m_temperature_decimal;
	
	// ����״̬
	machine_error_status_t m_error;
} machine_status_t;

// ��������
//����
#define ONLINE          1
// ����
#define STAGE_KEEP  	2
// ����
#define STAGE_FORWARD 	3
//����
#define STAGE_JUMP      4
//�ѻ�
#define  OFFLINE        5
// ǿ�Ƶ�ɫ
#define DIRECT_PHASE	6
// ȫ��״̬
typedef struct {
	
	byte m_is_degraded;	// ���ڽ�������״̬
	// ʱ��
	byte m_second;
	byte m_minute;
	byte m_hour;
	byte m_week;  	// -1 ���ʱ��оƬ������
	byte m_day;
	byte m_month;
	byte m_year[2]; // 20,05 ��һ���ֽڴ�����ͣ��ڶ����ֽڴ�����

	// �趨�Ŀ���ģʽ
	byte m_designate_control_mode;
	// ʵ�����еĿ���ģʽ
	byte m_actual_control_mode;
	// ʱ�η���
	byte m_schedule;
	// ��ʱ����
	byte m_timming;
	// ��λ����
	byte m_phase;
	// ��λ��
	byte m_step;
	//�׶κţ���ǰ��
	byte m_stage;
	//��һ�׶κ�
	byte m_next_stage;
	// ��ǰ��������ʱ��
	byte m_elapse_time;
	// �趨�Ĳ���ʱ��
	byte m_assign_time;
	// ʣ��Ĳ���ʱ��
	byte m_remain_time;
	//���н���״̬�Ĳ���ʱ��
	byte m_degraded_time;
//  ��ǰ����λ����������
//	byte m_command;

	// �źŻ��¶�
	int m_temperature;

	machine_error_status_t m_error;

	// ʼ�ռ�ʱ��
	int m_timer;
	//////////////////////////////
	lamp_status_t   m_current_lamp_status;
	// ָ��ǰ������λ����ָ��
	stage_plan_t*   m_current_phase_plan_p;
	//ָ���¸�������λ����ָ��
	stage_plan_t*   m_next_phase_plan_p;
	// ָ��ǰ������ʱ������ָ��
	timming_plan_t* m_current_timming_plan_p;

	
	detector_status_t 	m_current_detector_status;
	detector_counter_t 	m_current_detector_counter;
	detector_health_t 	m_detector_health;
	lamp_health_t 		m_lamp_health;

	// ��0 ��ʾ������ѭ����Ҫ��������
	byte m_restart:1;	

	// ���������Ƿ���ģ�����ǣ���Ҫ���������ñ�������
	byte m_is_config_updated:1;

	// ������ť״̬��1��ʾ���¹�
	byte m_step_forward_button_state:1;
	byte m_need_change_stage_flag:1;
	// ǿ�Ƶ�ɫ
	lamp_status_t m_direct_phase;
	//������Ϣ
	online_control_command_t m_online_command;
	//�Ƿ���ܿ�������
	byte  m_bNoRevCommand;
} controller_status_t;

// ������
typedef struct {
	// ��ʼ��
	byte m_start_month;
	// ��ʼ��
	byte m_start_day;
	// ������
	byte m_end_month;
	// ������
	byte m_end_day;
	// ʱ�η���
	byte m_schedule;
} special_day_t;

// �����ձ�
typedef struct {
	special_day_t m_day[MAX_SPECIAL_DAY_COUNT];
} special_day_table_t;

// �˿�����
#define RS_232 1
#define RS_485 2
#define ETHER 3

// ������
#define B_600 	1
#define B_1200 	2
#define B_2400	3
#define B_4800  4
#define B_9600  5
#define B_19200 6

//

// ͨ�Ų���
typedef struct {
	// �˿�����
	byte m_type;
	// ������
	byte m_baud;
	// IP��ַ
	byte m_ip[4];
	// ͨ��Э��
	byte m_protocol;
	// �Ƿ�����
	byte m_enable;
}  comm_param_t;

// ϵͳ��������
typedef struct {
	// ����ʱ��
	byte m_initial_yellow_flash_time;
	// �����ʱ��
	byte m_initial_all_red_time;
	//���ڽ���״̬��ʱ��
	byte m_run_degraded_time;
} system_param_t ;

// ѡ�����ģʽʱ�������źŻ�ǰ����ѡ������
#define CONTROL_MODE_FRONT_PANEL_SELECT 0

// ����ģʽѡ��
typedef struct {
	// ���к��ֿ���ģʽ
	byte m_control_mode;
} control_mode_selection_t;

// ��ο���ģʽ
typedef struct {
	byte m_enable_special;
	byte m_schedule_plan;
} multiple_schedule_control_param_t;

// ���Ӧ���Ʋ���
#define STRATEGY_HALF_ACTUATE 	1
// ȫ��Ӧ���Ʋ���
#define STRATEGY_FULL_ACTUATE	2

// ��Ȧ��ⷽʽ
#define LOOP_DETECT_METHOD	1
// ��Ƶ��ⷽʽ
#define VIDEO_DETECT_METHOD	2

// ��Ӧ���Ʋ���
typedef struct {
	// ���Ʋ��ԣ�ȫ��Ӧ���߰��Ӧ��������
	byte m_strategy;

	// ������ⷽ��(��Ƶ������Ȧ)
	byte m_detect_method;

	// ��С��
	byte m_minmum_green;
	// �����
	byte m_maximum_green;
	// ÿ�εĵ�����
	byte m_adjust_quantum;
	// ��ͷʱ��(��λΪ0.1��)
	byte m_vehicle_gap;
	// ʹ�õ���λ
//	byte m_phase;
	// ʹ�õ���ʱ
//	byte m_timming;

} actuate_control_param_t;

// ������Э������
typedef struct {
	// ��ʼСʱ
	byte m_start_hour;
	// ��ʼ����
	byte m_start_minute;
	// ��λ��
	byte m_offset;
	// ��λ����
	byte m_phase;
	// ��ʱ����
	byte m_timming;
} cordless_coordinate_control_param_t;
// ��������ռ����


// ��ͨͳ������
typedef struct {
	// ͳ�Ƽ������λ���ӣ�
	byte m_interval;
	// �Ƿ��ӡ
	byte m_should_print:1;
	// ��ӡʱ��
	byte m_print_hour;
	byte m_print_minute;

} traffic_stat_t;
// ����ϵͳ���������ò���
typedef struct {
	system_param_t 		m_system_param;											// ռ�� 2 ���ֽ�
	lamp_group_setting_t 	m_lamp_group_setting;								// ռ�� 1 + 3 * 32  �� 97 ���ֽ�
	detector_setting_t  	m_detector_setting;									// ռ�� 1 * 32 + 1 * 32 + 1 �� 65 ���ֽ�

	stage_plan_t    	m_phase_table[MAX_PHASE_PLAN_COUNT];					// ռ�� 6 * (1 + 18 * 96)  �� 10374 ���ֽ� 
	timming_plan_t  	m_timming_table[MAX_TIMMING_PLAN_COUNT];				// ռ�� 32 * (1 + 96) �� 3104 ���ֽ�
	schedule_plan_t 	m_schedule_table[MAX_SCHEDULE_PLAN_COUNT];				// ռ�� 20 * (1 + 24 + 24 + 24 + 24) �� 1940 ���ֽ�

	special_day_table_t   	m_special_day_table;								// ռ�� 5 * 10 �� 40 ���ֽ�
	multiple_schedule_control_param_t m_multiple_schedule_control_param;		// ռ�� 1 + 1 �� 2 ���ֽ�
	actuate_control_param_t m_actuate_control_param;							// ռ�� 8 ���ֽ�
	cordless_coordinate_control_param_t m_cordless_coordinate_control_param;	// ռ�� 5 ���ֽ�
	comm_param_t m_comm_param[MAX_COMM_COUNT];									// ռ�� 6 * 8 �� 48 ���ֽ�
	control_mode_selection_t m_control_mode_selection;							// ռ�� 1 ���ֽ�
	green_conflict_t      m_green_conflict;
	lamp_setting_t        m_lamp_setting;										    // ռ�� 9 ���ֽ�
} system_config_t ;																// �� 15705 ���ֽ�

	int lamp_group_setting_get_lamp_group_count(lamp_group_setting_t* self);
	int green_conflict_is_conflict(green_conflict_t* self,int n, int m);
	int green_conflict_set_conflict(green_conflict_t* self,int n, int m, int conflict);
	int phase_set_output(lamp_status_t* self, int n, int v);
	int phase_get_output(lamp_status_t* self,int n);
	int is_phase_green_conflict(lamp_status_t* phase, green_conflict_t* conflict);
	int lamp_health_get_health(lamp_health_t* self, int n);
	int lamp_health_set_health(lamp_health_t* self,int n, int v);
	int detector_health_get_health(detector_health_t* self, int n);
	int detector_health_set_health(detector_health_t* self, int n, int v);
	int detector_status_get_status(detector_status_t* self, int n);
	int detector_status_set_status(detector_status_t* self, int n, int v);
	int lamp_group_setting_get_terminal(lamp_group_setting_t* self,int lamp_no);
#ifdef __cplusplus
}
#endif

#endif