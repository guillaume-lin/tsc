/**
 * command_reactor.c
 * 
  * 
 * 
* Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-08 09:46:21 中国标准时间
 * 
 * $log$
 *
 */
#include "string.h"
#include "command_reactor.h"
#include "kernel.h"
#include "codec.h"
#include "io.h"
#include "util.h"

//w
extern int save_green_conflict();
//w

#ifdef __C51__
extern bit f_buzz; 		
#endif

//command_reactor_t g_command_reactor2,g_command_reactor7,g_command_reactor8;	
typedef int (*command_handler_t)(command_reactor_t* self);
typedef struct {
	byte m_cmd;
	command_handler_t m_handler;
} command_handler_entry_t;

/**
 * 未知命令的处理函数
 */
static int handle_unsupport_command(command_reactor_t* self)
{
	self->m_frame.m_command = 0x00;
	frame_set_data_len(&self->m_frame,0);
	return 0;
}

int handle_set_system_time(command_reactor_t* self);
int handle_set_system_param(command_reactor_t* self);
int handle_set_lamp_setting(command_reactor_t* self);
int handle_set_lamp_group_setting(command_reactor_t* self);
int handle_set_detector_setting(command_reactor_t* self);
int handle_set_phase_plan(command_reactor_t* self);
int handle_set_timming_plan(command_reactor_t* self);
int handle_set_schedule_plan(command_reactor_t* self);
int handle_set_special_day_table(command_reactor_t* self);
int handle_set_current_control_mode(command_reactor_t* self);
int handle_set_setup_control_mode(command_reactor_t* self);
int handle_set_multiple_schedule_control_param(command_reactor_t* self);
int handle_set_comm_param(command_reactor_t* self);
int handle_set_actuate_control_param(command_reactor_t* self);
int handle_set_cordless_coordinate_control_param(command_reactor_t* self);

int handle_get_system_time(command_reactor_t* self);
int handle_get_system_param(command_reactor_t* self);
int handle_get_lamp_setting(command_reactor_t* self);
int handle_get_lamp_group_setting(command_reactor_t* self);
int handle_get_detector_setting(command_reactor_t* self);
int handle_get_phase_plan(command_reactor_t* self);
int handle_get_timming_plan(command_reactor_t* self);
int handle_get_schedule_plan(command_reactor_t* self);
int handle_get_special_day_table(command_reactor_t* self);
int handle_get_machine_status(command_reactor_t* self);
int handle_get_current_control_mode(command_reactor_t* self);
int handle_get_setup_control_mode(command_reactor_t* self);
int handle_get_multiple_schedule_control_param(command_reactor_t* self);
int handle_get_comm_param(command_reactor_t* self);
int handle_get_detector_counter(command_reactor_t* self);
int handle_get_detector_status(command_reactor_t* self);
int handle_get_detector_health(command_reactor_t* self);
int handle_get_lamp_health(command_reactor_t* self);
int handle_get_volume_occupy(command_reactor_t* self);
int handle_get_actuate_control_param(command_reactor_t* self);
int handle_get_cordless_coordinate_control_param(command_reactor_t* self);

int handle_do_step_forward(command_reactor_t* self);
int handle_do_step_keep(command_reactor_t* self);
int handle_mcu_send_key(command_reactor_t* self);
int handle_get_key_val(command_reactor_t* self);

int handle_get_green_conflict(command_reactor_t* self);
int handle_set_green_conflict(command_reactor_t* self);

int handle_send_online_command(command_reactor_t* self);
int handle_get_version(command_reactor_t* self);

/**
 * 命令处理函数数组
 */
