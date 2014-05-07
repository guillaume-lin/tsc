/**
 * codec.c
 * 
 * ������ʵ��
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-07 16:31:35 �й���׼ʱ��
 * 
 * $log$
 *
 */
#include <string.h>
#include "io.h"
#include "codec.h"
#include "util.h"
#include "cpld_fifo.h"

/**
 *  ��ʼ��֡
 */
int init_frame(frame_t* frame,frame_data_t* datum)
{
	frame->m_header   = 0xAA;
	frame->m_command  = 0x00;
	frame->m_reserve  = 0x00;
	frame->m_data_len = 0x00;
	frame->m_data_p   = datum;
	frame->m_checksum = 0x00;
	return 0;
}
int frame_set_data_len(frame_t* self,int len)
{
	if(len > 255){
		self->m_reserve |= 0x80;
		self->m_data_len = len - 255;
	}
	else{
		self->m_reserve &= ~0x80;
		self->m_data_len = len;
	}
	return 0;
}
int frame_get_data_len(frame_t* self)
{
	if(self->m_reserve & 0x80)
		return self->m_data_len + 255;
	else
		return self->m_data_len;
}
// ����֡��У���
byte compute_checksum(frame_t* frame)
{
	int data_len;
	int i=0;
	byte csum = 0;
	csum += frame->m_header;
	csum += frame->m_command;
	csum += frame->m_reserve;
	csum += frame->m_data_len;
	data_len = frame_get_data_len(frame);
	if(data_len > 0 )
		for(i=0; i<data_len && i<MAX_FRAME_DATA_SIZE; i++){
			csum += frame->m_data_p->m_data[i];
		}
	return csum;
}
byte compute_buffer_checksum(byte* buf,byte len)
{
	int i=0;
	byte csum = 0;
	for(i=0; i<len; i++)
		csum += buf[i];
	return csum;
}
// �ж�ָ��������ID�Ƿ�ָʾ����Ӧ��
int is_cmd_response_ok(byte cmd,byte response)
{
	if(cmd + 0x80 == response)
		return 1;
	else
		return 0;
}
/**
 * @desc ����ָ���������Ӧ������ID
 * @param [in]
 * @param [out]
 * @param [inout]
 * @return  Ӧ�������ID
 */
byte get_response_cmd(byte cmd)
{
	return cmd + 0x80;
}

/**
 * @param fd  Ŀ��˿�������
 * @param frame  Ҫд���֡
 * @return ����д��֡�ĳ���
 */ 
int write_frame(int fd,frame_t* frame)
{
	byte buffer[MAX_FRAME_DATA_SIZE*2+5];
	byte header = 0xaa;
	int count = 0;
	int i=0;
	int data_len;
	frame->m_checksum = compute_checksum(frame);

	buffer[count++] = frame->m_header;
	buffer[count++] = frame->m_command;
	buffer[count++] = frame->m_reserve;
	buffer[count++] = frame->m_data_len;
	data_len = frame_get_data_len(frame);
	for(i=0; i<data_len && i<MAX_FRAME_DATA_SIZE; i++){
		buffer[count++] = frame->m_data_p->m_data[i];
		//20090625 ȥ���������������aa����2��aa�����
		// ���������е�һ��0xaa������0xaa��ʾ
//		if(frame->m_data_p->m_data[i] == 0xaa){
//			buffer[count++] = header;
//		}
	}
	buffer[count++] = frame->m_checksum;

	return write_comm(fd,buffer,count);
}

/**
 * read as much as possible from fd
 * and buffer it to internal fifo
 */
