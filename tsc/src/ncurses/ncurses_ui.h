#ifndef INCLUDED_NCURSES_UI_H
#define INCLUDED_NCURSES_UI_H
#include <cursesm.h>
#include <cursesf.h>
#include <stdlib.h>
#include "../control/datastruct.h"
#include "label.hpp"
#include "../control/vm.h"
#include "../control/command_reactor.h"
#include "key_translate.h"

extern vm_t g_vm;
extern command_reactor_t g_reactor;

static const char* itoa(int i)
{
        static char s[20];
        sprintf(s,"%d",i);
        return s;
}

class CFormStatus : public NCursesForm 
{
public:
		CFormStatus()
			:NCursesForm(14,60,(lines()-14)/2,(cols()-60)/2)
		{
			m_counter = 0;
			
			F = new NCursesFormField*[25];		
			F[0] = new CLabel("设置的控制模式:"		,0,0); F[10] = new NCursesFormField(1,3,0,22);
			F[1] = new CLabel("实际运行的控制模式:"	        ,1,0); F[11] = new NCursesFormField(1,3,1,22);
			F[2] = new CLabel("当前时段:"			,2,0); F[12] = new NCursesFormField(1,3,2,22);
			F[3] = new CLabel("当前配时:"			,3,0); F[13] = new NCursesFormField(1,3,3,22);
			F[4] = new CLabel("当前相位:"			,4,0); F[14] = new NCursesFormField(1,3,4,22);
			F[5] = new CLabel("当前步号:"			,5,0); F[15] = new NCursesFormField(1,3,5,22);
			F[6] = new CLabel("当前步设定时间:"		,6,0); F[16] = new NCursesFormField(1,3,6,22);
			F[7] = new CLabel("当前步剩余时间:"		,7,0); F[17] = new NCursesFormField(1,3,7,22);
			F[8] = new CLabel("当前步已运行时间:"          ,8,0);
			F[18] = new NCursesFormField(1,2,8,22);
			F[9] = new CLabel("检测器状态:"			,9,0); F[19] = new NCursesFormField(1,3,9,22);
			F[20] = new CLabel("系统错误号:"			,10,0); 
			F[21] = new NCursesFormField(1,3,10,22);
			F[22] = new CLabel("当前时间;"                  ,11,0);
			F[23] = new NCursesFormField(1,25,11,22);
			F[24] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
		//	boldframe();	// 加框修饰窗体
			halfdelay(2);	// 按键超时设为0.2秒	
		}

		int m_counter;
		/**
		 *  处理按键
		 */
		int virtualize(int c)
		{
			int key;
			int ret;

			ret = NCursesForm::virtualize(c);
			m_counter++;
			if(m_counter > 5)
				m_counter = 0;
			// 刷新屏幕
			if(m_counter == 0 || m_counter == 2)
				SetValue();

			// 接收按键
			reactor_handle_command(&g_reactor);
			// 转换按键
			c = translate_key(get_buffer_key(&g_reactor.m_key_buffer));
			if(c == ERR)
				return 0;
			else
				return ret ;
		}
		~CFormStatus()
		{
		}
		void GetValue()
		{
			m_Status.m_designate_control_mode	=(byte)atoi(F[10]->value());	

			m_Status.m_actual_control_mode		=(byte)atoi(F[11]->value());	

			m_Status.m_schedule			=(byte)atoi(F[12]->value());

			m_Status.m_timming			=(byte)atoi(F[13]->value());

			m_Status.m_phase			=(byte)atoi(F[14]->value());			

			m_Status.m_step				=(byte)atoi(F[15]->value());		

			m_Status.m_elapse_time			=(byte)atoi(F[16]->value());		

			m_Status.m_assign_time			=(byte)atoi(F[17]->value());	

			m_Status.m_remain_time			=(byte)atoi(F[18]->value());	

		}
		void SetValue()
		{
			vm_get_machine_status(&g_vm, &m_Status);
			// 设定的控制模式
			F[10]->set_value(itoa(m_Status.m_designate_control_mode	));
			//实际运行的控制模式
			F[11]->set_value(itoa(m_Status.m_actual_control_mode	));
			// 时段方案
			F[12]->set_value(itoa(m_Status.m_schedule));
			// 配时方案
			F[13]->set_value(itoa(m_Status.m_timming));
			// 相位方案
			F[14]->set_value(itoa(m_Status.m_phase));
			// 步伐号
			F[15]->set_value(itoa(m_Status.m_step));
			// 设定的步伐时间
			F[16]->set_value(itoa(m_Status.m_assign_time));
			// 剩余的步伐时间
			F[17]->set_value(itoa(m_Status.m_remain_time));
			// 当前步已运行时间
			F[18]->set_value(itoa(m_Status.m_elapse_time));
			//F[19]->set_value(itoa(m_Status.m_detector));
			// 错误代码
			// F[21]->set_value(m_Status.m_error[4]);	
			char buf[25];
			sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d %02d",
				m_Status.m_year[0]*100+m_Status.m_year[1],
				m_Status.m_month,
				m_Status.m_day,
				m_Status.m_hour,
				m_Status.m_minute,
				m_Status.m_second,
				m_Status.m_week);