static 
#ifdef __C51__
code		// C51需要把下面的表定义到code里面,要不然find_handler没法找到handler
#endif
command_handler_entry_t command_handler_map[] = 
{
	{CMD_SET_SYSTEM_TIME,handle_set_system_time},
	{CMD_SET_SYSTEM_PARAM,handle_set_system_param},
	{CMD_SET_LAMP_SETTING,handle_set_lamp_setting},
	{CMD_SET_LAMP_GROUP_SETTING,handle_set_lamp_group_setting},
	{CMD_SET_DETECTOR_SETTING,handle_set_detector_setting},
	{CMD_SET_PHASE_PLAN,handle_set_phase_plan},
	{CMD_SET_TIMMING_PLAN,handle_set_timming_plan},
	{CMD_SET_SCHEDULE_PLAN,handle_set_schedule_plan},
	{CMD_SET_SPECIAL_DAY_TABLE,handle_set_special_day_table},
	{CMD_SET_CURRENT_CONTROL_MODE,handle_set_current_control_mode},
	{CMD_SET_MULTIPLE_SCHEDULE_CONTROL_PARAM,handle_set_multiple_schedule_control_param},
	{CMD_SET_COMM_PARAM,handle_set_comm_param},
	{CMD_SET_ACTUATE_CONTROL_PARAM,handle_set_actuate_control_param},
	{CMD_SET_CORDLESS_COORDINATE_CONTROL_PARAM,handle_set_cordless_coordinate_control_param},
		
	{CMD_GET_SYSTEM_TIME,handle_get_system_time},
	{CMD_GET_SYSTEM_PARAM,handle_get_system_param},
	{CMD_GET_LAMP_SETTING,handle_get_lamp_setting},
	{CMD_GET_LAMP_GROUP_SETTING,handle_get_lamp_group_setting},
	{CMD_GET_DETECTOR_SETTING,handle_get_detector_setting},
	{CMD_GET_PHASE_PLAN,handle_get_phase_plan},
	{CMD_GET_TIMMING_PLAN,handle_get_timming_plan},
	{CMD_GET_SCHEDULE_PLAN,handle_get_schedule_plan},
	{CMD_GET_SPECIAL_DAY_TABLE,handle_get_special_day_table},
	{CMD_GET_MACHINE_STATUS,handle_get_machine_status},
	{CMD_GET_CURRENT_CONTROL_MODE,handle_get_current_control_mode},
	{CMD_GET_MULTIPLE_SCHEDULE_CONTROL_PARAM,handle_get_multiple_schedule_control_param},
	{CMD_GET_COMM_PARAM,handle_get_comm_param},
	{CMD_GET_DETECTOR_COUNTER,handle_get_detector_counter},
	{CMD_GET_DETECTOR_STATUS,handle_get_detector_status},
	{CMD_GET_DETECTOR_HEALTH,handle_get_detector_health},
	{CMD_GET_LAMP_HEALTH,handle_get_lamp_health},
	{CMD_GET_VOLUME_OCCUPY,handle_get_volume_occupy},
	{CMD_GET_ACTUATE_CONTROL_PARAM,handle_get_actuate_control_param},
	{CMD_GET_CORDLESS_COORDINATE_CONTROL_PARAM,handle_get_cordless_coordinate_control_param},

	{CMD_DO_STEP_FORWARD,handle_do_step_forward},
	{CMD_DO_STEP_KEEP, handle_do_step_keep},
	{CMD_MCU_SEND_KEY, handle_mcu_send_key},
	{CMD_GET_KEY_VAL,handle_get_key_val},
	{CMD_SET_GREEN_CONFLICT,handle_set_green_conflict},
	{CMD_SET_SETUP_CONTROL_MODE,handle_set_setup_control_mode},
	{CMD_GET_GREEN_CONFLICT,handle_get_green_conflict},
	{CMD_GET_SETUP_CONTROL_MODE,handle_get_setup_control_mode},
	{CMD_SEND_ONLINE_COMMAND,handle_send_online_command},
	{CMD_GET_VERSION,handle_get_version},
};

static command_handler_t find_handler(byte cmd)
{
	int i;
	for(i=0; i<sizeof(command_handler_map)/sizeof(command_handler_entry_t);
			i++){
		if(command_handler_map[i].m_cmd == cmd)
			return command_handler_map[i].m_handler;
	}
	return handle_unsupport_command;
}