int read_byte(int fd, unsigned char* buf, int len)
{
	static cpld_fifo_t fifo;
	static int init = 0;
	unsigned char tmp_buf[255];
	int ret,count;
	if(!init){
		init = 1;
		cpld_fifo_init(&fifo);
	}
	count = cpld_fifo_capacity(&fifo);
	if(count > 255)
		count = 255;
	if(count > 0){
		ret = read_comm(fd,tmp_buf,count);
		if(ret > 0){
			count = cpld_fifo_write(&fifo,tmp_buf,ret);
			// assert (count == ret)
		}
	}
	return cpld_fifo_read(&fifo,buf,len);
}
/**
 * @param fd �ļ�������,
 * @param frame ��Ŷ�ȡ����֡ , ���frame = NULL, ��ʾ��ն�������
 * @return > 0 ��ʾ����������һ֡, == 0 ��ʾû�ж���������֡, < 0 ��ʾ����
 */ 
int read_frame(int fd,frame_t* frame)
{
	static byte header;
	static byte command;
	static byte reserve;
	static byte data_len_byte;
	static int  data_len;
	static byte datum[MAX_FRAME_DATA_SIZE];
	static int data_to_read;
	static byte is_aa_read = 0;
	static byte pos = 0;
	byte csum;
	byte sum;
	int ret;
	

	if(frame == 0){
		// ��λ
		pos = 0;
		// ��ն�������
		while(read_byte(fd,datum,250) > 0);
		return -1;
	}

	switch(pos){
		case 0:		// ��ȡ֡ͷ
			header    = 0x00;
			command   = 0x00;
			reserve   = 0x00;
			data_len_byte  = 0x00;
			data_to_read = 0x00;
			is_aa_read = 0x00;
			csum      = 0x00;
			sum 	   = 0x00;
			do{
				ret = read_byte(fd,&header,1);
				if(ret != 1)
					return ret;
				if(header == 0xaa)
					break;
			}while(header != 0xaa);

			pos = 1;
		case 1:		// ��ȡcommand
			ret = read_byte(fd,&command,1);
			if(ret != 1)
				return ret;
			//090625 zjl Ҫ��ȥ��2��aa�����ļ�ͷ�������
			//if(command == 0xaa){
			//	// ������������aa,���ǺϷ���֡��ͷ
			//	pos = 0;
			//	return -1;
			//}
			pos = 2;
		case 2:		// ��ȡreserve
			ret = read_byte(fd,&reserve,1);
			if(ret != 1)
				return ret;
			pos = 3;
		case 3:		// ��ȡdata_len
			ret = read_byte(fd,&data_len_byte,1);
			if(ret != 1)
				return ret;
			pos = 4;
			is_aa_read = 0;		// ��λ��־
			data_to_read = 0;		// ׼����ȡ����
			data_len = data_len_byte;
			if(reserve & 0x80)
				data_len += 255;
		case 4:		// ��ȡ����
			while(data_to_read < data_len){
				ret = read_byte(fd,datum+data_to_read,1);
				if(ret != 1){
					return ret;
				}
				data_to_read++;
				//20090625 ���������е�aa��2��aa����ʾ
				//if(datum[data_to_read] != 0xaa){
				//	if(is_aa_read){
				//		// ����һ��0xaa,���ǽ�����ȴ����0xaa
				//		// ��ʽ����
				//		pos = 0;
				//		return -8;
				//	}
				//	data_to_read++;
				//} 
				//else {	// ��������һ��0xaa
				//	if(!is_aa_read){
				//		// ��ǰû����0xaa, �������
				//		is_aa_read = 1;
				//	}
				//	else {
				//		// ��ǰ�Ѿ�������һ��0xaa, �뻺����
				//		data_to_read++;
				//		is_aa_read = 0;
				//	}
				//}
					
			}
			pos = 5;
		case 5:		// ��ȡУ���
			sum = 0;
			csum = 0;
			ret = read_byte(fd,&csum,1);
			if(ret != 1)
				return ret;
			sum += compute_buffer_checksum(datum,data_len);
			sum += header+command+reserve+data_len_byte;

			pos = 0;

			if(csum != sum){
				// У���
				return -1;
			}
			

			// ����������һ֡
			frame->m_header = header;
			frame->m_command  = command;
			frame->m_reserve = reserve;
			frame->m_data_len = data_len_byte;
			memcpy(frame->m_data_p->m_data,datum,data_len);
			frame->m_checksum = csum;
			return data_len + 5;
		default:
			pos = 0;
			break;
	};

	return -11;
}

