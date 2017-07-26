#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	//int len = get_len();
	cpu.esp -= 4;		//用来push下条指令地址，指令地址为32位
	if(op_src->type == OP_TYPE_IMM){
		swaddr_write(cpu.esp, 4, cpu.eip + DATA_BYTE + 1, R_SS);
		
		cpu.eip += op_src->val;	//其实这里eip应该为	cpu.eip+=1+len+op_src->val,但是cpu_exec中要加上len+1	
		if(DATA_BYTE == 2)
			cpu.eip &= 0x0000ffff;
		print_asm("call $0x%x", cpu.eip+DATA_BYTE+1);

	}
	else{
		swaddr_write(cpu.esp, 4, cpu.eip + 2, R_SS);
		cpu.eip = op_src->val - 2; //其实这里eip应该为 cpu.eip==op_src->val,但是cpu_exec中要加上len+1	
		print_asm("call $0x%x", cpu.eip + 2);
	
	}
//	print_asm("call $0x%x", cpu.eip+DATA_BYTE+1);
	//printf("hello\n");

}

make_instr_helper(i)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"
