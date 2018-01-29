/* BM_HEAP */

#include "BM_Heap.h"

#if 0
void max_heapify_cnt_(Block* ptrBlock[], uint32_t size_, int32_t i)
{
	int32_t l = 2 * i + 1;
	int32_t r = 2 * i + 2;
	int32_t largest;
	int32_t size = (int32_t)size_;

	if (l <= size - 1 && ptrBlock[l]->numValid > ptrBlock[i]->numValid)	largest = l;
	else largest = i;

	if (r <= size - 1 && ptrBlock[r]->numValid > ptrBlock[largest]->numValid)	largest = r; // if, else, if�� ������ ���� ���� ū value�� ������ index�� ã�Ƴ� ��,
	if (largest != i) {                                          // ���� �θ� ����� ���� i�� largest�� �ƴ϶�� largest�� i�� �ٲ۴�.
																 //SWAP(Block_list[i].cnt, Block_list[largest].cnt);
		//SWAP(ptrBlock[i]->cnt, ptrBlock[largest]->cnt);
		SWAP_BLOCK(ptrBlock[i], ptrBlock[largest]);
		max_heapify_cnt_(ptrBlock, size, largest); // �� �Ŀ��� ��������� �ݺ�
	}
}

/* Sorting ptrBlock[] by cnt */
// ���ʿ� ptrBlock�� �迭���� ��� ���Ļ��°� �ٸ���.. cnt �������� �ѹ��� Block������ SWAP�Ѵٰ� �ؼ� �װ� cnt->PBA ������ �ٷ� �ǳ�?
void build_max_heap_cnt_(Block* ptrBlock[], uint32_t size)
{
	int32_t i;
	for (i = size / 2; i >= 0; --i) {
		max_heapify_cnt_(ptrBlock, size, i);
	}
}






/* ---------------------------------- */
// Heap ��� �� �����ϱ�. ������ ���ݹۿ� ����
// ������ �밳 int list�� �������� �������������, ���߿� Blocklist�� �ٲ� ��

/* Return and Extract(Erase) Maximum value in Max-heap */
uint32_t heap_maximum_extract(int* list, int size)
{
	uint32_t max;
	if (size < 1) {
		ERR(eHEAPUNDERFLOW_BM);
		printf("heap underflow\n");
	}
	max = list[0];
	list[0] = list[size - 1];
	list = (int*)realloc(list, sizeof(int) * --size);
	max_heapify(list, size, 0);
	return max;
}

/* Return Maximum value in Max-heap */
uint32_t heap_maximum(int* list) // int*�� �ƴ϶� Block*�̾�� �Ѵ�. 
{
	return list[0]; // return type�� ����� uint32_t�� �ƴ϶� Block
}

/* Return */
Block heap_maximum_cnt(Block* Block_list)
{
	// �� �̰� get victim�ΰ�
	return Block_list[111];
}

/*
void build_max_heap_cnt(Block* Block_list, uint32_t size)
{
	uint32_t i;
	for (i = size / 2; i >= 0; --i)
		max_heapify_cnt(Block_list, size, 0);
}
*/

void build_max_heap(int* list, int size)
{
	int i;
	for (i = size / 2; i >= 0; --i)
		max_heapify(list, size, i);
}

/*
void max_heapify_cnt(Block* Block_list, uint32_t size_, int32_t i)
{
	int32_t l = 2 * i + 1;
	int32_t r = 2 * i + 2;
	int32_t largest;
	int32_t size = (int32_t)size_;
	
	if (l <= size - 1 && Block_list[l].cnt > Block_list[i].cnt)	largest = l;
	else largest = i;

	if (r <= size - 1 && Block_list[r].cnt > Block_list[largest].cnt)	largest = r; // if, else, if�� ������ ���� ���� ū value�� ������ index�� ã�Ƴ� ��,
	if (largest != i) {                                          // ���� �θ� ����� ���� i�� largest�� �ƴ϶�� largest�� i�� �ٲ۴�.
		//SWAP(Block_list[i].cnt, Block_list[largest].cnt);
		SWAP_BLOCK(Block_list[i], Block_list[largest]);
		max_heapify_cnt(Block_list, size, largest); // �� �Ŀ��� ��������� �ݺ�
	}
}
*/

void max_heapify(int* list, int count, int i) // max heap���� fix�ϱ�
{
	int l = 2 * i + 1;
	int r = 2 * i + 2;
	int largest;
	if (l <= count - 1 && list[l] > list[i])	largest = l;
	else largest = i;
	if (r <= count - 1 && list[r] > list[largest])	largest = r; // if, else, if�� ������ ���� ���� ū value�� ������ index�� ã�Ƴ� ��,
	if (largest != i) {                                          // ���� �θ� ����� ���� i�� largest�� �ƴ϶�� largest�� i�� �ٲ۴�.
		SWAP(list[i], list[largest]);
		max_heapify(list, count, largest); // �� �Ŀ��� ��������� �ݺ�
	}
}