// ���뺯��
// ϵͳʱ��
	int encode_set_system_time_req(date_time_t* dt, frame_t* frame)
	{
		memcpy(frame->m_data_p->m_data,dt,sizeof(date_time_t));
		frame_set_data_len(frame,sizeof(date_time_t));
		return 0;
	}
int decode_set_system_time_req(date_time_t*dt, frame_t* frame)
{
	memcpy(dt,frame->m_data_p->m_data,sizeof(date_time_t));
	return 0;
}
int encode_get_system_time_res(date_time_t* dt, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,dt,sizeof(date_time_t));
	frame_set_data_len(frame,sizeof(date_time_t));
	return 0;
}
int decode_get_system_time_res(date_time_t* dt, frame_t* frame)
{
	memcpy(dt,frame->m_data_p->m_data,sizeof(date_time_t));
	return 0;
}
//��ת��Ϣ
int encode_set_checkpack_param_req(checkpack_t* sp, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,sp,sizeof(checkpack_t));
	frame_set_data_len(frame, sizeof(checkpack_t));
	return 0;
}
//��������
int encode_send_online_command_req(online_control_command_t *sp,frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,sp,sizeof(online_control_command_t));
	frame_set_data_len(frame, sizeof(online_control_command_t));
	return 0;
}
// ϵͳ����
int encode_set_system_param_req(system_param_t* sp, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,sp,sizeof(system_param_t));
	frame_set_data_len(frame, sizeof(system_param_t));
	return 0;
}
int decode_set_system_param_req(system_param_t* sp, frame_t * frame)
{
	memcpy(sp,frame->m_data_p->m_data,sizeof(system_param_t));
	return 0;
}
int encode_get_system_param_res(system_param_t* sp, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,sp,sizeof(system_param_t));
	frame_set_data_len(frame,sizeof(system_param_t));
	return 0;

}
	int decode_get_system_param_res(system_param_t* sp, frame_t* frame)
	{
		memcpy(sp,frame->m_data_p->m_data,sizeof(system_param_t));
		return 0;
	}
//�źŵ�����
int encode_set_lamp_setting_req(lamp_setting_t* ls, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,ls,sizeof(lamp_setting_t));
	frame_set_data_len(frame, sizeof(lamp_setting_t));
	return 0;
}
int decode_set_lamp_setting_req(lamp_setting_t* ls, frame_t* frame)
{
	memcpy(ls,frame->m_data_p->m_data,sizeof(lamp_setting_t));
	return 0;
}
// ��������
int encode_set_lamp_group_setting_req(lamp_group_setting_t* lgs, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,lgs,sizeof(lamp_group_setting_t));
	frame_set_data_len(frame, sizeof(lamp_group_setting_t));
	return 0;

}
int decode_set_lamp_group_setting_req(lamp_group_setting_t* lgs, frame_t* frame)
{
	memcpy(lgs,frame->m_data_p->m_data,sizeof(lamp_group_setting_t));
	return 0;
}
// ���������
int encode_set_detector_setting_req(detector_setting_t* ds, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,ds,sizeof(detector_setting_t));
	frame_set_data_len(frame, sizeof(detector_setting_t));
	return 0;
}
int decode_set_detector_setting_req(detector_setting_t* ds, frame_t* frame)
{
	memcpy(ds,frame->m_data_p->m_data,sizeof(detector_setting_t));
	return 0;
}
int encode_get_detector_setting_res(detector_setting_t* ds, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,ds,sizeof(detector_setting_t));
	frame_set_data_len(frame,sizeof(detector_setting_t));
	return 0;
}
	int decode_get_detector_setting_res(detector_setting_t* ds, frame_t* frame)
	{
		memcpy(ds,frame->m_data_p->m_data,sizeof(detector_setting_t));
		return 0;
	}