			F[23]->set_value(buf);
		}

		NCursesFormField** F;
		machine_status_t m_Status;
};


class CFormControlMode : public NCursesForm 
{
public:
		int m_counter;
		CFormControlMode()
			:NCursesForm(5,20,(lines()-5)/2,(cols()-20)/2)
		{
			F = new NCursesFormField*[3];		
			F[0] = new CLabel("当前控制模式:"		,0,0); 
			F[1] = new NCursesFormField(1,2,0,19);
			F[2] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();
			halfdelay(2);
			m_counter = 0;
		}

		~CFormControlMode()
		{
		}
		int virtualize(int c)
		{
			int key;
			int ret;

			if(c == CTRL('W')){
				GetValue();
				return 0;
			}else if(c == CTRL('R')){
				SetValue();
				return 0;
			}
			ret = NCursesForm::virtualize(c);		
			m_counter++;
			if(m_counter >= 5)
				m_counter = 0;

			// 接收按键
			reactor_handle_command(&g_reactor);
			// 转换按键
			c = translate_key(get_buffer_key(&g_reactor.m_key_buffer));
			if(c == ERR)
				return 0;
			else
				return ret;
		}
		void GetValue()
		{
			control_mode.m_control_mode =(byte)atoi(F[1]->value());
			vm_set_current_control_mode(&g_vm,&control_mode);
		}
		void SetValue()
		{
			vm_get_current_control_mode(&g_vm,&control_mode);
			F[1]->set_value(itoa(control_mode.m_control_mode));
		}
		control_mode_selection_t control_mode;
		NCursesFormField** F;
};


class CFormCommParam: public NCursesForm 
{
public:
		CFormCommParam()
			:NCursesForm(10,40,(lines()-12)/2,(cols()-42)/2)
		{
			F = new NCursesFormField*[11];		
			F[0] = new CLabel("通信端口:"		,0,0); 
			F[5] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("端口类型:"		,1,0); 
			F[6] = new NCursesFormField(1,2,1,22);
			F[2] = new CLabel("IP地址:"			,2,0); 
			F[7] = new NCursesFormField(1,2,2,22);
			F[3] = new CLabel("波特率:"			,3,0); 
			F[8] = new NCursesFormField(1,2,3,22);
			F[4] = new CLabel("通信协议:"		,4,0); 
			F[9] = new NCursesFormField(1,2,4,22);
			F[10] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		
		}
		~CFormCommParam()
		{
		}
		void GetValue()
		{
//				m_comm_param.m_name		=atoi(F[5]->value());

				m_comm_param.m_type		=atoi(F[6]->value());

				m_comm_param.m_baud		=atoi(F[7]->value());

				m_comm_param.m_ip[4]		=atoi(F[8]->value());

				m_comm_param.m_protocol	=  	atoi(F[9]->value());
		}
		void SetValue()
		{
				// 端口名称
//				F[5]->set_value(itoa(m_comm_param.m_name	));
				// 端口类型
				F[6]->set_value(itoa(m_comm_param.m_type	));
				// 波特率
				F[7]->set_value(itoa(m_comm_param.m_baud	));
				// IP地址
				F[8]->set_value(itoa(m_comm_param.m_ip[4]		));
				// 通信协议
				F[9]->set_value(itoa(m_comm_param.m_protocol));
			
		}
		comm_param_t m_comm_param;
		NCursesFormField** F;
};