/**
 * 初始化
 */
int init_command_reactor(command_reactor_t* self)
{
	self->m_fd = -1;
	init_frame(&self->m_frame,&self->m_frame_data);
	init_key_buffer(&self->m_key_buffer);
	return 0;
}

// 处理通信命令
int reactor_handle_command(command_reactor_t* self)
{
	int ret;
	int fd = self->m_fd;

	ret = read_frame(fd,&self->m_frame);

	if(ret > 0){
//		byte temp[5];
		
		byte cmd = self->m_frame.m_command;
		command_handler_t handler;
		// 调用命令处理函数, cmd 即为命令ID
		handler = find_handler(cmd);
//		display_status(cmd,cmd);

#ifdef __C51__
		if ((self->m_frame.m_reserve & 0x01) == 0x01) {
//			*temp = self->m_frame.m_header;
//			*(temp+1) = self->m_frame.m_command;
//			*(temp+2) = self->m_frame.m_reserve;
//			*(temp+3) = self->m_frame.m_data_len;
//			*(temp+4) = self->m_frame.m_checksum;
//			write_comm(2,temp,5);
			f_buzz = 1;
			}
#endif
	
		handler(self);
		ret = write_frame(fd,&self->m_frame);
	}

	return ret;
}

/**
 * 填充接收正确的应答
 */
static int fill_ok_response(frame_t* frame)
{
	frame->m_command = get_response_cmd(frame->m_command);
	frame_set_data_len(frame,0);
	return 0;
}
/**
 * 填充接收错误的应答
 */
static int fill_error_response(frame_t* frame)
{
	frame->m_command = 0x00;
	frame_set_data_len(frame,0);
	return 0;
}

/**
 * 系统时间
 */
int handle_set_system_time(command_reactor_t* self)
{
	date_time_t dt;
	decode_set_system_time_req(&dt,&self->m_frame);

	// 设置系统时间
	if(set_system_time(&dt) == 0)
		fill_ok_response(&self->m_frame);
	else
		fill_error_response(&self->m_frame);

	return 0;
}
int handle_get_system_time(command_reactor_t* self)
{
	date_time_t dt;
	dt.m_year[0]  = g_controller_status.m_year[0];
	dt.m_year[1] = g_controller_status.m_year[1];
	dt.m_month = g_controller_status.m_month;
	dt.m_day   = g_controller_status.m_day;
	dt.m_hour  = g_controller_status.m_hour;
	dt.m_minute = g_controller_status.m_minute;
	dt.m_second = g_controller_status.m_second;
	dt.m_weekday = g_controller_status.m_week;
	encode_get_system_time_res(&dt,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);

	return 0;
}

