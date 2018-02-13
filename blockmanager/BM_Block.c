/* BM_BLOCK */

#include "BM_Block.h"


/* Declaration of Data Structures */
Block* blockArray;
uint8_t* numValid_map[NOB];
uint32_t* PE_map[NOB];

#if 0
uint32_t BM_get(const request *req){
	block_params* params=(block_params*)malloc(sizeof(block_params));
	params->parents=req;
	params->test=-1;

	uint32_t PPA;

	algo_req *my_req=(algo_req*)malloc(sizeof(algo_req));
	my_req->end_req=block_end_req;
	my_req->params=(void*)params;

	//__block.li->pull_data(req->key,PAGESIZE,req->value,0,my_req,0);
	__block.li->pull_data(PPA, PAGESIZE, req->value, 0, my_req, 0);
}
#endif

struct algorithm __BM={
	.create=BM_create,
	.destroy=BM_destroy,
	.get=BM_get,
	.set=BM_set,
	.remove=BM_remove
};

/* Initiation of Bad-Block Manager */
int32_t BM_Init()
{
	int32_t bA_Exist;

	printf("BM_Init() Start..\n");
	printf("Start Bad-Block Manager..\n\n");
	
	printf("Read Flash..\n");
	
	/* Here.. READ FUNCTION.. */

	/* Read bA_Exist in Flash, and check whether blockArray exists or not */

	/* bA_Exist is a data of first page in the block for BM. If blockArray exists, then bA_Exist == _BA_EXIST. If not, blockArray doesn't exist */



	printf("Allocate blockArray..");	BM_InitBlock();	printf("..End\n");

	if (bA_Exist == _BA_GOODSTATE){
		/* SSD was turned off normally, so just load blockArray in Blockmanager block */ // Blockmanager block: Flash blocks for BM
		printf("In the past, SSD was normally turned off. Load blockArray..");
		BM_LoadBlock(0);	printf("..End\n");
	}
	/* If the body of 'else if' and 'else' is same, we can merge two condition to 'else' */
	else if (bA_Exist == _BA_BADSTATE){
		/* SSD was turned off abnormally, so scan all flash blocks and make blockArray */
		printf("SSD was abnormally turned off, so there is a consistancy problem.. Scan & Restore blockArray using OOB\n");
		
		
		//printf("Load blockArray..");	BM_LoadBlock(0);	printf("..End\n");
	}
	else{
		/* Turn on this SSD for the first time, so initialize blockArray */
		printf("Initialize blockArray..");
		BM_InitBlockArray();
		printf("..End\n");
	}

	printf("Bad Block Check..");	BM_BadBlockCheck();	printf("..End\n");
	printf("Fill pointer maps..");	BM_FillMap();		printf("..End\n");
	
	printf("BM_Init() End!\n");
}


#if 0
	if (bA_Exist != _BA_EXIST) {
		printf("There is no saved blockArray..\n");
		printf("Make new blockArray..\n");

		printf("\tAllocate blockArray....");	BM_InitBlock();	printf("End\n");

		printf("\tCheck whether blockArray data exists..");	BM_
		 

		printf("\tRead Data in Flash blocks....");	READ();	printf("End\n");
		printf("Finish to make blockArray!\n");
	}
	else if (bA_Exist )
	else {
		printf("There is saved blockArray..\n");
		printf("Load existing blockArray....");		BM_LoadBlock(0);	printf("End\n");
	}

	printf("Bad Block Check....");	BM_BadBlockCheck(blockArray);	printf("End\n");
	printf("Fill pointer maps....");	BM_FillMap(blockArray, numValid_map, PE_map);	printf("End\n");
	
	printf("BM_Init() End!\n");
}
#endif


/* Initialization of Block structures */
int32_t BM_InitBlock()
{
	blockArray= (Block*)malloc(sizeof(Block) * _NOB);

	
	//numValid_map = (uint8_t**)malloc(sizeof(uint8_t*) * NOB);
	//PE_map = (uint32_t**)malloc(sizeof(uint32_t*) * NOB);
	
	return(eNOERROR);
}

