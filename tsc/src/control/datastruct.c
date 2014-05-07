#include "datastruct.h"

/**
 *  判断设置的灯组数
 */
int lamp_group_setting_get_lamp_group_count(lamp_group_setting_t* self)
{
	int i;
	for(i=0; i<MAX_LAMP_GROUP_COUNT; i++){
		if(self->m_lamp_group[i][RED] == 0 && 
				self->m_lamp_group[i][GREEN] == 0 &&
				self->m_lamp_group[i][YELLOW] == 0){
			break;
		}
	}
	return i;
}
/**
 * 判断指定的两个端子是否冲突
 */
int green_conflict_is_conflict(green_conflict_t* self,int n, int m)
{
	int b,off;
	byte mask;
	int tmp;

	if(n < 1 || n > MAX_LAMP_GROUP_COUNT || 
	   m < 1 || m > MAX_LAMP_GROUP_COUNT)
		return 0;

	if(n > m ){
		tmp = n;
		n = m;
		m = tmp;
	}

	if(n == m)
		return 0;

	// assert(n<m);
	n -= 1;
	m -= 1;
	tmp = m*(m-1)/2+n;
	b = tmp/8;
	off = 7 - tmp%8;
	mask = 0x01 << off;
	if(self->m_conflict[b] & mask )
		return 1;
	else
		return 0;
}

int green_conflict_set_conflict(green_conflict_t* self,int n, int m, int conflict)
{
	int b,off;
	byte mask;
	int tmp;

	if(n < 1 || n > MAX_LAMP_GROUP_COUNT || 
	   m < 1 || m > MAX_LAMP_GROUP_COUNT)
		return -1;

	if(n > m ){
		tmp = n;
		n = m;
		m = tmp;
	}

	if(n == m)
		return 0;

	// assert(n<m);
	n -= 1;
	m -= 1;
	tmp = m*(m-1)/2+n;
	b = tmp/8;
	off = 7 - tmp%8;
	mask = 0x01 << off;
	
	if(conflict)
		self->m_conflict[b] |=  mask;
	else
		self->m_conflict[b] &= ~mask;

	return 0;

}
int lamp_group_setting_get_terminal(lamp_group_setting_t* self,int lamp_no)
{
	int group;
	int lamp;
	if(lamp_no < 1 || lamp_no > MAX_LAMP_GROUP_COUNT*3)
		return -1;
	group = (lamp_no - 1 ) / 3;
	lamp =  (lamp_no - 1) % 3;
	if(lamp == 0)
		return self->m_lamp_group[group][RED];
	else if(lamp == 1)
		return self->m_lamp_group[group][YELLOW];
	else
		return self->m_lamp_group[group][GREEN];
	
}
int phase_set_output(lamp_status_t* self, int n, int v)
{
	int b,off;
	unsigned char mask, c;

	//assert(n > 0 && n <= MAX_TERMINAL_COUNT)
	//assert(v >= 0 && v <= 2)
	n -= 1;
	b = n/4;
	off = n%4;
	mask = 0x03 << (3 - off)*2;
	v = v << (3 - off)*2;
	c = self->m_output[b] & ~mask;
	self->m_output[b] = c | v;
	return 0;
}
int phase_get_output(lamp_status_t* self,int n)
{
	int b,off;
	n -= 1;
	b = n/4;
	off = n % 4;

	return 0x03 & (self->m_output[b] >> (3 - off)*2);
}
/**
 * 根据指定的绿冲突表判断指定的相位是否有绿冲突
 */
int is_phase_green_conflict(lamp_status_t* phase, green_conflict_t* conflict)
{
	int i;
	int j;
	for(i=0; i<MAX_LAMP_GROUP_COUNT; i++){
		if(phase_get_output(phase,i+1) == 0)
			continue;
		for(j=i+1; j<MAX_LAMP_GROUP_COUNT; j++){
			if(phase_get_output(phase,j+1) > 0)
				if(green_conflict_is_conflict(conflict,i+1,j+1))
					return 1;
		}
	}
	return 0;
}

int lamp_health_get_health(lamp_health_t* self,int n)
{
	int b,off;
	unsigned char mask;
	b = (n-1)/8;
	off = 7 - (n-1)%8;
	mask = 0x01;
	mask <<= off;

	if(self->m_health[b]& mask)
		return 1;
	else
		return 0;
}
int lamp_health_set_health(lamp_health_t* self,int n, int v)
{
	int b, off;
	unsigned char mask;
	b = (n-1)/8;
	off = 7 - (n-1)%8;
	mask = 0x01;
	mask <<= off;

	if(v)
		self->m_health[b] |= mask;
	else
		self->m_health[b] &= ~mask;
	
	return 0;
}
int detector_health_get_health(detector_health_t* self, int n)
{

	int b,off;
	unsigned char mask;
	b = (n-1)/8;
	off = 7 - (n-1)%8;
	mask = 0x01;
	mask <<= off;

	if(self->m_health[b]& mask)
		return 1;
	else
		return 0;

}
int detector_health_set_health(detector_health_t* self, int n, int v)
{

	int b, off;
	unsigned char mask;
	b = (n-1)/8;
	off = 7 - (n-1)%8;
	mask = 0x01;
	mask <<= off;

	if(v)
		self->m_health[b] |= mask;
	else
		self->m_health[b] &= ~mask;
	
	return 0;
}
int detector_status_get_status(detector_status_t* self, int n)
{

	int b,off;
	unsigned char mask;
	b = (n-1)/8;
	off = 7 - (n-1)%8;
	mask = 0x01;
	mask <<= off;

	if(self->m_status[b]& mask)
		return 1;
	else
		return 0;
}
int detector_status_set_status(detector_status_t* self, int n, int v)
{

	int b, off;
	unsigned char mask;
	b = (n-1)/8;
	off = 7 - (n-1)%8;
	mask = 0x01;
	mask <<= off;

	if(v)
		self->m_status[b] |= mask;
	else
		self->m_status[b] &= ~mask;
	
	return 0;
}






