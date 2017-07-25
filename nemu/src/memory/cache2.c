#include "common.h"
#include "burst.h"
#include "misc.h"
#include "time.h"
#include "stdlib.h"

/* Simulate the (main) behavor of L2 Cache.*/

#define CACHE_SIZE (4 << 20)
#define BLOCK_SIZE 64
#define ASSOCIATE 16				//相联度

#define OFFSET_BITS 6				// log2(block_size)
#define INDEX_BITS 12				//log2 ( (cache_size / block_size) / associate )
#define TAG_BITS 14					// 32 - index - offset

//#define BLOCK_NUM  (CACHE_SIZE / BLOCK_SIZE)
#define GROUP_NUM (1 << INDEX_BITS)    //(cache_size / block_size) / associate 

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
//int cache2_time;

typedef union {
	struct {
		uint32_t offset	: OFFSET_BITS;
		uint32_t index	: INDEX_BITS;
		uint32_t tag	: TAG_BITS;
	};
	uint32_t addr;
} cache2_addr;

typedef struct {
	uint8_t buf[BLOCK_SIZE];
	uint32_t tag;
	bool valid;
	bool dirty;
} CB2;

CB2 cache2_block [ASSOCIATE][GROUP_NUM];

void init_cache2() {
	//cache2_time = 0;
	int i, j;
	for(i = 0; i < ASSOCIATE; i++) {
		for(j = 0; j < GROUP_NUM; j++) {
			cache2_block[i][j].valid = false;
			cache2_block[i][j].dirty = false;
		}
	}
}

void cache2_writeback(uint32_t i, uint32_t index){
	if(cache2_block[i][index].dirty == false)
		return;
	cache2_addr tmp;
	tmp.index = index;
	tmp.tag = cache2_block[i][index].tag;

	int j = 0;
	for(j = 0; j < BLOCK_SIZE; j++){
		tmp.offset = j;
		dram_write(tmp.addr, 1, cache2_block[i][index].buf[j]);
	}
}

uint32_t cache2_read(hwaddr_t addr, size_t len) {
	//cache2_time++;
	cache2_addr temp;
	temp.addr = addr;
	uint32_t offset = temp.offset;
	uint32_t index = temp.index;
	uint32_t tag = temp.tag;

	int i;
	for(i = 0; i < ASSOCIATE; i++){
		if((cache2_block[i][index].valid && cache2_block[i][index].tag == tag) ) {
			if(offset + len <= BLOCK_SIZE){
				uint32_t res;
				memcpy(&res, &cache2_block[i][index].buf[offset], len);
				return res;
			}
			else{
				uint32_t low, high;
				uint32_t len1, len2;
				len2 = offset + len - BLOCK_SIZE;
				len1 = len - len2;
				memcpy(&low, &cache2_block[i][index].buf[offset], 4);
				high = cache2_read(addr + len1, 4) << (len1 * 8);

				if(len1 == 1)
					unalign_rw(&high, 1) = low;
				else if(len1 == 2)
					unalign_rw(&high, 2) = low;
				else if(len1 == 3)
					unalign_rw(&high, 3) = low;
			
				return high;
			}
		}
	}

	/*读cache miss, 采用随机替换法*/
	
	for(i = 0; i < ASSOCIATE; i++)
		if(cache2_block[i][index].valid == false)
			break;

	if(i == ASSOCIATE){		
		srand((unsigned) time(0)); 
		i = rand() % ASSOCIATE;		
	}
	cache2_writeback(i, index);
	cache2_block[i][index].tag = tag;
	cache2_block[i][index].valid = true;
	cache2_block[i][index].dirty = false;
	cache2_addr tmp_addr = temp;
	tmp_addr.offset = 0;
	int j = 0;
	for(j = 0; j < BLOCK_SIZE; j++)
		cache2_block[i][index].buf[j] = dram_read(tmp_addr.addr + j, 1);

	return cache2_read(addr, len);
}

void cache2_write(hwaddr_t addr, size_t len, uint32_t data) {
	cache2_addr temp;
	temp.addr = addr;
	uint32_t offset = temp.offset;
	uint32_t index = temp.index;
	uint32_t tag = temp.tag;
	
	int i = 0;
	for(i = 0; i < ASSOCIATE; i++){
		if((cache2_block[i][index].valid && cache2_block[i][index].tag == tag) ) {
			if(offset + len <= BLOCK_SIZE){				
				memcpy(&cache2_block[i][index].buf[offset], &data, len);
				cache2_block[i][index].dirty = true;			
			}
			else{			
				uint32_t len1, len2;
				len2 = offset + len - BLOCK_SIZE;
				len1 = len - len2;
				int j = 0;
				for(j = 0; j < len1; j++){
					uint32_t tmp_v = data >> (j * 8);
					cache2_block[i][index].buf[offset + j] = unalign_rw(&tmp_v, 1);
					cache2_block[i][index].dirty = true;
				}
				cache2_write(addr+len1, len2, data >> (len1 * 8));
			}		
			return;			//写命中时：写回
		}
	}

	/*写不命中时： 写分配,先随机替换出一个块*/
	for(i = 0; i < ASSOCIATE; i++)
		if(cache2_block[i][index].valid == false)
			break;

	if(i == ASSOCIATE){		
		srand((unsigned) time(0)); 
		i = rand() % ASSOCIATE;		
	}
	cache2_writeback(i, index);
	cache2_block[i][index].tag = tag;
	cache2_block[i][index].valid = true;
	cache2_block[i][index].dirty = false;
	cache2_addr tmp_addr = temp;
	tmp_addr.offset = 0;
	int j = 0;
	for(j = 0; j < BLOCK_SIZE; j++)
		cache2_block[i][index].buf[j] = dram_read(tmp_addr.addr + j, 1);


	cache2_write(addr, len, data);
	

}


#undef CACHE_SIZE 
#undef BLOCK_SIZE 
#undef ASSOCIATE 				//相联度

#undef OFFSET_BITS 				// log2(block_size)
#undef INDEX_BITS 					//(cache_size / block_size) / associate
#undef TAG_BITS 					// 32 - index - offset

#undef GROUP_NUM

