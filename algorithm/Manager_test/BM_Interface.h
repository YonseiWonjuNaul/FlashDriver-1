#ifndef _BM_INTERFACE_H_
#define _BM_INTERFACE_H_

#include "BM_common.h"
#include "BM_Block.h"
#include "BM_Heap.h"


/* Function to validate the state of PPA corresponding PPA(argument) */
int32_t		BM_validate_ppa(Block* blockArray, PPA_T PPA);

/* Function to invalidate the state of PPA corresponding PPA(argument) */
int32_t		BM_invalidate_ppa(Block* blockArray, uint32_t PPA);


/* Function to check whether PPA(argument) is VALID or not */
int32_t		BM_is_invalid_ppa(Block* blockArray, uint32_t PPA);


/*
 * Function returning PBA of GC victim block
 * Select GC vimtim block which has maximum numValid
 */
uint32_t	BM_get_gc_victim(Block* blockArray, nV_T** numValid_map);


/*
 * Function returning PBA of minPE block
 */
uint32_t	BM_get_minPE_block(Block* blockArray, PE_T** PE_map);


/* 
 * Function to wear-leveling 
 */
uint32_t	BM_get_worn_block(Block *blockArray, PE_T** PE_map);


/* Function with primitive */
int32_t BM_update_block_with_gc(Block* blockArray, uint32_t PPA);


int32_t BM_update_block_with_push(Block* blockArray, uint32_t PPA);
int32_t BM_update_block_with_trim(Block* blockArray, uint32_t PPA);

// FTL -> Flash (Top -> Bottom)
// If FTL wants to access(Read, Write, Trim, ...) contents of FLASH, use this function.
inline PBA_T BM_FTLtoFLASH(Block* blockArray, PBA_T FTL_PBA)
{
	/* FTLs see virtual PBAs to ignore PE_cycle/BAD problems */
	return (blockArray[FTL_PBA].v_PBA);
}

// FTL <- Flash (Top <- Bottom)
// If FTL wants to get contents of FLASH, use this function.
// (WARNIG) FTLs don't need to use this function.
inline PBA_T BM_FLASHtoFTL(Block* blockArray, PBA_T v_PBA)
{
	/* If you want the PBA whose v_PBA is 4, then the PBA is blockArray[4].o_PBA. */
	return (blockArray[v_PBA].o_PBA);
}


#endif // !_BM_INTERFACE_H_