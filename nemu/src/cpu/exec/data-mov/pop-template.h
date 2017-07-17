#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	int len;
	if(DATA_BYTE == 2)
		len = 2;
	else
		len = 4;

	OPERAND_W(op_src, MEM_R(cpu.esp));
	
	cpu.esp += len;
	print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)


#include "cpu/exec/template-end.h"
