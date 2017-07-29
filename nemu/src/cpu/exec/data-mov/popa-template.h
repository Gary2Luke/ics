#include "cpu/exec/template-start.h"

#define instr popa

static void do_execute() {
	int i;
	
	for (i = 7; i >= 0; i--) {
		if (i == 4) 
			;
		else 
			cpu.gpr[i]._32 = swaddr_read(cpu.esp, DATA_BYTE, R_SS);
		cpu.esp += DATA_BYTE;
	}
	print_asm_template1();
}


make_instr_helper(r)

#include "cpu/exec/template-end.h"
