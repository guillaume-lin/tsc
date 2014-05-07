/**
 * vm.c
 * 
 * 虚拟机实现
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-07 17:00:01 中国标准时间
 * 
 * $log$
 *
 */
#include "vm.h"
#include "codec.h"
#include "io.h"

vm_t g_vm;

int vm_init(vm_t* self)
{
	self->m_fd = -1;
	self->m_try_count = 5;
	return init_frame(&self->m_frame,&self->m_frame_data);
}
/**
 * 设置mcu在接收到命令后是否响铃
 */
int vm_set_beep_option(vm_t* self,int flag)
{
	if(flag)
		self->m_frame.m_reserve |= 0x01;
	else
		self->m_frame.m_reserve &= ~0x01;
	return 0;
}
/**
 * 发送命令帧
 *
 * @return 0 表示成功，其它表示失败
 */
int send_command(vm_t* self,byte cmd, frame_t* frame)
{
	frame->m_command = cmd;
	read_frame(self->m_fd,0);//清空读缓冲区
	return write_frame(self->m_fd,frame) > 0 ? 0 : -1;
}

/**
 * 接收应答帧
 *
 * @return 0 表示成功，其它表示失败
 */
int recv_response(vm_t* self,byte cmd, frame_t* frame)
{
	int ret;
	int tries;
	tries = 0;
	ret = -1;

	do{
		ret  = read_frame(self->m_fd,frame);
		if(ret > 0 && is_cmd_response_ok(cmd,frame->m_command)) {
			// response is ok
			return 0;
			}

		if(ret == 0){
			// frame not complete, wait
			wait_for_100ms();
			}

		if(ret < 0){
			return -1;
		}
	}while(tries++ <= self->m_try_count);
	// timeout
	return -1;
}

/**
 *  处理命令的发送接收
 */