// ��λ����
	int encode_set_phase_plan_req(int planNo,int stepCount,int step, stage_t* phase,frame_t* frame)
	{
		frame->m_data_p->m_data[0] = planNo;
		frame->m_data_p->m_data[1] = stepCount;
		frame->m_data_p->m_data[2] = step;
		memcpy(frame->m_data_p->m_data+3,phase,sizeof(stage_t));
		frame_set_data_len(frame,sizeof(stage_t) + 3);
		return 0;
	}
int decode_set_phase_plan_req(int* planNo,int* stepCount,int* step, stage_t* phase,frame_t* frame)
{
	*planNo = frame->m_data_p->m_data[0];
	*stepCount = frame->m_data_p->m_data[1];
	*step = frame->m_data_p->m_data[2];
	memcpy(phase,frame->m_data_p->m_data+3,sizeof(stage_t));
	return 0;
}
	int encode_get_phase_plan_req(int plan_no, int step, frame_t* frame)
	{
		frame->m_data_p->m_data[0] = plan_no;
		frame->m_data_p->m_data[1] = step;
		frame_set_data_len(frame,2);
		return 0;
	}
int decode_get_phase_plan_req(int* plan_no, int* step, frame_t* frame)
{
	*plan_no = frame->m_data_p->m_data[0];
	*step = frame->m_data_p->m_data[1];
	return 0;
}
int encode_get_phase_plan_res(int step_count, stage_t* phase, frame_t* frame)
{
	frame->m_data_p->m_data[0] = step_count;
	memcpy(frame->m_data_p->m_data+1,phase,sizeof(stage_t));
	frame_set_data_len(frame,sizeof(stage_t) + 1);
	return 0;
}
int decode_get_phase_plan_res(int* step_count, stage_t* phase, frame_t* frame)
{
	*step_count = frame->m_data_p->m_data[0];
	memcpy(phase,frame->m_data_p->m_data+1,sizeof(stage_t));
	return 0;
}

// ��ʱ����
int encode_set_timming_plan_req(int plan_no, timming_plan_t* p, frame_t* frame)
{
	frame->m_data_p->m_data[0] = plan_no;
	memcpy(frame->m_data_p->m_data+1,p,sizeof(timming_plan_t));
	frame_set_data_len(frame,sizeof(timming_plan_t) + 1);
	return 0;
}
int decode_set_timming_plan_req(int* plan_no, timming_plan_t* p, frame_t* frame)
{
	*plan_no = frame->m_data_p->m_data[0];
	memcpy(p,frame->m_data_p->m_data+1,sizeof(timming_plan_t));
	return 0;
}	
int encode_get_timming_plan_res(timming_plan_t* plan, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,plan,sizeof(timming_plan_t));
	frame_set_data_len(frame,sizeof(timming_plan_t));
	return 0;
}
	int decode_get_timming_plan_res(timming_plan_t* plan, frame_t* frame)
	{
		memcpy(plan,frame->m_data_p->m_data,sizeof(timming_plan_t));
		return 0;
	}
	int encode_get_timming_plan_req(int plan_no,frame_t* frame)
	{
		frame->m_data_p->m_data[0] = plan_no;
		frame_set_data_len(frame,1);
		return 0;
	}
int decode_get_timming_plan_req(int* plan_no, frame_t* frame)
{
	*plan_no = frame->m_data_p->m_data[0];
	return 0;
}
// ʱ�η���
	int encode_set_schedule_plan_req(int planNo, schedule_plan_t* p, frame_t* frame)
	{
		frame->m_data_p->m_data[0] = planNo;
		memcpy(frame->m_data_p->m_data+1,p,sizeof(schedule_plan_t));
		frame_set_data_len(frame,sizeof(schedule_plan_t) + 1);
		return 0;
	}
