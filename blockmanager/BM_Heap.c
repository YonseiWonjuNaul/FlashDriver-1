/* BM_HEAP */

#include "BM_Heap.h"

/*
 * Functions for numValid_map with Min-HEAP 
 */

 /* Make Min-Heap with the pointers of numValid_map by numValid in blockArray */
//int32_t BM_Minheap_numValid(Block* blockArray, nV_T** numValid_map)
int32_t BM_Minheap_numValid(Block* blockArray, Block** numValid_map)
{
	nV_T* temp_NV = (nV_T*)malloc(sizeof(nV_T) * _NOB);

	for (int i = 0; i < _NOB; ++i) {
		if (blockArray[i].BAD == _NOTBADSTATE)
			//temp_NV[i] = blockArray[i].numValid;
			temp_NV[i] = BM_GETNUMVALID(numValid_map[i]);
		else
			temp_NV[i] = _PPB;//before: 0xff // Bad Block would go to leaf node.
	}

	BM__buildminheapNV(temp_NV, numValid_map); 

	free(temp_NV); 
	return 0;
}


/* Build min-heap by numValid */
//int32_t BM__buildminheapNV(nV_T* temp_NV, nV_T** numValid_map)
int32_t BM__buildminheapNV(nV_T* temp_NV, Block** numValid_map)
{
	int32_t i;
	for (i = _NOB / 2; i >= 0; --i) {
		BM__minheapifyNV(temp_NV, i, numValid_map);
	}
	return 0;
}

//int32_t BM__minheapifyNV(nV_T* temp_NV, int32_t i, nV_T** numValid_map)
int32_t BM__minheapifyNV(nV_T* temp_NV, int32_t i, Block** numValid_map)
{
	int32_t l = 2 * i + 1;
	int32_t r = 2 * i + 2;
	int32_t smallest;

	if (l <= _NOB - 1 && (unsigned)temp_NV[l] < (unsigned)temp_NV[i]) smallest = l;
	else smallest = i;

	if (r <= _NOB - 1 && (unsigned)temp_NV[r] < (unsigned)temp_NV[smallest])	smallest = r;
	if (smallest != i) {                      
		
		SWAP_NV(temp_NV[i], temp_NV[smallest]);
		SWAP_NV_PTR(numValid_map[i], numValid_map[smallest]); 
		
		BM__minheapifyNV(temp_NV, smallest, numValid_map);
	}
	return 0;
}


/*
 * Functions for numValid_map with MAX-HEAP 
 */

 /* Make Max-Heap with the pointers of numValid_map by numValid in blockArray */
// This function is not used..
//int32_t BM_Maxheap_numValid(Block* blockArray, nV_T** numValid_map)
int32_t BM_Maxheap_numValid(Block* blockArray, Block** numValid_map)
{
	nV_T* temp_NV = (nV_T*)malloc(sizeof(nV_T) * _NOB);

	for (int i = 0; i < _NOB; ++i) {
		//temp_NV[i] = blockArray[i].numValid;
		// Maybe need to add badblock check in here
		temp_NV[i] = BM_GETNUMVALID(numValid_map[i]);
	}

	BM__buildmaxheapNV(temp_NV, numValid_map); 

	free(temp_NV); 
	return 0;
}


/* Build max-heap by numValid */
//int32_t BM__buildmaxheapNV(nV_T* temp_NV, nV_T** numValid_map)
int32_t BM__buildmaxheapNV(nV_T* temp_NV, Block** numValid_map)
{
	int32_t i;
	for (i = _NOB / 2; i >= 0; --i) {
		BM__maxheapifyNV(temp_NV, i, numValid_map);
	}
	return 0;
}

