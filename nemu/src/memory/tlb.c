#include "common.h"
#include "time.h"
#include "stdlib.h"

#define TLB_NUM 64

PTE infer_pte(lnaddr_t);

typedef union{
	struct{
		uint32_t offset : 12;
		uint32_t tag : 20;
	};
	uint32_t addr;
} tlb_addr;

typedef struct{
	bool valid;
	PTE pte;		//最后一级页表项
	uint32_t tag;
} TLB;

TLB tlb_block[TLB_NUM];

void init_tlb(){
	int i = 0;
	for(i = 0; i < TLB_NUM; i++)
		tlb_block[i].valid = false;
}

hwaddr_t infer_tlb(lnaddr_t addr){
	tlb_addr temp;
	temp.addr = addr;
	uint32_t offset = temp.offset;
	uint32_t tag = temp.tag;

	int i = 0;
	for(i = 0; i < TLB_NUM; i++){
		if(tlb_block[i].valid && tlb_block[i].tag == tag)
			return (tlb_block[i].pte.page_frame << 12) + offset;
	}
	
	/*TLB不命中， 采用随机替换法*/
	//static uint32_t count = 0;
	//printf("TLB miss %d times!!\n", ++count);

	for(i = 0; i < TLB_NUM; i++)
		if(tlb_block[i].valid == false)
			break;

	if(i == TLB_NUM){
		srand((unsigned) time(0));
		i = rand() % TLB_NUM;
	}
	
	tlb_block[i].valid = true;
	tlb_block[i].tag = tag;
	tlb_block[i].pte = infer_pte(addr);
	
	return (tlb_block[i].pte.page_frame << 12) + offset;	

}