int decode_set_schedule_plan_req(int* planNo, schedule_plan_t* p, frame_t* frame)
{
	*planNo = frame->m_data_p->m_data[0];
	memcpy(p,frame->m_data_p->m_data+1,sizeof(schedule_plan_t));
	return 0;
}
	int encode_get_schedule_plan_req(int planNo, frame_t* frame)
	{
		frame->m_data_p->m_data[0] = planNo;
		frame_set_data_len(frame,1);
		return 0;
	}
int decode_get_schedule_plan_req(int* planNo, frame_t* frame)
{
	*planNo = frame->m_data_p->m_data[0];
	return 0;
}
int encode_get_schedule_plan_res(schedule_plan_t* schedule, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,schedule,sizeof(schedule_plan_t));
	frame_set_data_len(frame,sizeof(schedule_plan_t));
	return 0;
}
	int decode_get_schedule_plan_res(schedule_plan_t* schedule, frame_t* frame)
	{
		memcpy(schedule,frame->m_data_p->m_data,sizeof(schedule_plan_t));
		return 0;
	}

// ������
	int encode_set_special_day_table_req(special_day_table_t* day,frame_t* frame)
	{
		memcpy(frame->m_data_p->m_data,day,sizeof(special_day_table_t));
		frame_set_data_len(frame,sizeof(special_day_table_t));
		return 0;
	}
int decode_set_special_day_table_req(special_day_table_t* day, frame_t* frame)
{
	memcpy(day,frame->m_data_p->m_data,sizeof(special_day_table_t));
	return 0;
}
// ͨ�Ų���
	int encode_set_comm_param_req(int portNo,comm_param_t* param, frame_t* frame)
	{
		frame->m_data_p->m_data[0] = portNo;
		memcpy(frame->m_data_p->m_data+1,param,sizeof(comm_param_t));
		frame_set_data_len(frame,sizeof(comm_param_t) + 1);
		return 0;
	}
int decode_set_comm_param_req(int* portNo,comm_param_t* param, frame_t* frame)
{
	*portNo = frame->m_data_p->m_data[0];
	memcpy(param,frame->m_data_p->m_data+1,sizeof(comm_param_t));
	return 0;
}
	int encode_set_multiple_schedule_control_param_req(multiple_schedule_control_param_t* control, frame_t* frame)
	{
		memcpy(frame->m_data_p->m_data,control,sizeof(multiple_schedule_control_param_t));
		frame_set_data_len(frame,sizeof(multiple_schedule_control_param_t));
		return 0;
	}
int decode_set_multiple_schedule_control_param_req(multiple_schedule_control_param_t* control, frame_t* frame)
{
	memcpy(control,frame->m_data_p->m_data,sizeof(multiple_schedule_control_param_t));
	return 0;
}

int encode_set_cordless_coordinate_control_param_req(cordless_coordinate_control_param_t* control, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,control,sizeof(cordless_coordinate_control_param_t));
	frame_set_data_len(frame,sizeof(cordless_coordinate_control_param_t));
	return 0;
}
int decode_set_cordless_coordinate_control_param_req(cordless_coordinate_control_param_t* control, frame_t* frame)
{
	memcpy(control,frame->m_data_p->m_data,sizeof(cordless_coordinate_control_param_t));
	return 0;
}
int encode_set_current_control_mode_req(control_mode_selection_t* selection,frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,selection,sizeof(control_mode_selection_t));
	frame_set_data_len(frame,sizeof(control_mode_selection_t));
	return 0;
}
int decode_set_current_control_mode_req(control_mode_selection_t* selection,frame_t* frame)
{
	memcpy(selection,frame->m_data_p->m_data,sizeof(control_mode_selection_t));
	return 0;
}
// ���뺯��
// ϵͳʱ��
	int decode_system_time(date_time_t* dt, frame_t* frame)
	{
		memcpy(dt,frame->m_data_p->m_data,sizeof(date_time_t));
		return 0;
	}
// ϵͳ����
	int decode_system_param(system_param_t* sp, frame_t* frame)
	{
		memcpy(sp,frame->m_data_p->m_data,sizeof(system_param_t));
		return 0;
	}

