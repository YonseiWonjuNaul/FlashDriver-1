/* BM_BLOCK */

#include "BM_Block.h"



/* PPA�� ������ �׿� �ش��ϴ� Block�� Block_list���� ��� �ִ°��� ã�� �Լ� */
int32_t BM_Find_BlockPlace_by_PPA(Block* ptrBlock[], uint32_t size, uint32_t PPA)
{
	uint32_t PBA = BM_PBA_TO_PPA(PPA);

	/* PBA�� ������ Block_list���� �˻��ϱ� */
	int32_t Block_index = BM_search_PBA(ptrBlock, size, PBA);

	// ���� �̷��� �������� �ʿ� ���� �����ϰ� return BM_search_PBA() �� �ص� �ǰ� BM_search_PBA�� ���� �Լ��� �� �ʿ䵵 �����Ű��� �ϴ�
	return Block_index;
}


/* PBA�� ������ Block_list���� �˻��ϱ� */
int32_t BM_search_PBA(Block* ptrBlock[], uint32_t size, uint32_t PBA)
{
	// Heap���� Ư�� ���� ã�� ���� �� �� �ִ� �ٸ� ����� ����? ������ linear search�ε�
	// ���ʿ� ���� ã�� ���ؼ� SORTED ARRAY�� ���� �����̾����ٵ� ��� ����? ���ʿ� Block_list�� PBA �������� ���ĵǾ��ֵ��� ���峪?
	// �Ƹ� ��� ARRAY�� PBA �������� �����ϴ� �Լ��� ������ �� �� ����.
	int32_t i = 0;
	for (; i < (int32_t)size; ++i) {
		if (ptrBlock[i]->PBA == PBA)
			return i;
	}
	ERR(ePBASEARCH_BM);
}

/* PBA �������� PBA_array�� �����ϴ� �Լ� */
/*
 * PBA
 */






/* Page Validity Bit Functions */
void BM_bitset(Block targetBlock, uint8_t offset)
{

}






/* Common useful functions for block */
void print_arr(int32_t *arr, uint32_t size)
{
	int32_t i = 0;
	for (; i < (int32_t)size; ++i)
		printf("%d ", arr[i]);
	printf("\n");
}

void print_arr2(Block* arr[], uint32_t size)
{
	for (int i = 0; i < 10; ++i) {
		printf("arr[%d]->PBA: %d\n", i, arr[i]->PBA);
		printf("arr[%d]->cnt: %d\n", i, arr[i]->cnt);
		printf("arr[%d]->PE_cycle: %d\n", i, arr[i]->PE_cycle);
	}
}

void print_arr_PBA(Block* arr[], uint32_t size)
{
	printf("[PBA]\n");
	for (int i = 0; i < (int32_t)size; ++i) {
		printf("%d ", arr[i]->PBA);
	}
	printf("\n");
}
void print_arr_bit(Block* arr[], uint32_t size)
{
	printf("[bit]\n");
	for (int i = 0; i < (int32_t)size; ++i) {
		for (int j = 0; j < 4; ++j) {
			printf("%d ", arr[i]->bit[j]);
		}
		//printf("%d ", arr[i]->bit);
	}
	printf("\n");
}void print_arr_cnt(Block* arr[], uint32_t size)
{
	printf("[cnt]\n");
	for (int i = 0; i < (int32_t)size; ++i) {
		printf("%d ", arr[i]->cnt);
	}
	printf("\n");
}void print_arr_PE(Block* arr[], uint32_t size)
{
	printf("[PE]\n");
	for (int i = 0; i < (int32_t)size; ++i) {
		printf("%d ", arr[i]->PE_cycle);
	}
	printf("\n");
}
void print_arr_all(Block* arr[], uint32_t size) // PBA, bit, cnt, P/E cycle
{
	printf("------------------------------------\n");
	printf("size: %d\n", size);
	printf("Print PBA, bit, cnt, P/E cycle..\n");
	print_arr_PBA(arr, size);
	print_arr_bit(arr, size);
	print_arr_cnt(arr, size);
	print_arr_PE(arr, size);
	printf("------------------------------------\n");
	printf("\n");
}