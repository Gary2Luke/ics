#include "cpu/exec/helper.h"

#ifndef __CDQ_H__
#define __CDQ_H__

make_helper(cdq){
	if((cpu.eax >> 31) == 1)
		cpu.edx = 0xffffffff;
	else
		cpu.edx = 0;
	print_asm("cdq");
	return 1;
}

#endif