int encode_get_lamp_setting_res(lamp_setting_t* ls, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,ls,sizeof(lamp_setting_t));
	frame_set_data_len(frame,sizeof(lamp_setting_t));
	return 0;
}
int decode_get_lamp_setting_res(lamp_setting_t* ls, frame_t* frame)
{
	memcpy(ls,frame->m_data_p->m_data,sizeof(lamp_setting_t));
	return 0;
}
int encode_get_lamp_group_setting_res(lamp_group_setting_t* lgs, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,lgs,sizeof(lamp_group_setting_t));
	frame_set_data_len(frame,sizeof(lamp_group_setting_t));
	return 0;
}
int decode_get_lamp_group_setting_res(lamp_group_setting_t* lgs, frame_t* frame)
{
	memcpy(lgs,frame->m_data_p->m_data,sizeof(lamp_group_setting_t));
	return 0;
}

int encode_get_special_day_table_res(special_day_table_t* day, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,day,sizeof(special_day_table_t));
	frame_set_data_len(frame,sizeof(special_day_table_t));
	return 0;
}
	int decode_get_special_day_table_res(special_day_table_t* day, frame_t* frame)
	{
		memcpy(day,frame->m_data_p->m_data,sizeof(special_day_table_t));
		return 0;
	}

	int encode_get_comm_param_req(int port_no, frame_t* frame)
	{
		frame->m_data_p->m_data[0] = port_no;
		frame_set_data_len(frame,1);
		return 0;
	}
int decode_get_comm_param_req(int* port_no, frame_t* frame)
{
	*port_no = frame->m_data_p->m_data[0];

	return 0;
}
int encode_get_comm_param_res(comm_param_t* param, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,param,sizeof(comm_param_t));
	frame_set_data_len(frame,sizeof(comm_param_t));
	return 0;
}
	int decode_get_comm_param_res(comm_param_t* param, frame_t* frame)
	{
		memcpy(param,frame->m_data_p->m_data,sizeof(comm_param_t));
		return 0;
	}
int encode_get_multiple_schedule_control_param_res(multiple_schedule_control_param_t*
						   param, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,param,sizeof(multiple_schedule_control_param_t));
	frame_set_data_len(frame,sizeof(multiple_schedule_control_param_t));
	return 0;
}
	int decode_get_multiple_schedule_control_param_res(multiple_schedule_control_param_t*
						             param, frame_t* frame)
	{
		memcpy(param,frame->m_data_p->m_data,sizeof(multiple_schedule_control_param_t));
		return 0;
	}
int encode_get_actuate_control_param_res(actuate_control_param_t* param, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,param,sizeof(actuate_control_param_t));
	frame_set_data_len(frame,sizeof(actuate_control_param_t));
	return 0;

}
	int decode_get_actuate_control_param_res(actuate_control_param_t* param, frame_t* frame)
	{
		memcpy(param,frame->m_data_p->m_data,sizeof(actuate_control_param_t));
		return 0;
	}
	int encode_set_actuate_control_param_req(actuate_control_param_t* param, frame_t* frame)
	{
		memcpy(frame->m_data_p->m_data,param,sizeof(actuate_control_param_t));
		frame_set_data_len(frame,sizeof(actuate_control_param_t));
		return 0;
	
	}
int decode_set_actuate_control_param_req(actuate_control_param_t* param, frame_t* frame)
{
	memcpy(param,frame->m_data_p->m_data,sizeof(actuate_control_param_t));
	return 0;
}
int encode_get_cordless_coordinate_control_param_res(cordless_coordinate_control_param_t* 							param, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,param,sizeof(cordless_coordinate_control_param_t));
	frame_set_data_len(frame,sizeof(cordless_coordinate_control_param_t));
	return 0;

}
int decode_get_cordless_coordinate_control_param_res(cordless_coordinate_control_param_t*
						      param, frame_t* frame)
{
	memcpy(param,frame->m_data_p->m_data,sizeof(cordless_coordinate_control_param_t));
	return 0;
}

