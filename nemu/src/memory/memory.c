#include "nemu.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

uint32_t cache_read(hwaddr_t, size_t);
void cache_write(hwaddr_t, size_t, uint32_t);
lnaddr_t seg_translate(swaddr_t, uint8_t);
hwaddr_t page_translate(lnaddr_t);

uint32_t is_mmio(hwaddr_t);
uint32_t mmio_read(hwaddr_t, size_t, int);
void mmio_write(hwaddr_t, size_t, uint32_t, int);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	int num = is_mmio(addr);
	if (num==-1) 
		return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	else 
		return mmio_read(addr, len, num) & (~0u >> ((4 - len) << 3));
	//return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
//	return cache_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	int num = is_mmio(addr);
	if (num==-1) 
		dram_write(addr, len, data);
	else
		mmio_write(addr, len, data, num);
	//cache_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	if(cpu.cr0.paging == 0) return hwaddr_read(addr, len);
	if ((addr & 0xfff) + len > 0x1000) {
		uint32_t off = addr & 0xfff;
		hwaddr_t hwaddr2;
		hwaddr_t hwaddr = page_translate(addr);
		hwaddr2 = page_translate(addr + 0x1000 - off);
		return hwaddr_read(hwaddr, 0x1000 - off) + (hwaddr_read(hwaddr2, len - 0x1000 + off) << ((0x1000 - off) * 8));
	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len);
	}
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	if(cpu.cr0.paging == 0){
		hwaddr_write(addr, len, data);
		return;
	}
	if ((addr & 0xfff) + len > 0x1000) {
		uint32_t off = addr & 0xfff;
		hwaddr_t hwaddr2;
		hwaddr_t hwaddr = page_translate(addr);
		hwaddr2 = page_translate(addr + 0x1000 - off);
		hwaddr_write(hwaddr, 0x1000 - off, data);
		hwaddr_write(hwaddr2, len + off - 0x1000, data >> ((0x1000 - off) * 8));
	}
	else {
		hwaddr_t hwaddr = page_translate(addr);
		hwaddr_write(hwaddr, len, data);
	}
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, sreg);
	return lnaddr_read(lnaddr, len);
	//return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, sreg);
	//printf("addr = 0x%x, lnaddr = 0x%x\n", addr, lnaddr);
	lnaddr_write(lnaddr, len, data);
	//lnaddr_write(addr, len, data);
}

