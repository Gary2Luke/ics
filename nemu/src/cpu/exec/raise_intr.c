#include "common.h"
#include <setjmp.h>
#include "cpu/exec/helper.h"

void load_sreg(uint32_t);

extern jmp_buf jbuf;

void raise_intr(uint8_t NO, uint32_t len) {		//len为指令的长度， 当前eip+len就是下条指令的eip
	/* TODO: Trigger an interrupt/exception with ``NO''.
		 *	 * That is, use ``NO'' to index the IDT.
		 *		 */
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, cpu.eflags, R_SS);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, cpu.cs, R_SS);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, cpu.eip + len, R_SS);		//len为指令的长度， 当前eip+len就是下条指令的eip

	/*根据IDTR寄存器获得段选择符*/
	uint8_t idt[8];
	int i;
	for (i = 0; i < 8; i++)
		idt[i]=lnaddr_read(cpu.idtr.base + 8 * NO + i, 1);
	GateDesc *gde = (GateDesc*) idt;
	cpu.cs = gde->segment;		//获得段选择符，装入cs寄存器
	
	/*根据CS寄存器中的段选择符, 在GDT中进行索引, 找到一个段描述符, 并把这个段的一些信息加载到CS寄存器的描述符cache中*/	
	load_sreg(R_CS);

	/*在段描述符中读出段的基地址, 和门描述符中的offset域相加, 得出入口地址*/
	cpu.eip = cpu.SR_cache[R_CS].base + (gde->offset_31_16 << 16) + gde->offset_15_0;
	/* Jump back to cpu_exec() */
	longjmp(jbuf, 1);
}