class CFormSetDate: public NCursesForm 
{
public:
		CFormSetDate()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[15];		
			F[0] = new CLabel("年:"		,0,0); F[7] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("月:"		,1,0); F[8] = new NCursesFormField(1,2,1,22);
			F[2] = new CLabel("日:"		,2,0); F[9] = new NCursesFormField(1,2,2,22);
			F[3] = new CLabel("时:"		,3,0); F[10] = new NCursesFormField(1,2,3,22);
			F[4] = new CLabel("分:"		,4,0); F[11] = new NCursesFormField(1,2,4,22);
			F[5] = new CLabel("秒:"		,5,0); F[12] = new NCursesFormField(1,2,5,22);
			F[6] = new CLabel("周:"		,6,0); F[13] = new NCursesFormField(1,2,6,22);
			F[14] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		
		}
		~CFormSetDate()
		{
		}
		void GetValue()
		{
				m_date.m_year[2]	=atoi(F[7]->value());			// 20,05 第一个字节存放世纪，第二个字节存放年份
				m_date.m_month		=atoi(F[8]->value());			// 1 - 12
				m_date.m_day		=atoi(F[9]->value());				// 1 - 31
				m_date.m_hour		=atoi(F[10]->value());			// 0 - 23
				m_date.m_minute		=atoi(F[11]->value());			// 0 - 59
				m_date.m_second		=atoi(F[12]->value());			// 0 - 59
				m_date.m_weekday	=atoi(F[13]->value());			// 0 - 6, 0 代表星期天

		}
		void SetValue()
		{
			F[7]->set_value ( itoa(m_date.m_year[2]	));
			F[8]->set_value ( itoa(m_date.m_month	));
			F[9]->set_value ( itoa(m_date.m_day		));
			F[10]->set_value( itoa(m_date.m_hour	));
			F[11]->set_value( itoa(m_date.m_minute	));
			F[12]->set_value( itoa(m_date.m_second	));
			F[13]->set_value( itoa(m_date.m_weekday	));
		}
		date_time_t m_date;
		NCursesFormField** F;
};


class CFormCordless: public NCursesForm 
{
public:
		CFormCordless()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[7];		
			F[0] = new CLabel("相位差:"		,0,0); F[3] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("相位方案:"	,1,0); F[4] = new NCursesFormField(1,2,1,22);
			F[2] = new CLabel("配时方案:"	,2,0); F[5] = new NCursesFormField(1,2,2,22);
			F[6] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		
		}
		~CFormCordless()
		{
		}
		void GetValue()
		{
		}
		void SetValue()
		{
		}
		NCursesFormField** F;
};


