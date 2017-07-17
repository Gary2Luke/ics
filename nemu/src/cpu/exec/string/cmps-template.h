#include "cpu/exec/template-start.h"

#define instr cmps

make_helper(concat(cmps_, SUFFIX)) {
	DATA_TYPE tmp1 = MEM_R(cpu.esi);
	DATA_TYPE tmp2 = MEM_R(cpu.edi);
	DATA_TYPE res = tmp1 - tmp2; 
	update_EFLAGS(res);
	if((tmp1 != tmp2) && MSB(res) == MSB(tmp2))
		cpu.OF = 1;
	else
		cpu.OF = 0;

	cpu.CF = ((long long) tmp1 - (long long) tmp2) >> (8 * DATA_BYTE) & 1;
	cpu.AF = (tmp1 & 0x7) > (tmp2 & 0x7) ? 0 : 1;
	
	if(cpu.DF == 0){
		cpu.esi += DATA_BYTE;
		cpu.edi += DATA_BYTE;
	}
	else{
		cpu.esi -= DATA_BYTE;
		cpu.edi -= DATA_BYTE;
	}

	print_asm("cmps" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
