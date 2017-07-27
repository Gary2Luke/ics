#include "nemu.h"

typedef union{
	struct{
		uint32_t offset : 12;
		uint32_t page : 10;
		uint32_t dir : 10;
	};
	uint32_t val;
} line_addr;

hwaddr_t page_translate(lnaddr_t addr){
	line_addr lnaddr;
	lnaddr.val = addr;
	PDE pde;		//页目录项
	PTE pte;		//页表项
	
	pde.val = hwaddr_read((cpu.cr3.page_directory_base << 12) + 4 * lnaddr.dir, 4);
	Assert(pde.present == 1, "PDE is not valid !!\n");
	pte.val = hwaddr_read((pde.page_frame << 12) + 4 * lnaddr.page, 4);
	Assert(pte.present == 1, "PTE is not valid !!\n");
	return (pte.page_frame << 12) + lnaddr.offset;
}
