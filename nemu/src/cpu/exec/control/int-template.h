#include "cpu/exec/template-start.h"

#define instr int

void raise_intr(uint8_t, uint32_t);

static void do_execute() {
	int len = get_len();
	raise_intr(op_src->val, len + 1);		//len + 1为指令的长度
	print_asm_template1();
}

make_instr_helper(i)


#include "cpu/exec/template-end.h"
