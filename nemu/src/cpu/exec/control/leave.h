#ifndef __LEAVE_H__
#define __LEAVE_H__

make_helper(leave){
	cpu.esp = cpu.ebp;
	cpu.ebp = swaddr_read(cpu.esp, 4, R_SS);
	cpu.esp += 4;
	print_asm("leave");
	return 1;
}


#endif