int encode_get_current_control_mode_res(control_mode_selection_t* param,frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,param,sizeof(control_mode_selection_t));
	frame_set_data_len(frame,sizeof(control_mode_selection_t));
	return 0;

}
	int decode_get_current_control_mode_res(control_mode_selection_t* param,frame_t* frame)
	{
		memcpy(param,frame->m_data_p->m_data,sizeof(control_mode_selection_t));
		return 0;
	}

// �źŻ�״̬
int encode_get_machine_status_res(machine_status_t* status, frame_t* frame)
{
	int size=sizeof(machine_status_t);
	memcpy(frame->m_data_p->m_data,status,size);
	frame_set_data_len(frame,sizeof(machine_status_t));
	return 0;
}

	int decode_get_machine_status_res(machine_status_t* status, frame_t* frame)
	{
		memcpy(status,frame->m_data_p->m_data,sizeof(machine_status_t));
		return 0;
	}

	int encode_do_lamp_status_req(lamp_status_t* phase,frame_t* frame)
	{
		memcpy(frame->m_data_p->m_data,phase,sizeof(lamp_status_t));
		frame_set_data_len(frame,sizeof(lamp_status_t));
		return 0;
	}
int decode_do_lamp_status_req(lamp_status_t* phase,frame_t* frame)
{
	memcpy(phase,frame->m_data_p->m_data,sizeof(lamp_status_t));
	return 0;
}

int encode_get_detector_counter_res(detector_counter_t* counter, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,counter,sizeof(detector_counter_t));
	frame_set_data_len(frame,sizeof(detector_counter_t));
	return 0;
}
	int decode_get_detector_counter_res(detector_counter_t* counter, frame_t* frame)
	{
		memcpy(counter,frame->m_data_p->m_data,sizeof(detector_counter_t));
		return 0;
	}
int encode_get_detector_status_res(detector_status_t* status,frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,status,sizeof(detector_status_t));
	frame_set_data_len(frame,sizeof(detector_status_t));

	return 0;
}
	int decode_get_detector_status_res(detector_status_t* status,frame_t* frame)
	{
		memcpy(status,frame->m_data_p->m_data,sizeof(detector_status_t));
	
		return 0;
	}

int encode_get_lamp_health_res(lamp_health_t* health, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,health,sizeof(lamp_health_t));
	frame_set_data_len(frame,sizeof(lamp_health_t));

	return 0;
}
int decode_get_lamp_health_res(lamp_health_t* health, frame_t* frame)
{
	memcpy(health,frame->m_data_p->m_data,sizeof(lamp_health_t));
	
	return 0;
}

int encode_get_detector_health_res(detector_health_t* health, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,health,sizeof(detector_health_t));
	frame_set_data_len(frame,sizeof(detector_health_t));

	return 0;
}
int decode_get_detector_health_res(detector_health_t* health, frame_t* frame)
{
	memcpy(health,frame->m_data_p->m_data,sizeof(detector_health_t));
	return 0;
}
int encode_get_volume_occupy_res(volume_occupy_time_t* oc, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,oc,sizeof(volume_occupy_time_t));
	frame_set_data_len(frame,sizeof(volume_occupy_time_t));

	return 0;
}
int encode_get_volume_occupy_req(volume_occupy_t* oc, frame_t* frame)
{
	memcpy(frame->m_data_p->m_data,oc,sizeof(volume_occupy_t));
	frame_set_data_len(frame,sizeof(volume_occupy_t));
	return 0;
}
int decode_get_volume_occupy_req(volume_occupy_t* oc, frame_t* frame)
{
	memcpy(oc,frame->m_data_p->m_data,sizeof(volume_occupy_t));
	return 0;
}

int decode_get_volume_occupy_res(volume_occupy_time_t* oc, frame_t* frame)
{
	memcpy(oc,frame->m_data_p->m_data,sizeof(volume_occupy_time_t));
	return 0;
}
