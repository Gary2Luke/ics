#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute() {
	DATA_TYPE res = op_dest->val + op_src->val + cpu.CF; 
	update_EFLAGS(res);
	if(MSB(op_dest->val) == MSB(op_src->val) && MSB(res) != MSB(op_src->val))
		cpu.OF = 1;
	else
		cpu.OF = 0;

	long long tmp = (long long) op_dest->val + (long long) op_src->val + (long long) cpu.CF;
	if(tmp >> (8 * DATA_BYTE))
		cpu.CF = 1;
	else
		cpu.CF = 0;

	cpu.AF = ((op_dest->val & 0x7) + (op_src->val & 0x7) + cpu.CF) > 0x7 ? 0 : 1;
	OPERAND_W(op_dest, res);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"
