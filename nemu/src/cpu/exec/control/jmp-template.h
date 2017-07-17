#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	if(op_src->type == OP_TYPE_IMM){
		if(MSB(op_src->val) == 1){
			if (DATA_BYTE == 1)
				op_src->val |= 0xffffff00;
			else if (DATA_BYTE == 2)
				op_src->val |= 0xffff0000;
		}
		cpu.eip += op_src->val;
		if(DATA_BYTE == 2)
			cpu.eip &= 0x0000ffff;
		print_asm("jmp $0x%x", cpu.eip + DATA_BYTE + 1);
	}
	else{
		#if DATA_BYTE == 2 
		cpu.eip = (op_src->val & 0x0000ffff) - 2;
		#else 
		if (op_src->type == OP_TYPE_MEM) 
			cpu.eip = op_src->val - DATA_BYTE - 3;
		else
			cpu.eip = op_src->val - 2;
		#endif
		print_asm("jmp $%s", op_src->str);
	}
}

make_instr_helper(i)
make_instr_helper(rm)

make_helper(concat(ljmp_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
//	uint16_t sreg = instr_fetch(eip + 5, 2);
//	cpu.cs = sreg;
//	load_sreg(R_CS);
	cpu.eip = addr - 7;
//	print_asm("ljmp" str(SUFFIX) " $0x%x,$0x%x", sreg, addr);

	return 7;
}

#include "cpu/exec/template-end.h"