void heapSort(int* list, int count)
{
	//�ϴ� build_max_heap�� ����
	int i;
	int size = count;
	for (i = count / 2; i >= 0; i--)	max_heapify(list, count, i);  // build_max_heap
																	  //
	for (i = count - 1; i > 0; i--)
	{
		SWAP(list[0], list[i]); // ���� ū ���� ���� ���� �� ��ü
		size--;					// ������ ���� heap tree���� ����
		max_heapify(list, size, 0); // �ٽ� max heap���� fix
	}
}

#endif

/*----------------------------�����Ŵ� �ʿ���� �͵�. ������ �� */
/*
 * Functions for numValid_map with MAX-HEAP 
 */

 /* Make Max-Heap with the pointers of numValid_map by numValid in blockArray */
int32_t BM_Maxheap_numValid(Block* blockArray, uint8_t* numValid_map[])
{
	uint8_t* temp_NV = (uint8_t*)malloc(sizeof(uint8_t) * NOB);

	for (int i = 0; i < NOB; ++i) {
		temp_NV[i] = blockArray[i].numValid;
	}

	BM__buildmaxheapNV(temp_NV, numValid_map); // ���� buildmaxheap���� ���� ���� �ʿ䰡 ������? �ǹ̻����δ� ������ ������ �ణ �� ��ȿ�������� ������

	free(temp_NV); // �̰� ���� temp�ϱ�? heap�� ��� �����ϰ� �����Ϸ��� �� temp_NV�� ��� �����ǰ� ���������� ����ؾ� �ϴ� �� �ƴұ�?
}


/* Build max-heap by numValid */
int32_t BM__buildmaxheapNV(uint8_t* temp_NV, uint8_t* numValid_map[])
{
	int32_t i;
	for (i = NOB / 2; i >= 0; --i) {
		BM__maxheapifyNV(temp_NV, i, numValid_map);
	}
}

int32_t BM__maxheapifyNV(uint8_t* temp_NV, int32_t i, uint8_t* numValid_map[])
{
	int32_t l = 2 * i + 1;
	int32_t r = 2 * i + 2;
	int32_t largest;

	if (l <= NOB - 1 && temp_NV[l] > temp_NV[i]) largest = l;
	else largest = i;

	if (r <= NOB - 1 && temp_NV[r] > temp_NV[largest])	largest = r;
	if (largest != i) {                                          // ���� �θ� ����� ���� i�� largest�� �ƴ϶�� largest�� i�� �ٲ۴�.
		
		SWAP_NV(temp_NV[i], temp_NV[largest]);
		SWAP_NV_PTR(numValid_map[i], numValid_map[largest]); 
		
		BM__maxheapifyNV(temp_NV, largest, numValid_map);
	}
}

/*
 * Functions for PE_map with SORT
 */

/* ������ ������ sorting�ϴ� ��������, max�� min�� �ʿ��� ���� �𸥴�. */
/* Sorting the pointers of PE_map by PE_cycle in blockArray */
int32_t BM_SortPE(Block* blockArray, uint32_t* PE_map[])
{
	//uint32_t temp_PE[NOB];
	uint32_t* temp_PE = (uint32_t*)malloc(sizeof(uint32_t) * NOB);

	for (int i = 0; i < NOB; ++i) {
		temp_PE[i] = blockArray[i].PE_cycle;
	}

	BM__quicksort(temp_PE, 0, NOB - 1, PE_map); // Need Verification of this function


	free(temp_PE);
	return(eNOERROR);
}
void BM__quicksort(uint32_t* temp_PE, int p, int r, uint32_t* PE_map[]) // If this function works, it would be maybe better to fix with 'quicksort_Optimized'
{
	int q;
	int x;
	int i;
	int j;
	if (p < r) { // partition�ϴ� �ڵ�
		x = temp_PE[r];
		i = p - 1;
		for (j = p; j < r; j++) {
			if (temp_PE[j] <= x) {
				i++; SWAP_PE(temp_PE[i], temp_PE[j]); //	PE_map�� element(pointer)�� SWAP�ϴ� �ڵ带 ���⿡ ����. 
				SWAP_PE_PTR(PE_map[i], PE_map[j]);
			}
		}
		SWAP_PE(temp_PE[i + 1], temp_PE[r]);	SWAP_PE_PTR(PE_map[i + 1], PE_map[r]);
		q = i + 1; // divide(��������� partition)
		quickSort(temp_PE, p, q - 1, PE_map); // ��� ȣ��
		quickSort(temp_PE, q + 1, r, PE_map);
	}
}