//int32_t BM__maxheapifyNV(nV_T* temp_NV, int32_t i, nV_T** numValid_map)
int32_t BM__maxheapifyNV(nV_T* temp_NV, int32_t i, Block** numValid_map)
{
	int32_t l = 2 * i + 1;
	int32_t r = 2 * i + 2;
	int32_t largest;

	if (l <= _NOB - 1 && temp_NV[l] > temp_NV[i]) largest = l;
	else largest = i;

	if (r <= _NOB - 1 && temp_NV[r] > temp_NV[largest])	largest = r;
	if (largest != i) {                      
		
		SWAP_NV(temp_NV[i], temp_NV[largest]);
		SWAP_NV_PTR(numValid_map[i], numValid_map[largest]); 
		
		BM__maxheapifyNV(temp_NV, largest, numValid_map);
	}
	return 0;
}

/*
 * Functions for PE_cycle with MIN-HEAP 
 * !!! FOLLOWINGS ARE NOT VERIFIED !!!
 */

 /* Make Min-Heap with the pointers of PE_map by PE_cycle in blockArray */
int32_t BM_Minheap_PEcycle(Block* blockArray, Block** PE_map)
{
	PE_T* temp_PE = (PE_T*)malloc(sizeof(PE_T) * _NOB);

	for (int i = 0; i < _NOB; ++i) {
		//temp_PE[i] = blockArray[i].PE_cycle;
		temp_PE[i] = BM_GETPECYCLE(PE_map[i]);
	}


	BM__buildminheapPE(temp_PE, PE_map); 

	free(temp_PE); 
	return 0;
}


/* Build min-heap by PE_cycle */
int32_t BM__buildminheapPE(PE_T* temp_PE, Block** PE_map)
{
	int32_t i;
	for (i = _NOB / 2; i >= 0; --i) {
		BM__minheapifyPE(temp_PE, i, PE_map); // Is that OKAY?? I don't know about 'MINheapify'
	}
	return 0;
}

int32_t BM__minheapifyPE(PE_T* temp_PE, int32_t i, Block** PE_map)
{
	int32_t l = 2 * i + 1;
	int32_t r = 2 * i + 2;
	int32_t smallest;

	if (l <= _NOB - 1 && temp_PE[l] < temp_PE[i]) smallest = l;
	else smallest = i;

	if (r <= _NOB - 1 && temp_PE[r] < temp_PE[smallest])	smallest = r;
	if (smallest != i) {
		
		SWAP_PE(temp_PE[i], temp_PE[smallest]);
		SWAP_PE_PTR(PE_map[i], PE_map[smallest]); 
		
		BM__minheapifyPE(temp_PE, smallest, PE_map);
	}
	return 0;
}




/*
 * Functions for PE_map with SORT
 */

/* Sorting the pointers of PE_map by PE_cycle in blockArray */
int32_t BM_SortPE(Block* blockArray, Block** PE_map)
{
	PE_T* temp_PE = (PE_T*)malloc(sizeof(PE_T) * _NOB);

	for (int i = 0; i < _NOB; ++i) {
		//temp_PE[i] = blockArray[i].PE_cycle;
		temp_PE[i] = BM_GETPECYCLE(PE_map[i]);
	}

	BM__quicksort(temp_PE, 0, _NOB - 1, PE_map); // Need Verification of this function


	free(temp_PE);
	return(eNOERROR);
}

void BM__quicksort(PE_T* temp_PE, int p, int r, Block** PE_map) 
{
	int q;
	//int x;
	PE_T x;
	int i;
	int j;
	if (p < r) { // partitioning code
		x = temp_PE[r];
		i = p - 1;
		for (j = p; j < r; j++) {
			if (temp_PE[j] <= x) {
				i++; SWAP_PE(temp_PE[i], temp_PE[j]); 
				SWAP_PE_PTR(PE_map[i], PE_map[j]);
			}
		}
		SWAP_PE(temp_PE[i + 1], temp_PE[r]);	SWAP_PE_PTR(PE_map[i + 1], PE_map[r]);
		q = i + 1; // divide(partitioning end)
		BM__quicksort(temp_PE, p, q - 1, PE_map); // recursive call 
		BM__quicksort(temp_PE, q + 1, r, PE_map);
	}
}

/* If we want Maxheap_PE, comparing by (signed) casting. Then, PE of bad block 0xffffffff would be -1, so bad block will go to leaf nodes. */