int do_command(vm_t* self, byte cmd, frame_t* frame)
{
	if(send_command(self,cmd,frame) != 0)
		return -1;
	if(recv_response(self,cmd,frame) != 0)
		return -1;
	return 0;
}
	// 系统时间
	
	int vm_set_system_time(vm_t* self, date_time_t* dt)
	{
		encode_set_system_time_req(dt,&self->m_frame);
	
		if(do_command(self,CMD_SET_SYSTEM_TIME,&self->m_frame) != 0)
			return -1;
		// decode response
		return 0;
	}
	int vm_get_system_time(vm_t* self,date_time_t* dt)
	{
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_SYSTEM_TIME,&self->m_frame) != 0)
			return -1;
	
		decode_get_system_time_res(dt,&self->m_frame);
		return 0;
	}
	
	// 设置系统参数
	int vm_set_system_param(vm_t* self,system_param_t* sp)
	{
		encode_set_system_param_req(sp,&self->m_frame);
	
		if(do_command(self,CMD_SET_SYSTEM_PARAM,&self->m_frame) != 0)
			return -1;
		return 0;
	}
	int vm_get_system_param(vm_t* self,system_param_t* sp)
	{
		frame_set_data_len(&self->m_frame,0);	
		if(do_command(self,CMD_GET_SYSTEM_PARAM,&self->m_frame) != 0)
			return -1;
	
		decode_get_system_param_res(sp,&self->m_frame);
		return 0;
	}
	//设置信号灯参数
	int vm_set_lamp_setting(vm_t* self,lamp_setting_t* ls)
	{

		encode_set_lamp_setting_req(ls,&self->m_frame);

		if(do_command(self,CMD_SET_LAMP_SETTING,&self->m_frame) != 0)
			return -1;
		return 0;
	}
	// 设置灯组参数
	int vm_set_lamp_group_setting(vm_t* self,lamp_group_setting_t* lgs)
	{
	
		encode_set_lamp_group_setting_req(lgs,&self->m_frame);
	
		if(do_command(self,CMD_SET_LAMP_GROUP_SETTING,&self->m_frame) != 0)
			return -1;
		return 0;
	}
	// 设置检测器参数
	int vm_set_detector_setting(vm_t* self,detector_setting_t* ds)
	{
	
		encode_set_detector_setting_req(ds,&self->m_frame);
	
		if(do_command(self,CMD_SET_DETECTOR_SETTING,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	// 设置相位方案
	int vm_set_phase_plan(vm_t* self,int planNo,int step,int step_count,stage_t* phase)
	{
	
		encode_set_phase_plan_req(planNo,step_count,step,phase,&self->m_frame);
	
		if(do_command(self,CMD_SET_PHASE_PLAN,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	// 设置配时方案
	int vm_set_timming_plan(vm_t* self,timming_plan_t* plan,int plan_no)
	{
	
		encode_set_timming_plan_req(plan_no,plan,&self->m_frame);
	
		if(do_command(self,CMD_SET_TIMMING_PLAN,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	// 设置时段方案
	int vm_set_schedule_plan(vm_t* self,schedule_plan_t* plan,int plan_no)
	{
	
		encode_set_schedule_plan_req(plan_no,plan,&self->m_frame);
	
		if(do_command(self,CMD_SET_SCHEDULE_PLAN,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	// 设置特殊日
	int vm_set_special_day_table(vm_t* self,special_day_table_t* day)
	{
	
		encode_set_special_day_table_req(day,&self->m_frame);
	
		if(do_command(self,CMD_SET_SPECIAL_DAY_TABLE,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	
	// 设置通信参数
	int vm_set_comm_param(vm_t* self,int port_no,comm_param_t* param)
	{
	
		encode_set_comm_param_req(port_no,param,&self->m_frame);
	
		if(do_command(self,CMD_SET_COMM_PARAM,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	// 设置多时段控制参数
	int vm_set_multiple_schedule_control_param(vm_t* self,multiple_schedule_control_param_t* param)
	{
		encode_set_multiple_schedule_control_param_req(param,&self->m_frame);
	
		if(do_command(self,CMD_SET_MULTIPLE_SCHEDULE_CONTROL_PARAM,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	// 设置感应控制参数
	int vm_set_actuate_control_param(vm_t* self,actuate_control_param_t* param)
	{
	
		encode_set_actuate_control_param_req(param,&self->m_frame);
	
		if(do_command(self,CMD_SET_ACTUATE_CONTROL_PARAM,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	// 设置无线缆协调控制参数
	int vm_set_cordless_coordinate_control_param(vm_t* self,cordless_coordinate_control_param_t* param)
	{
	
		encode_set_cordless_coordinate_control_param_req(param,&self->m_frame);
	
		if(do_command(self,CMD_SET_CORDLESS_COORDINATE_CONTROL_PARAM,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	// 设置当前运行的控制模式
	int vm_set_current_control_mode(vm_t* self,control_mode_selection_t* mode)
	{
	
		encode_set_current_control_mode_req(mode,&self->m_frame);
	
		if(do_command(self,CMD_SET_CURRENT_CONTROL_MODE,&self->m_frame) != 0)
			return -1;
	
		return 0;
	}
	//设置系统启动时的控制模式
	int vm_set_setup_control_mode(vm_t* self,control_mode_selection_t* mode)
	{

		encode_set_current_control_mode_req(mode,&self->m_frame);

		if(do_command(self,CMD_SET_SETUP_CONTROL_MODE,&self->m_frame) != 0)
			return -1;

		return 0;
	}	
	// 取信号灯参数
	int vm_get_lamp_setting(vm_t* self,lamp_setting_t* ls)
	{

		frame_set_data_len(&self->m_frame,0);	
		if(do_command(self,CMD_GET_LAMP_SETTING,&self->m_frame) != 0)
			return -1;

		decode_get_lamp_setting_res(ls,&self->m_frame);
		return 0;
	}
	// 取灯组参数
	int vm_get_lamp_group_setting(vm_t* self,lamp_group_setting_t* lgs)
	{

		frame_set_data_len(&self->m_frame,0);	
		if(do_command(self,CMD_GET_LAMP_GROUP_SETTING,&self->m_frame) != 0)
			return -1;
	
		decode_get_lamp_group_setting_res(lgs,&self->m_frame);
		return 0;
	}
	// 取检测器参数
	int vm_get_detector_setting(vm_t* self,detector_setting_t* ds)
	{

		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_DETECTOR_SETTING,&self->m_frame) != 0)
			return -1;
	
		decode_get_detector_setting_res(ds,&self->m_frame);
		return 0;
	}
	// 取相位方案
	int vm_get_phase_plan(vm_t* self,int plan_no,int step, int* step_count, stage_t* phase)
	{
	
		encode_get_phase_plan_req(plan_no,step,&self->m_frame);
		if(do_command(self,CMD_GET_PHASE_PLAN,&self->m_frame) != 0)
			return -1;
	
		decode_get_phase_plan_res(step_count,phase,&self->m_frame);
		return 0;
	}
	// 取配时方案
	int vm_get_timming_plan(vm_t* self,int plan_no, timming_plan_t* plan)
	{
	
		encode_get_timming_plan_req(plan_no,&self->m_frame);
	
		if(do_command(self,CMD_GET_TIMMING_PLAN,&self->m_frame) != 0)
			return -1;
	
		decode_get_timming_plan_res(plan,&self->m_frame);
		return 0;
	}
	// 取时段方案
	int vm_get_schedule_plan(vm_t* self,int plan_no, schedule_plan_t* plan)
	{
	
		encode_get_schedule_plan_req(plan_no,&self->m_frame);
		if(do_command(self,CMD_GET_SCHEDULE_PLAN,&self->m_frame) != 0)
			return -1;
	
		decode_get_schedule_plan_res(plan,&self->m_frame);
		return 0;
	}
	// 取特殊日
	int vm_get_special_day_table(vm_t* self, special_day_table_t* day)
	{

		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_SPECIAL_DAY_TABLE,&self->m_frame) != 0)
			return -1;
	
		decode_get_special_day_table_res(day,&self->m_frame);
		return 0;
	}
	// 取通信参数
	int vm_get_comm_param(vm_t* self,int port_no, comm_param_t* param)
	{
	
		encode_get_comm_param_req(port_no,&self->m_frame);
	
		if(do_command(self,CMD_GET_COMM_PARAM,&self->m_frame) != 0)
			return -1;
	
		decode_get_comm_param_res(param,&self->m_frame);
		return 0;
	}
	// 取多时段控制参数
	int vm_get_multiple_schedule_control_param(vm_t* self,multiple_schedule_control_param_t* param)
	{

		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_MULTIPLE_SCHEDULE_CONTROL_PARAM,&self->m_frame) != 0)
			return -1;
	
		decode_get_multiple_schedule_control_param_res(param,&self->m_frame);
		return 0;
	}
	// 取感应控制参数
	int vm_get_actuate_control_param(vm_t* self,actuate_control_param_t* param)
	{
	
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_ACTUATE_CONTROL_PARAM,&self->m_frame) != 0)
			return -1;
	
		decode_get_actuate_control_param_res(param,&self->m_frame);
		return 0;
	}
	// 取无线缆协调控制参数
	int vm_get_cordless_coordinate_control_param(vm_t* self,cordless_coordinate_control_param_t* param)
	{
	
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_CORDLESS_COORDINATE_CONTROL_PARAM,&self->m_frame) != 0)
			return -1;
	
		decode_get_cordless_coordinate_control_param_res(param,&self->m_frame);
		return 0;
	}
	// 取当前运行的控制模式
	int vm_get_current_control_mode(vm_t* self,control_mode_selection_t* mode)
	{
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_CURRENT_CONTROL_MODE,&self->m_frame) != 0)
			return -1;
	
		decode_get_current_control_mode_res(mode,&self->m_frame);
		return 0;
	}
	//取系统启动时的控制模式
	int vm_get_setup_control_mode(vm_t* self, control_mode_selection_t* mode)
	{
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_SETUP_CONTROL_MODE,&self->m_frame) != 0)
			return -1;

		decode_get_current_control_mode_res(mode,&self->m_frame);
		return 0;
	}
	// 取信号机的当前状态
	int vm_get_machine_status(vm_t* self, machine_status_t* status)
	{
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_MACHINE_STATUS,&self->m_frame) != 0)
			return -1;
		decode_get_machine_status_res(status,&self->m_frame);
		return 0;
	}
	
	// 保持
	int vm_do_step_keep(vm_t* self)
	{

		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_DO_STEP_KEEP,&self->m_frame) != 0)
			return -1;
		return 0;
	}
	// 步进
	int vm_do_step_forward(vm_t* self)
	{
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_DO_STEP_FORWARD,&self->m_frame) != 0)
			return -1;
		return 0;
	}
	
	// 强制灯色
	int vm_do_lamp_status(vm_t* self,lamp_status_t* phase)
	{
		encode_do_lamp_status_req(phase,&self->m_frame);
		if(do_command(self,CMD_DO_LAMP_STATUS,&self->m_frame) != 0)
			return -1;
		return 0;
	}
	// 检测器计数值
	int vm_get_detector_counter(vm_t* self, detector_counter_t* counter)
	{
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_DETECTOR_COUNTER,&self->m_frame) != 0)
			return -1;
		decode_get_detector_counter_res(counter,&self->m_frame);
		return 0;
	}
	// 检测器状态
	int vm_get_detector_status(vm_t* self, detector_status_t* status)
	{
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_DETECTOR_STATUS,&self->m_frame) != 0)
			return -1;
		decode_get_detector_status_res(status,&self->m_frame);
		return 0;
	}
	// 信号灯故障情况
	int vm_get_lamp_health(vm_t* self, lamp_health_t* health)
	{
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_LAMP_HEALTH,&self->m_frame) != 0)
			return -1;
		decode_get_lamp_health_res(health,&self->m_frame);
		return 0;
	}
	// 检测器故障情况
	int vm_get_detector_health(vm_t* self, detector_health_t* health)
	{
		frame_set_data_len(&self->m_frame,0);
		if(do_command(self,CMD_GET_DETECTOR_HEALTH,&self->m_frame) != 0)
			return -1;
		decode_get_detector_health_res(health,&self->m_frame);
		return 0;
	}
	
	// 取车流量和占有率
	int vm_get_volume_occupy(vm_t* self,volume_occupy_time_t vo_time, volume_occupy_t* vo)
	{
		encode_get_volume_occupy_res(&vo_time,&self->m_frame);
		if(do_command(self,CMD_GET_VOLUME_OCCUPY,&self->m_frame) != 0)
			return -1;
		decode_get_volume_occupy_req(vo,&self->m_frame);
		return 0;
	}
// 单片机给pc104发送按键
int vm_mcu_send_key(vm_t* self, byte key)
{
	self->m_frame.m_data_len = 1;
	self->m_frame.m_data_p->m_data[0] = key;

	if(do_command(self,CMD_MCU_SEND_KEY,&self->m_frame) != 0)
		return -1;
	else
		return 0;
}
//wint vm_get_key(vm_t* self, byte* key)
//w{
//w	static int i=1;
//w	if(i > 18)
//w		i = 1;
//w	*key = 0;//i++;
//w	return 0;
//w}
//w	int vm_mcu_ping(vm_t* self, byte echo)
//w	{
//w		self->m_frame.m_data_len = 1;
//w		self->m_frame.m_data_p->m_data[0] = echo;
//w		if(do_command(self,CMD_MCU_PING,&self->m_frame) != 0)
//w			return -1;
//w		else
//w			return 0;
//w	}


int vm_get_key_val(vm_t* self, byte* key_val)
{

	frame_set_data_len(&self->m_frame,0);
	if(do_command(self,CMD_GET_KEY_VAL,&self->m_frame) != 0) 
		return -1;
	*key_val = self->m_frame.m_data_p->m_data[0];

	return 0;						
}
int vm_get_version(vm_t *self,int *version)
{
	frame_set_data_len(&self->m_frame,0);
	if(do_command(self,CMD_GET_VERSION,&self->m_frame) != 0)
		return -1;
	*version=self->m_frame.m_data_p->m_data[0];
	return 0;
}
int vm_get_green_conflict(vm_t* self, green_conflict_t* conflict)
{
	frame_set_data_len(&self->m_frame,0);
	if(do_command(self,CMD_GET_GREEN_CONFLICT,&self->m_frame) != 0)
		return -1;
	memcpy(conflict,self->m_frame.m_data_p->m_data,sizeof(green_conflict_t));
	return 0;
}
int vm_set_green_conflict(vm_t* self, green_conflict_t* conflict)
{
	memcpy(self->m_frame.m_data_p->m_data,conflict,sizeof(green_conflict_t));
	frame_set_data_len(&self->m_frame,sizeof(green_conflict_t));
	if(do_command(self,CMD_SET_GREEN_CONFLICT,&self->m_frame) != 0)
		return -1;
	return 0;
}
//中转信息
int vm_set_checkpack_param(vm_t* self,checkpack_t* sp)
{

	encode_set_checkpack_param_req(sp,&self->m_frame);

	if(do_command(self,CMD_SET_VERIFY_USERINFO,&self->m_frame) != 0)
		return -1;
	return self->m_frame.m_data_p->m_data[0];
}
int vm_send_online_command(vm_t* self,online_control_command_t* sp)
{
	encode_send_online_command_req(sp,&self->m_frame);
	if(do_command(self,CMD_SEND_ONLINE_COMMAND,&self->m_frame) != 0)
		return -1;
	return 0;
}