class CFormActuate: public NCursesForm 
{
public:
		CFormActuate()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[15];		
			F[0] = new CLabel("车辆检测方式:"		,0,0); 
			F[1] = new CLabel("最小绿:"				,1,0); 
			F[4] = new NCursesFormField(1,2,0,22);	
			F[8] = new CLabel("步长调整量:"				,0,33); 
			F[11] = new NCursesFormField(1,2,0,55);
			F[5] = new NCursesFormField(1,2,1,22);	
			F[9] = new CLabel("车头时距:"				,1,33); 
			F[12] = new NCursesFormField(1,2,1,55);
			F[2] = new CLabel("最大绿:"				,2,0); 
			F[6] = new NCursesFormField(1,2,2,22);	
			F[10] = new CLabel("相位方案:"				,2,33); 
			F[13] = new NCursesFormField(1,2,2,55);
			F[3] = new CLabel("配时方案:"			,3,0); 
			F[7] = new NCursesFormField(1,2,3,22);
			F[14] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		
		}
		~CFormActuate()
		{
		}
		void GetValue()
		{
		// 车辆检测方法(视频或者线圈)
		m_actuate.m_detect_method		=atoi(F[4]->value());
		// 最小绿
		m_actuate.m_minmum_green		=atoi(F[5]->value());
		// 最大绿
		m_actuate.m_maximum_green		=atoi(F[6]->value());
		// 每次的调整量
		m_actuate.m_adjust_quantum		=atoi(F[7]->value());
		// 车头时距
		m_actuate.m_vehicle_gap			=atoi(F[11]->value());
		// 使用的相位
		m_actuate.m_phase				=atoi(F[12]->value());
		// 使用的配时
		m_actuate.m_timming				=atoi(F[13]->value());



		}
		void SetValue()
		{
		                   // 车辆检测方法(视频或者线圈)   
		F[4 ]->set_value(itoa(m_actuate.m_detect_method		));
		// 最小绿
		F[5 ]->set_value(itoa(m_actuate.m_minmum_green			));
		// 最大绿
		F[6 ]->set_value(itoa(m_actuate.m_maximum_green		));
		// 每次的调整量
		F[7 ]->set_value(itoa(m_actuate.m_adjust_quantum		));
		// 车头时距
		F[11]->set_value(itoa(m_actuate.m_vehicle_gap			));
		// 使用的相位
		F[12]->set_value(itoa(m_actuate.m_phase				));
		// 使用的配时
		F[13]->set_value(itoa(m_actuate.m_timming				));
		}
		actuate_control_param_t m_actuate;
		NCursesFormField** F;
};

class CFormMulti: public NCursesForm 
{
public:
		CFormMulti()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[5];		
			F[0] = new CLabel("启用特殊控制:"		,0,0); F[2] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("使用时段方案号:"		,1,0); F[3] = new NCursesFormField(1,2,1,22);
			F[4] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		
		}
		~CFormMulti()
		{
		}
		void GetValue()
		{
				m_Multi.m_enable_special	= atoi(F[2]->value());
				m_Multi.m_schedule_plan		= atoi(F[3]->value());;
		}
		void SetValue()
		{
			F[2]->set_value(itoa(m_Multi.m_enable_special));
			F[3]->set_value(itoa(m_Multi.m_schedule_plan	));
		}
		multiple_schedule_control_param_t m_Multi;
		NCursesFormField** F;
};



class CFormSpecialDay: public NCursesForm 
{
public:
		CFormSpecialDay()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[13];	
			F[0] = new CLabel("特殊日期号:"			,0,0); F[6] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("特殊日期编号:"		,1,0); F[7] = new NCursesFormField(1,2,1,22);
			F[2] = new CLabel("起始月:"				,2,0); F[8] = new NCursesFormField(1,2,2,22);
			F[3] = new CLabel("起始日:"				,3,0); F[9] = new NCursesFormField(1,2,3,22);
			F[4] = new CLabel("结束月:"				,4,0); F[10] = new NCursesFormField(1,2,4,22);
			F[5] = new CLabel("结束日:"				,5,0); F[11] = new NCursesFormField(1,2,5,22);
			F[12] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();
		
		
		}
		~CFormSpecialDay()
		{
		}
		void GetValue()
		{

			// 起始月
			m_date.m_start_month	= atoi(F[8]->value());
			// 起始日
			m_date.m_start_day		= atoi(F[9]->value());
			// 结束月
			m_date.m_end_month		= atoi(F[10]->value());
			// 结束日
			m_date.m_end_day		= atoi(F[11]->value());


		}
		void SetValue()
		{
								  // 起始月 	
			F[8]->set_value (itoa(m_date.m_start_month));
			// 起始日
			F[9]->set_value (itoa(m_date.m_start_day	));
			// 结束月
			F[10]->set_value(itoa(m_date.m_end_month	));
			// 结束日
			F[11]->set_value(itoa(m_date.m_end_day	));

		}
		special_day_t m_date;
		NCursesFormField** F;
};