int handle_set_system_param(command_reactor_t* self)
{
	decode_set_system_param_req(&g_config.m_system_param,&self->m_frame);
	//判断参数有效性
	if(g_config.m_system_param.m_initial_all_red_time<0||g_config.m_system_param.m_initial_all_red_time>99)
		g_config.m_system_param.m_initial_all_red_time=4;
	if(g_config.m_system_param.m_initial_yellow_flash_time<0||g_config.m_system_param.m_initial_yellow_flash_time>99)
		g_config.m_system_param.m_initial_yellow_flash_time=4;
	save_system_param();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_system_param(command_reactor_t* self)
{
	encode_get_system_param_res(&g_config.m_system_param,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_get_lamp_setting(command_reactor_t* self)
{
	encode_get_lamp_setting_res(&g_config.m_lamp_setting,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_set_lamp_setting(command_reactor_t* self)
{
	decode_set_lamp_setting_req(&g_config.m_lamp_setting,&self->m_frame);
	save_lamp_setting();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_set_lamp_group_setting(command_reactor_t* self)
{
	int i=0;
	int j=0;
	decode_set_lamp_group_setting_req(&g_config.m_lamp_group_setting,&self->m_frame);
//0921	init_special_schedule_data();
	//判断参数有效性
	if(g_config.m_lamp_group_setting.m_lamp_group_count>MAX_LAMP_GROUP_COUNT||g_config.m_lamp_group_setting.m_lamp_group_count<0)
		g_config.m_lamp_group_setting.m_lamp_group_count=0;
	for(i=0;i<MAX_LAMP_GROUP_COUNT;i++)
	{
		for (j=0;j<3;j++)
		{
			if(g_config.m_lamp_group_setting.m_lamp_group[i][j]>MAX_TERMINAL_COUNT||g_config.m_lamp_group_setting.m_lamp_group[i][j]<0)
				g_config.m_lamp_group_setting.m_lamp_group[i][j]=0;
		}
	}	
	save_lamp_group_setting();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_lamp_group_setting(command_reactor_t* self)
{
	encode_get_lamp_group_setting_res(&g_config.m_lamp_group_setting,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_set_detector_setting(command_reactor_t* self)
{
	int i=0;
	decode_set_detector_setting_req(&g_config.m_detector_setting,&self->m_frame);
	//判断参数有效性
	for (i=0;i<MAX_DETECTOR_COUNT;i++)
	{
		if(g_config.m_detector_setting.m_lamp_group[i]>MAX_LAMP_GROUP_COUNT||g_config.m_detector_setting.m_lamp_group[i]<0)
			g_config.m_detector_setting.m_lamp_group[i]=0;
	}
	save_detector_setting();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_detector_setting(command_reactor_t* self)
{
	encode_get_detector_setting_res(&g_config.m_detector_setting,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}

/**
 *  设置相位方案
 */
int handle_set_phase_plan(command_reactor_t* self)
{
	int i;
	int plan_no;
	int step_count;
	int step;
	stage_t phase;
	decode_set_phase_plan_req(&plan_no,&step_count,&step,&phase,&self->m_frame);
	if(plan_no > MAX_PHASE_PLAN_COUNT || step > MAX_STAGE_COUNT||step<1||plan_no<1){
		fill_error_response(&self->m_frame);
		return 0;
	}
	g_config.m_phase_table[plan_no-1].m_stage_count = step_count;
	g_config.m_phase_table[plan_no-1].m_stage[step-1] = phase;	
	//判断参数有效性
	if(step_count>MAX_STAGE_COUNT||step_count<0)
		g_config.m_phase_table[plan_no-1].m_stage_count=0;
	for (i=0;i<MAX_PHASE_COUNT;i++)
	{
		if(g_config.m_phase_table[plan_no-1].m_stage[step-1].m_phase[i]>MAX_LAMP_GROUP_COUNT
			||g_config.m_phase_table[plan_no-1].m_stage[step-1].m_phase[i]<0)
			g_config.m_phase_table[plan_no-1].m_stage[step-1].m_phase[i]=0;
	}
	//保存
	save_phase_plan(plan_no,step);
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_phase_plan(command_reactor_t* self)
{
	int plan_no;
	int step;
	stage_plan_t* phase_plan;
	decode_get_phase_plan_req(&plan_no,&step,&self->m_frame);

	if(plan_no > MAX_PHASE_PLAN_COUNT || step > MAX_STAGE_COUNT||step<1||plan_no<1)
	{
		fill_error_response(&self->m_frame);
		return 0;
	}
	phase_plan = &g_config.m_phase_table[plan_no - 1];
	encode_get_phase_plan_res(phase_plan->m_stage_count,
				  &phase_plan->m_stage[step -1],&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}

int handle_set_timming_plan(command_reactor_t* self)
{
	int i;
	int plan_no;
	timming_plan_t timming;
	decode_set_timming_plan_req(&plan_no,&timming,&self->m_frame);

	if(plan_no < 1 || plan_no > MAX_TIMMING_PLAN_COUNT){
		fill_error_response(&self->m_frame);
		return 0;
	}
	//判断配时的有效性
	for(i=0;i<MAX_STAGE_COUNT;i++)
	{
		if(timming.m_greenflashtimming[i]>255||timming.m_greenflashtimming[i]<0)
			timming.m_greenflashtimming[i]=0;
		if(timming.m_greentimming[i]>255||timming.m_greentimming[i]<0)
			timming.m_greentimming[i]=0;
		if(timming.m_yellowtimming[i]>255||timming.m_yellowtimming[i]<0)
			timming.m_yellowtimming[i]=0;
		if(timming.m_redtimming[i]>255||timming.m_redtimming[i]<0)
			timming.m_redtimming[i]=0;
	}
	//保存
	g_config.m_timming_table[plan_no - 1] = timming;
	save_timming_plan(plan_no);
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_timming_plan(command_reactor_t* self)
{
	int plan_no;
	decode_get_timming_plan_req(&plan_no,&self->m_frame);

	if(plan_no < 1 || plan_no > MAX_TIMMING_PLAN_COUNT){
		fill_error_response(&self->m_frame);
		return 0;
	}
	encode_get_timming_plan_res(&g_config.m_timming_table[plan_no-1],&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_set_schedule_plan(command_reactor_t* self)
{
	int i;
	int plan_no;
	schedule_plan_t schedule;
	decode_set_schedule_plan_req(&plan_no,&schedule,&self->m_frame);
	if(plan_no < 1 || plan_no > MAX_SCHEDULE_PLAN_COUNT){
		fill_error_response(&self->m_frame);
		return 0;
	}
	//判断参数有效性
	for (i=0;i<MAX_TIME_SEGMENT_COUNT;i++)
	{
		if(/*schedule.m_phase[i]>MAX_PHASE_PLAN_COUNT||*/schedule.m_phase[i]<0)
			schedule.m_phase[i]=0;
		if(/*schedule.m_timming[i]>MAX_TIMMING_PLAN_COUNT||*/schedule.m_timming[i]<0)
			schedule.m_timming[i]=0;
	}
	g_config.m_schedule_table[plan_no-1] = schedule;
	save_schedule_plan(plan_no);
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_schedule_plan(command_reactor_t* self)
{
	int plan_no;
	decode_get_schedule_plan_req(&plan_no,&self->m_frame);
	if(plan_no>MAX_SCHEDULE_PLAN_COUNT||plan_no<1)
	{
		fill_error_response(&self->m_frame);
		return 0;
	}
	encode_get_schedule_plan_res(&g_config.m_schedule_table[plan_no-1],&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_set_special_day_table(command_reactor_t* self)
{
	int i;
	decode_set_special_day_table_req(&g_config.m_special_day_table,&self->m_frame);
	for (i=0;i<MAX_SPECIAL_DAY_COUNT;i++)
	{
		if(g_config.m_special_day_table.m_day[i].m_schedule>MAX_SCHEDULE_PLAN_COUNT||g_config.m_special_day_table.m_day[i].m_schedule<=0)
			g_config.m_special_day_table.m_day[i].m_schedule=1;
	}
	save_special_day();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_special_day_table(command_reactor_t* self)
{
	encode_get_special_day_table_res(&g_config.m_special_day_table,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_set_comm_param(command_reactor_t* self)
{
	int port_no;
	comm_param_t param;
	decode_set_comm_param_req(&port_no,&param,&self->m_frame);
	if(port_no < 1 || port_no > MAX_COMM_COUNT){
		fill_error_response(&self->m_frame);
		return 0;
	}
	g_config.m_comm_param[port_no - 1] = param;
	save_comm_param(port_no);
	fill_ok_response(&self->m_frame);
	return 0;
}

int handle_get_comm_param(command_reactor_t* self)
{
	int port_no;

	decode_get_comm_param_req(&port_no,&self->m_frame);
	if(port_no < 1 || port_no > MAX_COMM_COUNT){
		fill_error_response(&self->m_frame);
	}
	encode_get_comm_param_res(&g_config.m_comm_param[port_no-1],&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}

int handle_get_machine_status(command_reactor_t* self)
{
	machine_status_t status;
	machine_status_t* s1;
	controller_status_t* s2;
	s1 = &status;
	s2 = &g_controller_status;

	// 从g_controller_status组装machine_status_t
	s1->m_is_degraded = s2->m_is_degraded;
	s1->m_second      = s2->m_second;
	s1->m_minute	  = s2->m_minute;
	s1->m_hour	  = s2->m_hour;
	s1->m_week   	  = s2->m_week;
	s1->m_day	  = s2->m_day;
	s1->m_month       = s2->m_month;
	s1->m_year[0]	  = s2->m_year[0];
	s1->m_year[1]     = s2->m_year[1];
	s1->m_designate_control_mode = s2->m_designate_control_mode;
	s1->m_actual_control_mode = s2->m_actual_control_mode;
	s1->m_schedule	  = 	s2->m_schedule;
	s1->m_timming 	  = 	s2->m_timming;
	s1->m_phase	  = 	s2->m_phase;
	s1->m_stage   =     s2->m_stage;
	s1->m_step	  = 	s2->m_step;
	s1->m_elapse_time 	= s2->m_elapse_time;
	s1->m_assign_time	= s2->m_assign_time;
	s1->m_remain_time	= s2->m_remain_time;
	s1->m_degraded_time = s2->m_degraded_time;
	s1->m_current_lamp_status = s2->m_current_lamp_status;
	get_detector_status(&s1->m_current_detector_status);
	get_detector_counter(&s1->m_current_detector_counter);

	get_detector_health(&s1->m_detector_health);
	get_lamp_health(&s1->m_lamp_health);

	s1->m_command 	= s2->m_online_command.m_cmd;
	s1->m_error = s2->m_error;

	s1->m_temperature_integer = s2->m_temperature/10;
	s1->m_temperature_decimal = s2->m_temperature%10;

	encode_get_machine_status_res(&status,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_set_multiple_schedule_control_param(command_reactor_t* self)
{
	multiple_schedule_control_param_t* param;
	param = &g_config.m_multiple_schedule_control_param;
	decode_set_multiple_schedule_control_param_req(param,&self->m_frame);
	if(param->m_schedule_plan>MAX_SCHEDULE_PLAN_COUNT||param->m_schedule_plan<=0)
		param->m_schedule_plan=1;
	save_multiple_schedule_control_param();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_multiple_schedule_control_param(command_reactor_t* self)
{
	encode_get_multiple_schedule_control_param_res(&g_config.m_multiple_schedule_control_param,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}

// 步进
int handle_do_step_forward(command_reactor_t* self)
{
	if(g_controller_status.m_bNoRevCommand==0)
	{
		g_controller_status.m_online_command.m_cmd= STAGE_FORWARD;
		fill_ok_response(&self->m_frame);
	}
	else
		fill_error_response(&self->m_frame);
	return 0;
}
// 保持
int handle_do_step_keep(command_reactor_t* self)
{
	if(g_controller_status.m_bNoRevCommand==0)
	{
		g_controller_status.m_online_command.m_cmd= STAGE_KEEP;
		fill_ok_response(&self->m_frame);
	}
	else
		fill_error_response(&self->m_frame);
	return 0;
}
// 强制灯色
int handle_do_lamp_status(command_reactor_t* self)
{
	if(g_controller_status.m_bNoRevCommand==0)
	{
		g_controller_status.m_online_command.m_cmd= DIRECT_PHASE;
		decode_do_lamp_status_req(&g_controller_status.m_direct_phase,&self->m_frame);
		fill_ok_response(&self->m_frame);
	}
	else
		fill_error_response(&self->m_frame);
	return 0;
}
int handle_get_detector_counter(command_reactor_t* self)
{
	detector_counter_t counter;
	get_detector_counter(&counter);
	encode_get_detector_counter_res(&counter,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_get_detector_status(command_reactor_t* self)
{
	detector_status_t status;
	get_detector_status(&status);
	encode_get_detector_status_res(&status,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_get_detector_health(command_reactor_t* self)
{
	detector_health_t health;
	get_detector_health(&health);
	encode_get_detector_health_res(&health,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_get_lamp_health(command_reactor_t* self)
{
	lamp_health_t health;
	get_lamp_health(&health);
	encode_get_lamp_health_res(&health,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int get_volume_occupy_Info(volume_occupy_time_t *vo_time_p,volume_occupy_t *vo_p)
{
	int  i;
	for(i=g_volume_occupy.m_pointer;i>0;i--)
	{
		if(g_volume_occupy.vo[i].m_year[0]==vo_time_p->m_year[0]&&g_volume_occupy.vo[i].m_year[1]==vo_time_p->m_year[1]&&
			g_volume_occupy.vo[i].m_month==vo_time_p->m_month&&g_volume_occupy.vo[i].m_day==vo_time_p->m_day&&
			g_volume_occupy.vo[i].m_hour==vo_time_p->m_hour&&g_volume_occupy.vo[i].m_minute==vo_time_p->m_minute)
		{
			memcpy(vo_p,&g_volume_occupy.vo[i],sizeof(volume_occupy_t));
			return 0;
		}
	}
	for(i=g_volume_occupy.m_pointer;i<MAX_VOLUME_OCCCUPY_NUM;i++)
	{
		if(g_volume_occupy.vo[i].m_year[0]==vo_time_p->m_year[0]&&g_volume_occupy.vo[i].m_year[1]==vo_time_p->m_year[1]&&
			g_volume_occupy.vo[i].m_month==vo_time_p->m_month&&g_volume_occupy.vo[i].m_day==vo_time_p->m_day&&
			g_volume_occupy.vo[i].m_hour==vo_time_p->m_hour&&g_volume_occupy.vo[i].m_minute==vo_time_p->m_minute)
		{
			memcpy(vo_p,&g_volume_occupy.vo[i],sizeof(volume_occupy_t));
			return 0;
		}
	}
	memset(vo_p,0,sizeof(volume_occupy_t));
	vo_p->m_year[0]=vo_time_p->m_year[0];
	vo_p->m_year[1]=vo_time_p->m_year[1];
	vo_p->m_month=vo_time_p->m_month;
	vo_p->m_day=vo_time_p->m_day;
	vo_p->m_hour=vo_time_p->m_hour;
	vo_p->m_minute=vo_time_p->m_minute;
	return 0;
}
int handle_get_volume_occupy(command_reactor_t* self)
{
	volume_occupy_time_t vo_time;
	volume_occupy_t      vo;
	decode_get_volume_occupy_res(&vo_time,&self->m_frame);
	if(get_volume_occupy_Info(&vo_time,&vo)<0)
	{
		fill_error_response(&self->m_frame);
		return 0;
	}
	encode_get_volume_occupy_req(&vo,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}

int handle_set_actuate_control_param(command_reactor_t* self)
{
	decode_set_actuate_control_param_req(&g_config.m_actuate_control_param,&self->m_frame);
	save_actuate_control_param();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_actuate_control_param(command_reactor_t* self)
{
	encode_get_actuate_control_param_res(&g_config.m_actuate_control_param,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}

int handle_set_cordless_coordinate_control_param(command_reactor_t* self)
{
	decode_set_cordless_coordinate_control_param_req(&g_config.m_cordless_coordinate_control_param,&self->m_frame);
	if (g_config.m_cordless_coordinate_control_param.m_phase>MAX_PHASE_PLAN_COUNT||g_config.m_cordless_coordinate_control_param.m_phase<=0) {
		g_config.m_cordless_coordinate_control_param.m_phase=1;
	}
	if (g_config.m_cordless_coordinate_control_param.m_timming>MAX_TIMMING_PLAN_COUNT||g_config.m_cordless_coordinate_control_param.m_timming<=0) {
		g_config.m_cordless_coordinate_control_param.m_timming=1;
	}
	save_cordless_coordinate_control_param();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_cordless_coordinate_control_param(command_reactor_t* self)
{
	encode_get_cordless_coordinate_control_param_res(&g_config.m_cordless_coordinate_control_param,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_mcu_send_key(command_reactor_t* self)
{
	// 如果缓冲区满了,丢弃掉最早的缓冲区
	if(is_buffer_full(&self->m_key_buffer))
		get_buffer_key(&self->m_key_buffer);

	put_buffer_key(&self->m_key_buffer,self->m_frame.m_data_p->m_data[0]);

	fill_ok_response(&self->m_frame);

	return 0;
}

int handle_get_key_val(command_reactor_t* self)
{
	if (key_count!=0x00) {
		self->m_frame.m_data_p->m_data[0] = key_buf[key_count-1];
		key_count--;
		}
	else self->m_frame.m_data_p->m_data[0] = 0x00;
	frame_set_data_len(&self->m_frame,1);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;	
}

int handle_get_green_conflict(command_reactor_t* self)
{
	memcpy(self->m_frame.m_data_p->m_data,&g_config.m_green_conflict,sizeof(green_conflict_t));
	frame_set_data_len(&self->m_frame,sizeof(green_conflict_t));
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_set_green_conflict(command_reactor_t* self)
{
	memcpy(&g_config.m_green_conflict,self->m_frame.m_data_p->m_data,sizeof(green_conflict_t));
	save_green_conflict();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_send_online_command(command_reactor_t* self)
{
	if(g_controller_status.m_bNoRevCommand==0)//允许接收命令
	{
		memcpy(&g_controller_status.m_online_command,self->m_frame.m_data_p->m_data,sizeof(online_control_command_t));
		if(g_controller_status.m_online_command.m_cmd==ONLINE)//要求联机
		{
			//设置当前的运行模式为执行机模式
			g_controller_status.m_designate_control_mode=EXECUTIVE_MACHINE_CONTROL;
			g_controller_status.m_is_degraded=0;
			g_controller_status.m_degraded_time=0;
		}
	}
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_setup_control_mode(command_reactor_t* self)
{
	encode_get_current_control_mode_res(&g_config.m_control_mode_selection,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_set_setup_control_mode(command_reactor_t* self)
{
	control_mode_selection_t control_mode_selection;
	decode_set_current_control_mode_req(&control_mode_selection,&self->m_frame);
	if(control_mode_selection.m_control_mode>9||control_mode_selection.m_control_mode<1)
	{
		fill_error_response(&self->m_frame);
		return 0;
	}
	g_config.m_control_mode_selection=control_mode_selection;
	save_control_mode_selection();
	fill_ok_response(&self->m_frame);
	return 0;
}
int handle_get_version(command_reactor_t* self)
{
	self->m_frame.m_data_p->m_data[0]=2;
	self->m_frame.m_data_len=1;
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_get_current_control_mode(command_reactor_t* self)
{
	control_mode_selection_t control_mode_selection;
	control_mode_selection.m_control_mode=g_controller_status.m_actual_control_mode;
	encode_get_current_control_mode_res(&control_mode_selection,&self->m_frame);
	self->m_frame.m_command = get_response_cmd(self->m_frame.m_command);
	return 0;
}
int handle_set_current_control_mode(command_reactor_t* self)
{
	control_mode_selection_t control_mode_selection;
	memcpy(&control_mode_selection,self->m_frame.m_data_p->m_data,sizeof(control_mode_selection_t));
	if(control_mode_selection.m_control_mode>9||control_mode_selection.m_control_mode<1)
	{
		fill_error_response(&self->m_frame);
		return 0;
	}
	if(g_controller_status.m_designate_control_mode!=control_mode_selection.m_control_mode)
	{
		g_controller_status.m_designate_control_mode=control_mode_selection.m_control_mode;
		g_controller_status.m_is_degraded=0;
		g_controller_status.m_degraded_time=0;
//		set_front_panel_control_mode(g_controller_status.m_designate_control_mode);
	}
	fill_ok_response(&self->m_frame);
	return 0;
}
