/* BM_BLOCK */

#include "BM_Block.h"


/* Declaration of Data Structures */
Block* blockArray;
uint8_t* numValid_map[NOB];
uint32_t* PE_map[NOB];

/* Initiation of Bad-Block Manager */
int32_t BM_Init()
{
	printf("Start Bad-Block Manager..\n");
	printf("BM_InitBlock..\n");		BM_InitBlock();
	printf("BM_LoadBlock..\n");		BM_LoadBlock(0);
	printf("BM_FillMap..\n");		BM_FillMap(blockArray, numValid_map, PE_map);

}

/* Initialization of Block structures */
int32_t BM_InitBlock()
{
	blockArray= (Block*)malloc(sizeof(Block) * NOB);

	
	//numValid_map = (uint8_t**)malloc(sizeof(uint8_t*) * NOB);
	//PE_map = (uint32_t**)malloc(sizeof(uint32_t*) * NOB);
	
	return(eNOERROR);
}

/* Load data to blockArray */
/*
 * Write each PBA, numValid, etc... to blockArray. 
 * ��𿡼� �ҷ��;� ����? ptr 2���� ���߿� ����� �Ŷ�� �ĵ� block �����͸� �о�ͼ� PBA, PE_cycle ���� �˾ƾ� �ϴµ�..
 */
int32_t BM_LoadBlock(int32_t somewhere)
{
	printf("Loading Block Data..\n");
	// �ϴ� �� �Լ��� �ϼ��Ǿ��ִٰ� �����ϰ� �ٸ� �Լ��� ���� ������...

}


/* Fill numValid_map, PE_map from blockArray */
int32_t BM_FillMap(Block* blockArray, uint8_t** numValid_map, uint32_t** PE_map)
{
	for (int i = 0; i < NOB; ++i) {
		numValid_map[i] = &(blockArray[i].numValid);
		PE_map[i] = &(blockArray[i].PE_cycle);

		blockArray[i].ptrNV_data = &(numValid_map[i]);
		blockArray[i].ptrPE_data = &(PE_map[i]);
	}

	return(eNOERROR);


}

/* Check which blocks are bad */
int32_t BM_BadBlockCheck(Block* blockArray)
{
	/* This functions should be made with HARDWARE Function */

}


/* Shutdown of Block structures */
int32_t BM_Shutdown(Block* blockArray, uint8_t** numValid_map, uint32_t** PE_map)
{
	// �� ��򰡿� �����ؾ� �ϴµ�..... Flash ���� Ư�� block���� dedicated block���� �ξ �ű⿡ �ϴ� ������� �ϰ� �� �� ����.
	// �ٸ�.. ������ FTL�̳� wear-leveling ������ ������ �ʵ��� �ؾ� �� ��. ���߿� ���ǵ� ��

	free(blockArray);
	//free(numValid_map);
	//free(PE_map);
}






#if 0
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
#endif // 0












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
		printf("arr[%d]->cnt: %d\n", i, arr[i]->numValid);
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
void print_arr_ValidP(Block* arr[], uint32_t size)
{
	printf("[ValidP]\n");
	for (int i = 0; i < (int32_t)size; ++i) {
		for (int j = 0; j < 4; ++j) {
			printf("%d ", arr[i]->ValidP[j]);
		}
		//printf("%d ", arr[i]->ValidP);
	}
	printf("\n");
}void print_arr_cnt(Block* arr[], uint32_t size)
{
	printf("[cnt]\n");
	for (int i = 0; i < (int32_t)size; ++i) {
		printf("%d ", arr[i]->numValid);
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
void print_arr_all(Block* arr[], uint32_t size) // PBA, ValidP, cnt, P/E cycle
{
	printf("------------------------------------\n");
	printf("size: %d\n", size);
	printf("Print PBA, ValidP, cnt, P/E cycle..\n");
	print_arr_PBA(arr, size);
	print_arr_ValidP(arr, size);
	print_arr_cnt(arr, size);
	print_arr_PE(arr, size);
	printf("------------------------------------\n");
	printf("\n");
}