// 定义系统参数在 EEPROM 的地址

// 时段方案地址（最多支持 20 个时段方案）
// 时段方案 0
#define schedule_plan_t_count0 			0x2000;					// 该时段方案有效时段的个数 1 个字节
#define schedule_plan_t_hour0 			0x2001;					// 该时段方案各时段的起始时间（小时）24 个字节（最大 24 个时段）
#define schedule_plan_t_minute0 		0x2019;					// 该时段方案各时段的起始时间（分钟）24 个字节（最大 24 个时段）
#define schedule_plan_t_phase0 			0x2031;					// 该时段方案各时段的要使用的相位方案号 24 个字节（最大 24 个时段）
#define schedule_plan_t_timming0 		0x2049;					// 该时段方案各时段的要使用的配时方案号 24 个字节（最大 24 个时段）
// 时段方案 1                                       		
#define schedule_plan_t_count0 			0x2061;					// 该时段方案有效时段的个数 1 个字节
#define schedule_plan_t_hour0 			0x2062;					// 该时段方案各时段的起始时间（小时）24 个字节（最大 24 个时段）
#define schedule_plan_t_minute0 		0x207A;					// 该时段方案各时段的起始时间（分钟）24 个字节（最大 24 个时段）
#define schedule_plan_t_phase0 			0x2092;					// 该时段方案各时段的要使用的相位方案号 24 个字节（最大 24 个时段）
#define schedule_plan_t_timming0 		0x20AC;					// 该时段方案各时段的要使用的配时方案号 24 个字节（最大 24 个时段）



// 时段方案 19
#define schedule_plan_t_count0 			0x2000+19*97;			// 该时段方案有效时段的个数 1 个字节
#define schedule_plan_t_hour0 			0x2018+19*97+1;			// 该时段方案各时段的起始时间（小时）24 个字节（最大 24 个时段）
#define schedule_plan_t_minute0 		0x2030+19*97+24;		// 该时段方案各时段的起始时间（分钟）24 个字节（最大 24 个时段）
#define schedule_plan_t_phase0 			0x2048+19*97+48;		// 该时段方案各时段的要使用的相位方案号 24 个字节（最大 24 个时段）
#define schedule_plan_t_timming0 		0x2060+19*97+72;		// 该时段方案各时段的要使用的配时方案号 24 个字节（最大 24 个时段）

// 配时方案地址（最多支持 32 个时段方案）
// 配时方案 0
#define timming_plan_t_step				
#define timming_plan_t_timming			