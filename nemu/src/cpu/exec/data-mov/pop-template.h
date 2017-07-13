#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	
	cpu.esp += DATA_BYTE;
	print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)


#include "cpu/exec/template-end.h"
