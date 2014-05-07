/**
 * vm.t.c
 * 
 * test vm interface
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-08 16:54:42 中国标准时间
 * 
 * $log$
 *
 */
#include <assert.h>
#include "vm.h"
#include "io.h"



int main(int argc, char* argv[])
{
	vm_t vm;
	unsigned char ch1,ch2;
	vm_init(&vm);
	vm.m_try_count = 3;
	vm.m_fd = open_comm(8);
	assert(vm.m_fd == 8);
	ch1 = ch2 = 0;
	while(1){
		ch2 = ch1;
		assert(vm_mcu_ping(&vm,ch1) == 0);
		assert(ch1 == ch2);
	}
	close_comm(vm.m_fd);
	return 0;
}
