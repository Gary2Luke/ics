#include "cpu/exec/template-start.h"

/*---------------seto------------------------------*/
#define instr seto

static void do_execute() {
		if(cpu.OF == 1) 
					OPERAND_W(op_src, 1);
			else 
						OPERAND_W(op_src, 0);
				print_asm_template1();
}

make_instr_helper(rm)

#undef instr
	/*---------------setno------------------------------*/
#define instr setno

	static void do_execute() {
			if(cpu.OF == 0) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*--------------setb-------------------------------*/
#define instr setb

	static void do_execute() {
			if(cpu.CF == 1) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*--------------setae-------------------------------*/
#define instr setae

	static void do_execute() {
			if(cpu.CF == 0) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*-------------sete--------------------------------*/
#define instr sete

	static void do_execute() {
			if(cpu.ZF == 1) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*-------------setne--------------------------------*/
#define instr setne

	static void do_execute() {
			if(cpu.ZF == 0) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*--------------setbe-------------------------------*/
#define instr setbe

	static void do_execute() {
			if(cpu.CF == 1 || cpu.ZF == 1) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*--------------seta-------------------------------*/
#define instr seta

	static void do_execute() {
			if(cpu.CF == 0 && cpu.ZF == 0) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*--------------sets-------------------------------*/
#define instr sets

	static void do_execute() {
			if(cpu.SF == 1) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*-------------setns--------------------------------*/
#define instr setns

	static void do_execute() {
			if(cpu.SF == 0) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*-------------setp--------------------------------*/
#define instr setp

	static void do_execute() {
			if(cpu.PF == 1) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*-------------setnp--------------------------------*/
#define instr setnp

	static void do_execute() {
			if(cpu.PF == 0) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*--------------setl-------------------------------*/
#define instr setl

	static void do_execute() {
			if(cpu.SF != cpu.OF) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*---------------setge------------------------------*/
#define instr setge

	static void do_execute() {
			if(cpu.SF == cpu.OF) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*----------------setle-----------------------------*/
#define instr setle

	static void do_execute() {
			if(cpu.ZF == 1 || cpu.SF != cpu.OF) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*-----------------setg----------------------------*/
#define instr setg

	static void do_execute() {
			if(cpu.ZF == 0 && cpu.SF == cpu.OF) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)

#undef instr
	/*------------------setcxz---------------------------*/
#define instr setcxz

	static void do_execute() {
			if(cpu.ecx == 0) OPERAND_W(op_src, 1);
				else OPERAND_W(op_src, 0);
					print_asm_template1();
	}

make_instr_helper(rm)
#undef instr
	/*---------------------------------------------*/


#include "cpu/exec/template-end.h"
