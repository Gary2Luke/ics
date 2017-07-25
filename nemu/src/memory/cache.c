#include "common.h"
#include "burst.h"
#include "misc.h"
#include "time.h"
#include "stdlib.h"

/* Simulate the (main) behavor of Cache.*/

#define CACHE_SIZE (64 << 10)
#define BLOCK_SIZE 64
#define ASSOCIATE 8				//相联度

#define OFFSET_BITS 6				// log2(block_size)
#define INDEX_BITS 7				//log2 ( (cache_size / block_size) / associate )
#define TAG_BITS 19					// 32 - index - offset

//#define BLOCK_NUM  (CACHE_SIZE / BLOCK_SIZE)
#define GROUP_NUM (1 << INDEX_BITS)    //(cache_size / block_size) / associate 

uint32_t cache2_read(hwaddr_t, size_t);
void cache2_write(hwaddr_t, size_t, uint32_t);
int64_t run_time;		//模拟cache运行时间

typedef union {
	struct {
		uint32_t offset	: OFFSET_BITS;
		uint32_t index	: INDEX_BITS;
		uint32_t tag	: TAG_BITS;
	};
	uint32_t addr;
} cache_addr;

typedef struct {
	uint8_t buf[BLOCK_SIZE];
	uint32_t tag;
	bool valid;
} CB;

CB cache_block [ASSOCIATE][GROUP_NUM];

void init_cache() {
	run_time = 0;
	int i, j;
	for(i = 0; i < ASSOCIATE; i++) {
		for(j = 0; j < GROUP_NUM; j++) {
			cache_block[i][j].valid = false;
		}
	}
}

uint32_t cache_read(hwaddr_t addr, size_t len) {
	cache_addr temp;
	temp.addr = addr;
	uint32_t offset = temp.offset;
	uint32_t index = temp.index;
	uint32_t tag = temp.tag;

	int i;
	for(i = 0; i < ASSOCIATE; i++){
		if((cache_block[i][index].valid && cache_block[i][index].tag == tag) ) {
			run_time += 2;
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
	run_time += 200;

	for(i = 0; i < ASSOCIATE; i++)
		if(cache_block[i][index].valid == false)
			break;

	if(i == ASSOCIATE){		
		srand((unsigned) time(0)); 
		i = rand() % ASSOCIATE;		
	}
	cache_block[i][index].tag = tag;
	cache_block[i][index].valid = true;
	cache_addr tmp_addr = temp;
	tmp_addr.offset = 0;
	int j = 0;
	for(j = 0; j < BLOCK_SIZE; j++)
		cache_block[i][index].buf[j] = cache2_read(tmp_addr.addr + j, 1);

	return cache_read(addr, len);
	
	
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	cache_addr temp;
	temp.addr = addr;
	uint32_t offset = temp.offset;
	uint32_t index = temp.index;
	uint32_t tag = temp.tag;
	
	int i = 0;
	for(i = 0; i < ASSOCIATE; i++){
		if((cache_block[i][index].valid && cache_block[i][index].tag == tag) ) {

			run_time += 2;

			if(offset + len <= BLOCK_SIZE){				
				memcpy(&cache_block[i][index].buf[offset], &data, len);
				
			}
			else{			
				uint32_t len1, len2;
				len2 = offset + len - BLOCK_SIZE;
				len1 = len - len2;
				int j = 0;
				for(j = 0; j < len1; j++){
					uint32_t tmp_v = data >> (j * 8);
					cache_block[i][index].buf[offset + j] = unalign_rw(&tmp_v, 1);
				}
				cache_write(addr+len1, len2, data >> (len1 * 8));
			}
		
		break;
		}
	}

	/*写穿透，写不分配*/
	run_time += 200;	
	cache2_write(addr, len, data);
}

void cache_debug(swaddr_t addr){
	cache_addr temp;
	temp.addr = addr;
	uint32_t offset = temp.offset;
	uint32_t index = temp.index;
	uint32_t tag = temp.tag;
	
	printf("addr = 0x%08x, tag = 0x%x, index = 0x%x, offset = 0x%x\n", addr, tag, index, offset);
	int i = 0;
	for(i = 0; i < ASSOCIATE; i++){
		if((cache_block[i][index].valid && cache_block[i][index].tag == tag) ) {
			printf("L1 Cache Hit !!\n");
			cache_addr tmpaddr = temp;
            tmpaddr.offset = 0;
            printf("cache block: begin at address %08x\n", tmpaddr.addr);
            int j = 0;
            for(j = 0; j < BLOCK_SIZE; ++j){
	            printf("%02x ", cache_block[i][index].buf[j]);
	            if(j % 16 == 15) printf("\n");
	        }
	        printf("\n");
            return;
		}
		
	}
	printf("L1 cache Missed !!\n");
}


#undef CACHE_SIZE 
#undef BLOCK_SIZE 
#undef ASSOCIATE 				//相联度

#undef OFFSET_BITS 				// log2(block_size)
#undef INDEX_BITS 					//(cache_size / block_size) / associate
#undef TAG_BITS 					// 32 - index - offset

#undef GROUP_NUM

