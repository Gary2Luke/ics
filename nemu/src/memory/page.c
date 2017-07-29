#include "nemu.h"

void init_tlb();
hwaddr_t infer_tlb(lnaddr_t);

typedef union{
	struct{
		uint32_t offset : 12;
		uint32_t page : 10;
		uint32_t dir : 10;
	};
	uint32_t val;
} line_addr;

PTE infer_pte(lnaddr_t addr){
	line_addr lnaddr;
	lnaddr.val = addr;
	PDE pde;		//页目录项
	PTE pte;		//页表项
	
	pde.val = hwaddr_read((cpu.cr3.page_directory_base << 12) + 4 * lnaddr.dir, 4);
	//Assert(pde.present == 1, "PDE is not valid !!\n");
	pte.val = hwaddr_read((pde.page_frame << 12) + 4 * lnaddr.page, 4);
	//Assert(pte.present == 1, "PTE is not valid !!\n");
	return pte;
}


uint32_t last_cr3;
hwaddr_t page_translate(lnaddr_t addr){
	if(last_cr3 != cpu.cr3.val){		//更新CR3时（进程切换）强制冲刷TLB的内容,
		//printf("old_cr3 = 0x%x, cpu.cr3 = 0x%x\n", last_cr3, cpu.cr3.val);
		init_tlb();
		last_cr3 = cpu.cr3.val;
		//printf("CR3 changed !!\n");
	}
	return infer_tlb(addr);
}
