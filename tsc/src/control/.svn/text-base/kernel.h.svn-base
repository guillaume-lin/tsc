/**
 * kernel.h
 *
 * 包含程序主控制循环，共享的数据结构
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-06-25 10:11:20 中国标准时间
 *
 * $log$
 *
 */ 
#ifndef INCLUDED_KERNEL_H
#define INCLUDED_KERNEL_H
#include "datastruct.h"
#include "detector_set.h"
#ifdef __cplusplus__
extern "C" {
#endif
//////////////////////////////////////////////////////////////////////////////////////
// 确定当前时间应该走的相位方案和配时方案
int get_current_schedule(byte* schedule,byte* phase,byte* timming);
// 确定几秒后应该走的相位方案和配时方案
int get_next_schedule(byte* schedule,byte* phase,byte* timming,byte second);
// 载入指定的配时和相位
int load_current_schedule(byte schedule,byte phase, byte timming);
// 判断指定相位方案，配时方案是否合法
int is_schedule_valid(byte phase,byte timming);
//转到特定的相位和配时（先判断相位，配时的合理性，再引导该配时）
//int switching_with_schedule(byte schedule,byte phase, byte timming);
//引导特定的模式参数（全红，黄闪和灭灯）
int init_special_schedule_data(unsigned char ctemp);
//判断检测器的状态
int  is_detect_state(detector_set_t *pset);
//判断信号灯状态
int  is_lamp_state();
//判断无线协调时间段是否已经到达
int  is_cordless_coordinate_time();
// 判断当前步是否是最后一步
int is_last_step();
//判断当前步是否是最后阶段的第2步，在最后阶段的第2步，要获取下个时间要走的相位方案和配时方案
int is_last_stage_first_step();
// 判断当前步是否已经走完
int is_step_end();
// 判断是否一个周期运行结束
int is_cycle_end();
//设置程序的初始状态，先黄闪3秒，在全红3秒
void set_init_state();
//查找当前灯组在下一个阶段中是否亮绿灯
int find_lamp_group(int lamp_group);
//设置相位时间
void set_phase_time();
//设置新的相位
int set_phase_state(lamp_status_t* phase);
// 根据全局状态中的步号, 输出当前步
int run_current_step();
//设置当前的阶段数和对应的相位数
int  set_current_stage();
// 根据当前时段的配时，运行完当前周期
int switching_with_multiple_schedule(int mode_switch, int timer);
// 采用先黄闪后四面红的方式进行过渡
int switching_with_yellow_flash_all_red(int mode_switch, int yellow_time, int red_time, int timer);
//判断灯组的设置状态，是否要工作在长红，长绿，长黄状态哦 
int judge_lamp_group_work_mode(int nRed,int nYellow,int nGreen);
//获取信号机的车流量和占有率
int get_volume_occupy(detector_counter_t  *pdetector_volume,int  detector_occupy[]);
///////////////////////////////////////////////////////////////////////////////////////


/**
 * 控制子程序的原型
 * @param mode_switch  0 表示并非刚刚切换到此控制模式，非0表示刚刚切换到此控制模式
 * @param timer  取值0-9表示第几个100ms
 *
 */
typedef int (*control_mode_t)(int mode_switch,int timer) ;

/**
 * 控制程序主循环
 */
int controller_main();

///////////////////////////////////////////////////////////////////
// 系统配置参数
extern system_config_t g_config;

extern volume_occupy_group_t g_volume_occupy;

//0921 extern phase_plan_t g_yellow_flash_phase_plan;
//0921 extern phase_plan_t g_all_red_phase_plan;
//0921 extern phase_plan_t g_all_off_phase_plan;
extern stage_plan_t g_special_phase_plan;
extern lamp_status_t g_special_phase;
extern timming_plan_t g_special_phase_timming_plan;


// 系统状态
extern controller_status_t g_controller_status;

#ifdef __cplusplus__
};
#endif
#endif
