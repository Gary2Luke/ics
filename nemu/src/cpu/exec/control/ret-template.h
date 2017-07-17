#include "cpu/exec/template-start.h"

#define instr ret

static void do_execute() {
	//int len = get_len();
	cpu.eip = swaddr_read(cpu.esp, 4);
	cpu.esp += 4 + op_src->val;
	cpu.eip -= (1 + DATA_BYTE);
	print_asm("ret $0x%x", op_src->val);
}

make_instr_helper(i)


#include "cpu/exec/template-end.h"
