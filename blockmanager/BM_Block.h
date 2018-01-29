#ifndef _BM_BLOCK_H_
#define _BM_BLOCK_H_

#include "BM_common.h"


typedef struct {
	uint32_t	PBA;			/* PBA of this block */
	int8_t		bit[NOP];		/* index means Validity of offset pages */
	uint8_t		numValid;		/* Number of invalidate pages in this block*/
	uint32_t	PE_cycle;		/* P/E cycles of this block */
	uint8_t		*ptrNV_data;	/* Pointer of numValid map */
	uint32_t	*ptrPE_data;	/* Pointer of PE map */
	int8_t		Bad;			/* Whether this block is bad or not */
} Block;

/* Type of member variable */
#if 0
#define PBA_T	uint32_t
#define bit_T	int8_t /* Caution: bit type is actually ARRAY of int8_t */
#define nV_T	uint8_t
#define PE_T	uint32_t
#endif // 0
typedef uint32_t	PBA_T;
typedef int8_t		bit_T;  /* Caution: bit type is actually ARRAY of int8_t */
typedef uint8_t		nV_T;
typedef uint32_t	PE_T;
typedef int8_t		Bad_T;

/* �̸��� �� �ϰ������� �ٲ���...  nV_map, PE_map Ȥ�� numValid_map, PE_map �̷��� */
uint8_t* numValid_map[NOB];	/* Array of pointers to numValid */
uint32_t* PEmap[NOB];	/* Array of pointers to PE */

//Block blockArray[NOB];	/* Array of Blocks */

/* PPA�� ������ PBA�� ã�� ��ũ�� */
#define BM_PPA_TO_PBA(PPA)	PPA/PPB

/* bit member variable���� �ش� page�� Valid������ ��Ÿ���� macro */
#define BM_VALIDPAGE	1
#define BM_INVALIDPAGE	0

int32_t BM_Find_BlockPlace_by_PPA(Block* ptrBlock[], uint32_t size, uint32_t PPA);
int32_t BM_search_PBA(Block* ptrBlock[], uint32_t size, uint32_t PBA);



/* Common useful functions */
void print_arr(int32_t *arr, uint32_t size);
void print_arr2(Block* arr[], uint32_t size);
void print_arr_bit(Block* arr[], uint32_t size); // Can't work with correct manner
void print_arr_cnt(Block* arr[], uint32_t size);
void print_arr_PE(Block* arr[], uint32_t size);
void print_arr_all(Block* arr[], uint32_t size);



#endif // !_BM_BLOCK_H_


