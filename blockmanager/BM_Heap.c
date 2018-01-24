/* BM_HEAP */

#include "BM_Heap.h"

/*
 * Heap Functions for ptrBlock
 */



void max_heapify_cnt_(Block* ptrBlock[], uint32_t size_, int32_t i)
{
	int32_t l = 2 * i + 1;
	int32_t r = 2 * i + 2;
	int32_t largest;
	int32_t size = (int32_t)size_;

	if (l <= size - 1 && ptrBlock[l]->cnt > ptrBlock[i]->cnt)	largest = l;
	else largest = i;

	if (r <= size - 1 && ptrBlock[r]->cnt > ptrBlock[largest]->cnt)	largest = r; // if, else, if�� ������ ���� ���� ū value�� ������ index�� ã�Ƴ� ��,
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