class CFormTimeSchedule: public NCursesForm 
{
public:
		CFormTimeSchedule()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[11];	
			F[0] = new CLabel("时段方案号:"			,0,0); 
			F[5] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("起始时:"				,2,0); 
			F[6] = new NCursesFormField(1,2,2,22);
			F[2] = new CLabel("起始分:"				,3,0); 
			F[7] = new NCursesFormField(1,2,3,22);
			F[3] = new CLabel("相位方案号:"			,4,0); 
			F[8] = new NCursesFormField(1,2,4,22);
			F[4] = new CLabel("配时方案号:"			,5,0); 
			F[9] = new NCursesFormField(1,2,5,22);
			
			F[10] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		}
		~CFormTimeSchedule()
		{
		}
		void GetValue()
		{
			m_schedule.m_time_segment_count							= atoi( F[5]->value());
			m_schedule.m_hour[m_schedule.m_time_segment_count]		= atoi( F[6]->value());
			m_schedule.m_minute[m_schedule.m_time_segment_count]	= atoi( F[7]->value());
			m_schedule.m_phase[m_schedule.m_time_segment_count]		= atoi( F[8]->value());
			m_schedule.m_timming[m_schedule.m_time_segment_count]	= atoi( F[9]->value());
 		}
		void SetValue()
		{
			F[5]->set_value(itoa(m_schedule.m_time_segment_count						));
			F[6]->set_value(itoa(m_schedule.m_hour[m_schedule.m_time_segment_count]		));
			F[7]->set_value(itoa(m_schedule.m_minute[m_schedule.m_time_segment_count]	));
			F[8]->set_value(itoa(m_schedule.m_phase[m_schedule.m_time_segment_count]	));
			F[9]->set_value(itoa(m_schedule.m_timming[m_schedule.m_time_segment_count]	));
		}
	     
		schedule_plan_t m_schedule;
		NCursesFormField** F;
};


class CFormTimming: public NCursesForm 
{
public:
		CFormTimming()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[25];	
			F[0] = new CLabel("时段方案:"			,0,0); 
			F[2] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("步伐数:"			    ,1,33); 
			F[3] = new NCursesFormField(1,2,1,55);
			for ( int i =1;i<=10 ;i++)
			{
				char s[3];
				memset (s,0x00,3);
				F[3+i]=new CLabel(s, 2,(i-1)*7);
				F[3+10+i]=new NCursesFormField(1,2,2,(i-1)*7);
			}
			F[24] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		}
		~CFormTimming()
		{
		}
		void GetValue()
		{
		
			m_Plan.m_step = atoi(F[3]->value());
			for ( int i=1;i<=10;i++)
			{
				m_Plan.m_timming[i-1]= atoi(F[3+10+i]->value());
			}
		}

		void SetValue()
		{ 
		
			F[3]->set_value(itoa(m_Plan.m_step));
			for ( int i=1;i<=10;i++)
			{
				F[3+10+i]->set_value(itoa(m_Plan.m_timming[i]));
			}

		}
		timming_plan_t m_Plan;
		NCursesFormField** F;
};


class CFormSafe: public NCursesForm 
{
public:
		CFormSafe()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[30];	
			F[0] = new CLabel("相位方案号:"			,0,0); 
			F[2] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("步伐数:"			    ,1,33); 
			F[3] = new NCursesFormField(1,2,1,55);
			F[4] = new CLabel("最短时间:"			,2,33);
			for ( int i =1;i<=12 ;i++)
			{
				char s[3];
				memset (s,0x00,3);
				F[4+i]=new CLabel(s, 3,(i-1)*7);
				F[4+12+i]=new NCursesFormField(1,2,3,(i-1)*7);
			}
			F[29] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		}
		~CFormSafe()
		{
		}
		void GetValue()
		{
		}
		void SetValue()
		{
		}
		NCursesFormField** F;
};


class CFormPhase: public NCursesForm 
{
public:
		CFormPhase()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[61];	
			F[0] = new CLabel("相位方案:"			,0,0); F[6] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("第"			    ,1,10); F[7] = new NCursesFormField(1,2,1,15);
			F[3] = new CLabel("步"			    ,1,22);
			F[4] = new CLabel("总共:"			,2,0);	F[8] = new NCursesFormField(1,2,1,15);
			F[5] = new CLabel("步"			    ,1,22);