/* Load data to blockArray */
/*
 * Write each PBA, numValid, etc... to blockArray. 
 * ��𿡼� �ҷ��;� ����? ptr 2���� ���߿� ����� �Ŷ�� �ĵ� block �����͸� �о�ͼ� PBA, PE_cycle ���� �˾ƾ� �ϴµ�..
 */
int32_t BM_LoadBlock(uint32_t PBA_BM) /* Flash block �ϳ��� blockArray�� ��� �� �� �ֳ�? */
{
	printf("Loading Block Data..\n");
	// �ϴ� �� �Լ��� �ϼ��Ǿ��ִٰ� �����ϰ� �ٸ� �Լ��� ���� ������...



}


/* Initalize blockArray */
int32_t BM_InitBlockArray()
{
	for (int i=0; i<NOB; ++i){
		blockArray[i].PBA = PBA;
		memset(blockArray[i].ValidP, BM_VALIDPAGE, sizeof(int8_t));
		blockArray[i].numValid = _PPB;
		blockArray[i].PE_cycle = 0;
	}
}



#if 1
/* Scan data to make blockArray */
/*
 * Scan all flash blocks to fill the data of blockArray
 */
int32_t BM_ScanFlash()
{
	/* Access and Read data in all flash blocks */
	for (int i=0; i<NOB; ++i){
		BM_ReadBlock(i);

	}
}

int32_t BM_ReadBlock(int32_t PBA){
	/* (Later)Read OOB of PBA block and Fill blockArray */

	#if 0
	int8_t* temp_block = (int8_t*)malloc(_PPB * PAGESIZE);

	for (uint8_t offset = 0; offset < _PPB; ++offset){ // int offset..?
		algo_req* my_req = (algo_req*)malloc(sizeof(algo_req));
		my_req->parents = NULL;
		my_req->end_req = BM_end_req;

		//uint32_t PPA = PBA * _PPB + offset;
		__block.li->pull_data(PBA * _PPB + offset, PAGESIZE, temp_block + (offset*PAGESIZE), 0, my_req, 0); // Question: Format of pull_data to 'Block' is right?
	}
	blockArray[PBA].PBA = PBA;
	#endif
}

#endif

/* Check which blocks are bad */
int32_t BM_BadBlockCheck(Block* blockArray)
{
	/* This functions should be made with HARDWARE Function */

	for (int i = 0; i < NOB; ++i) {
		/* HARDWARE Functions to check bad state */


		blockArray[i].BAD = _BADSTATE;
	}
}

/* Fill numValid_map, PE_map from blockArray */
//int32_t BM_FillMap(Block* blockArray, uint8_t** numValid_map, uint32_t** PE_map)
int32_t BM_FillMap()
{
	for (int i = 0; i < NOB; ++i) {
		if (blockArray[i].BAD != _BADSTATE) { /* Does this condition need? Anyway we know the state of BAD.. Don't need to prevent filling maps */
			numValid_map[i] = &(blockArray[i].numValid);
			PE_map[i] = &(blockArray[i].PE_cycle);

			blockArray[i].ptrNV_data = &(numValid_map[i]);
			blockArray[i].ptrPE_data = &(PE_map[i]);
		}
	}

	return(eNOERROR);


}



/* Shutdown of Block structures */
//int32_t BM_Shutdown(Block* blockArray, uint8_t** numValid_map, uint32_t** PE_map)
int32_t BM_Shutdown();
{
	// �� ��򰡿� �����ؾ� �ϴµ�..... Flash ���� Ư�� block���� dedicated block���� �ξ �ű⿡ �ϴ� ������� �ϰ� �� �� ����.
	// �ٸ�.. ������ FTL�̳� wear-leveling ������ ������ �ʵ��� �ؾ� �� ��. ���߿� ���ǵ� ��


	/* PUSH blockArray, numValid_map, PE_map to Flash */ 
	/* Come Here! */

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