#include "common.h"
#include "burst.h"
#include "misc.h"

/* Simulate the (main) behavor of Cache.*/

#define CACHE_SIZE 64 << 10
#define BLOCK_SIZE 64
#define ASSOCIATE 8				//相联度

#define OFFSET_BITS 6				// log2(block_size)
#define INDEX_BITS 8					//(cache_size / block_size) / associate
#define TAG_BITS 18					// 32 - index - offset

typedef union {
	struct {
		uint32_t offset	: OFFSET_BITS;
		uint32_t index	: INDEX_BITS;
		uint32_t tag	: TAG_BITS;
	};
	uint32_t addr;
} cache_addr;



uint8_t dram[NR_RANK][NR_BANK][NR_ROW][NR_COL];
uint8_t *hw_mem = (void *)dram;

typedef struct {
	uint8_t buf[OFFSET_BITS];
	uint32_t tag;
	bool valid;
} CB;

CB cache_block [ASSOCIATE][INDEX_BITS];

void init_cache() {
	int i, j;
	for(i = 0; i < ASSOCIATE; i ++) {
		for(j = 0; j < INDEX_BITS; j ++) {
			cache_block[i][j].valid = false;
		}
	}
}

uint32_t cache_read(hwaddr_t addr, size_t len) {
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	cache_addr temp;
	temp.addr = addr;
	uint32_t offset = temp.offset;
	uint32_t index = temp.index;
	uint32_t tag = temp.tag;

	int i;
	for(i = 0; i < ASSOCIATE; i++){
		if((cache_block[i][index].valid && cache_block[i][index].tag == tag) ) {
			if(offset + len <= BLOCK_SIZE){
				uint32_t res;
				memcpy(&res, &cache_block[i][index].buf[offset], len);
				return res;
			}
			else{
				uint32_t low, high;
				uint32_t len1, len2;
				len2 = offset + len - BLOCK_SIZE;
				len1 = len - len2;
				memcpy(&low, &cache_block[i][index].buf[offset], 4);
				high = cache_read(addr + len1, 4) << (len1 * 8);
				unalign_rw(&high, len1) = low;
				return high;
			}
		}

	}

	/*读cache miss, 采用随机替换法*/
	for(i = 0; i < ASSOCIATE; i++)
		if(cache[i][index].valid == false)
			break;

	if(i == ASSOCIATE){		
		srand((unsigned) time(0)); 
		i = rand() % ASSOCIATE;		
	}
	cache_block[i][index].tag = tag;
	cache_block[i][index].valid = true;
	cache_addr tmp_addr = addr;
	tmp_addr.offset = 0;
	int j = 0;
	for(j = 0; j < BLOCK_SIZE; j++)
		cache_block[i][index].buf[j] = dram_read(tmp_addr+j, 1);

	return cache_read(addr, len);
	
	
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	cache_addr temp;
	temp.addr = addr;
	uint32_t offset = temp.offset;
	uint32_t index = temp.index;
	uint32_t tag = temp.tag;
	
	int i = 0;
	for(i = 0; i < ASSOCIATE; i++){
		if((cache_block[i][index].valid && cache_block[i][index].tag == tag) ) {
			if(offset + len <= BLOCK_SIZE){				
				memcpy(&cache_block[i][index].buf[offset], len, &data);
				return;
			}
			else{
			}
		}

	}

}