			F[9]   = new CLabel("灯组"			    ,3,0);
			F[10]   = new CLabel("红灯"			    ,4,0);
			F[11]   = new CLabel("黄灯"			    ,5,0);
			F[12]   = new CLabel("绿灯"			    ,5,0);

			for ( int i =1;i<=12 ;i++)
			{
				char s[3];
				memset (s,0x00,3);
				F[13+i]=new CLabel(s, 3,(i-1)*7);
				F[13+12+i]=new NCursesFormField(1,2,4,(i-1)*7);
				F[13+12+i]=new NCursesFormField(1,2,5,(i-1)*7);
			}
			F[60] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		}
		~CFormPhase()
		{
		}
		void GetValue()
		{
			m_phase.m_step=atoi(F[7]->value());
			//for ( int i =0;i<
			//m_phase.m_phase[m_phase.m_step];

		}
		void SetValue()
		{
		} 
		phase_plan_t m_phase;
		NCursesFormField** F;
};


class CFormDetector: public NCursesForm 
{
public:
		CFormDetector()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[55];	
			F[0] = new CLabel("检测器总数:"			,0,0); 
			F[6] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("上下游检测器距离"	,1,10); 
			F[7] = new NCursesFormField(1,2,1,15);
			F[3] = new CLabel("米"					,1,22);
			F[4] = new CLabel("编号"				,2,0);	
			F[5] = new CLabel("灯组编号"		    ,2,15);

			F[6]   = new CLabel("上下游"			    ,2,30);
			F[7]   = new CLabel("检测器接口编号"			    ,2,45);
			F[8]   = new CLabel("属性"			    ,2,60);
			
			for ( int i =1;i<=9 ;i++)
			{
				char s[3];
				memset (s,0x00,3);
				sprintf(s,"%d",i);
				F[8+(i-1)*5] = new CLabel(s, 2+i, 0);
				for ( int j=0;j<4;j++)
					 F[8+(i-1)*5+j] = new NCursesFormField(1,2,2+i,15*j);

			}
			F[54] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		}
		~CFormDetector()
		{
		}
		void GetValue()
		{
		}
		void SetValue()
		{
		}
		NCursesFormField** F;
};


class CFormLampGroup: public NCursesForm 
{
public:
		CFormLampGroup()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[16];	
			F[0] = new CLabel("第:"			,0,0); F[10] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("组"			,0,33); 
			F[3] = new CLabel("总共："		,0,44);F[11] = new NCursesFormField(1,2,0,55);
			F[4] = new CLabel("组"			,0,66);	
			F[5] = new CLabel("灯色"		,2,15);

			F[6]   = new CLabel("红"			    ,2,30);
			F[7]   = new CLabel("黄"			    ,2,45);
			F[8]   = new CLabel("绿"			    ,2,60);
			F[9]   = new 	CLabel("接线端子"		,3,0);

			F[12] = new NCursesFormField(1,2,3,30);
			F[13] = new NCursesFormField(1,2,3,45);
			F[14] = new NCursesFormField(1,2,3,60);
			F[15] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		
		}
		~CFormLampGroup()
		{
		}
		void GetValue()
		{
		}
		void SetValue()
		{
		}
		NCursesFormField** F;
};

class CFormBasic: public NCursesForm 
{
public:
		CFormBasic()
			:NCursesForm(22,70,(lines()-24)/2,(cols()-72)/2)
		{
			F = new NCursesFormField*[7];	
			F[0] = new CLabel("灯组总数"			,0,0);  F[3] = new NCursesFormField(1,2,0,22);
			F[1] = new CLabel("检测器总数"			,1,0);	F[4] = new NCursesFormField(1,2,1,22);
			F[2] = new CLabel("四面红时间："		,2,0);	F[5] = new NCursesFormField(1,2,2,22);
			F[6] = new NCursesFormField();
			InitForm(F,TRUE,TRUE);
			boldframe();

		}
		~CFormBasic()
		{
		}
		void GetValue()
		{
		}
		void SetValue()
		{
		}
		NCursesFormField** F;
};

#endif
