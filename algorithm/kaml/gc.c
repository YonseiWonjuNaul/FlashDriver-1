#include <stdlib.h>
#include <stdio.h>
#include "gc.h"

extern struct algorithm algo_pftl;
extern int OOB[_NOP];
extern int mapping_table[_NOP];	// use LPA as index, use 1 block as log block
extern uint8_t garbage_table[_NOP/8];

extern int log_seg_num; 
extern int gc_read_cnt;
extern int gc_target_cnt;

int garbage_collection(int reserv_ppa_start, int erase_seg_num)
{
	int invalid_cnt = 0;
	printf("[AT GC]---------------------------------\n");
	gc_read_cnt=0;
	gc_target_cnt=0;
	
	int start_page_num = erase_seg_num * _PPS;	// reserved segment
	int end_page_num = (erase_seg_num+1) * _PPS;
	uint8_t bit_compare;
	
	for(uint32_t i = start_page_num; i < end_page_num; i++){ 	//valid checking
		value_set *value_w;

		if (garbage_table[i/8] & (1 << (i % 8))) {  // 1: invalid
			invalid_cnt++; 
			if(invalid_cnt == _PPS) {	// all page is invalid

				for(int i=start_page_num; i<start_page_num+_PPS; i++){
					garbage_table[i/8] &= ~(1 << (i % 8));
				}

				algo_pftl.li->trim_block(start_page_num, ASYNC);  //delete all segment
				return start_page_num; 
			}
		}
		else {//copy on reserved segment	// 0: valid
			algo_req *my_req = (algo_req*)malloc(sizeof(algo_req));
			value_set *value_r = inf_get_valueset(NULL, FS_MALLOC_R, PAGESIZE);

			mapping_table[OOB[i]] = reserv_ppa_start;	// mapping_table update
			OOB[reserv_ppa_start] = OOB[i];			// OOB update
			OOB[i] = -1;
			garbage_table[reserv_ppa_start/8] &= ~(1 << (reserv_ppa_start % 8));

			//GC_R
			my_req->type = GC_R;
			gc_read_cnt++;
			algo_pftl.li->read(i, PAGESIZE, value_r, 1, my_req);
			
			//waiting for gc_read
			gc_general_waiting();
			
			//GC_W
			my_req = (algo_req *)malloc(sizeof(algo_req));
			my_req->type = GC_W;
			
			printf("[IN GC] value_r: %x\n", value_r);
			printf("[IN GC] value_r->value: %s\n", value_r->value);
			value_w = inf_get_valueset(value_r->value, FS_MALLOC_W, PAGESIZE);
		//	printf("[IN GC] value_w->value: %s\n", value_w->value);

			inf_free_valueset(value_r, FS_MALLOC_R);
			algo_pftl.li->write(reserv_ppa_start, PAGESIZE, value_w, 1, my_req);
		//	inf_free_valueset(value_w, FS_MALLOC_W);
			
			//increase reserved ppa number
			reserv_ppa_start++;
		}
	}
	
	printf("before trim\n");
	/*      delete old segment	*/
	algo_pftl.li->trim_block(start_page_num, ASYNC);
	printf("after trim\n");
	
	// return update reserv segment number
	log_seg_num = start_page_num / _PPS;

	for(int i = start_page_num; i < end_page_num; i++){
		garbage_table[i/8] &= ~(1 << (i % 8));
	}
	printf("[AT GC] END OF GC\n");

	return reserv_ppa_start;
}

void gc_general_waiting()
{
	//spin lcok for gc_read
	while(gc_read_cnt != gc_target_cnt) { }

	gc_read_cnt = 0;
	gc_target_cnt = 0;
}
