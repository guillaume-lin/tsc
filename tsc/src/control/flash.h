// ����ϵͳ������ EEPROM �ĵ�ַ

// ʱ�η�����ַ�����֧�� 20 ��ʱ�η�����
// ʱ�η��� 0
#define schedule_plan_t_count0 			0x2000;					// ��ʱ�η�����Чʱ�εĸ��� 1 ���ֽ�
#define schedule_plan_t_hour0 			0x2001;					// ��ʱ�η�����ʱ�ε���ʼʱ�䣨Сʱ��24 ���ֽڣ���� 24 ��ʱ�Σ�
#define schedule_plan_t_minute0 		0x2019;					// ��ʱ�η�����ʱ�ε���ʼʱ�䣨���ӣ�24 ���ֽڣ���� 24 ��ʱ�Σ�
#define schedule_plan_t_phase0 			0x2031;					// ��ʱ�η�����ʱ�ε�Ҫʹ�õ���λ������ 24 ���ֽڣ���� 24 ��ʱ�Σ�
#define schedule_plan_t_timming0 		0x2049;					// ��ʱ�η�����ʱ�ε�Ҫʹ�õ���ʱ������ 24 ���ֽڣ���� 24 ��ʱ�Σ�
// ʱ�η��� 1                                       		
#define schedule_plan_t_count0 			0x2061;					// ��ʱ�η�����Чʱ�εĸ��� 1 ���ֽ�
#define schedule_plan_t_hour0 			0x2062;					// ��ʱ�η�����ʱ�ε���ʼʱ�䣨Сʱ��24 ���ֽڣ���� 24 ��ʱ�Σ�
#define schedule_plan_t_minute0 		0x207A;					// ��ʱ�η�����ʱ�ε���ʼʱ�䣨���ӣ�24 ���ֽڣ���� 24 ��ʱ�Σ�
#define schedule_plan_t_phase0 			0x2092;					// ��ʱ�η�����ʱ�ε�Ҫʹ�õ���λ������ 24 ���ֽڣ���� 24 ��ʱ�Σ�
#define schedule_plan_t_timming0 		0x20AC;					// ��ʱ�η�����ʱ�ε�Ҫʹ�õ���ʱ������ 24 ���ֽڣ���� 24 ��ʱ�Σ�



// ʱ�η��� 19
#define schedule_plan_t_count0 			0x2000+19*97;			// ��ʱ�η�����Чʱ�εĸ��� 1 ���ֽ�
#define schedule_plan_t_hour0 			0x2018+19*97+1;			// ��ʱ�η�����ʱ�ε���ʼʱ�䣨Сʱ��24 ���ֽڣ���� 24 ��ʱ�Σ�
#define schedule_plan_t_minute0 		0x2030+19*97+24;		// ��ʱ�η�����ʱ�ε���ʼʱ�䣨���ӣ�24 ���ֽڣ���� 24 ��ʱ�Σ�
#define schedule_plan_t_phase0 			0x2048+19*97+48;		// ��ʱ�η�����ʱ�ε�Ҫʹ�õ���λ������ 24 ���ֽڣ���� 24 ��ʱ�Σ�
#define schedule_plan_t_timming0 		0x2060+19*97+72;		// ��ʱ�η�����ʱ�ε�Ҫʹ�õ���ʱ������ 24 ���ֽڣ���� 24 ��ʱ�Σ�

// ��ʱ������ַ�����֧�� 32 ��ʱ�η�����
// ��ʱ���� 0
#define timming_plan_t_step				
#define timming_plan_t_timming			