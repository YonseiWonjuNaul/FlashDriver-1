#ifndef _BM_BLOCK_H_
#define _BM_BLOCK_H_

#include "BM_common.h"


typedef struct { // 16 bytes
	uint32_t	PBA;			/* PBA of this block */
	int8_t		ValidP[NOP];	/* index means Validity of offset pages. 1 means VALID, 0 means INVALID */
	uint8_t		numValid;		/* Number of Valid pages in this block*/
	uint32_t	PE_cycle;		/* P/E cycles of this block */
	uint8_t		*ptrNV_data;	/* Pointer of numValid map */
	uint32_t	*ptrPE_data;	/* Pointer of PE map */
	int8_t		BAD;			/* Whether this block is bad or not */
} Block;

/* Type of member variable */
typedef uint32_t	PBA_T;
typedef int8_t		ValidP_T;  /* Caution: ValidP type is actually ARRAY of int8_t */
typedef uint8_t		nV_T;
typedef uint32_t	PE_T;
typedef int8_t		Bad_T;


/* BAD Status of blocks */
#define _BADSTATE	1
#define _NOTBADSTATE	0

/* (IGNORE!) Don't Care about that NOW (It's incomplete) */
/* Whether blockArray exists or not */
#define _BA_EXIST	32
#define _BA_GOODSTATE	10
#define _BA_BADSTATE	20

/* Macros for finding PBA from PPA */
#define BM_PPA_TO_PBA(PPA)	PPA/PPB

/* Macros that indicate whether the page is valid or not */
#define BM_VALIDPAGE	1
#define BM_INVALIDPAGE	0
#define BM_WRITTEN		1	/* (IGNORE!) Not Determined yet */


int32_t BM_Find_BlockPlace_by_PPA(Block* ptrBlock[], uint32_t size, uint32_t PPA);
int32_t BM_search_PBA(Block* ptrBlock[], uint32_t size, uint32_t PBA);

#endif // !_BM_BLOCK_H_


