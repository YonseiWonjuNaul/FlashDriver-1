/* Badblock Manager */
#include "BM_Interface.h"

#define METHOD 1	// Which method has better performance?

// �⺻ 4���� �������̽�

int32_t		BM_invalidate_ppa(Block* blockArray, uint32_t PPA)
{
	PBA_T PBA = BM_PPA_TO_PBA(PPA);
	uint8_t offset = PPA % PPB;

#if (METHOD == 1)
	blockArray[PBA].bit[offset] = BM_INVALIDPAGE;
	return (eNOERROR);
#endif

#if (METHOD == 2)
	if (blockArray[PBA].bit[offset] == BM_INVALIDPAGE) {
		printf("Input PPA is already INVALID\n");
		return (eNOERROR);
	}
	else {
		blockArray[PBA].bit[offset] = BM_INVALIDPAGE;
		return (eNOERROR);
	}
#endif
}
int32_t		BM_is_invalid_ppa(Block* blockArray, uint32_t PPA) // ptrBlock�� ��� ������ �ξ�� ����? �ϴ� �� parameter�� ����
{
	// parameter�� ���� PPA�� VALID���� INVALID���� ��ȯ�ϴ� �Լ��ε�
	// 1bit�� �ϰ� �ص� �ǰ����� status�� VALID INVALID �ܿ� �� �������� �����Ƿ� �ϴ� char�� ��ȯ

	PBA_T PBA = BM_PPA_TO_PBA(PPA);
	uint8_t offset = PPA % PPB;


	/* if - else if should be switched if invalid page is more than valid page */
	if (blockArray[PBA].bit[offset] == BM_VALIDPAGE) {
		printf("Input PPA is VALID\n");
		return (0);
	}
	else if (blockArray[PBA].bit[offset] == BM_INVALIDPAGE) {
		printf("Input PPA is UNVALID\n");
		return (1);
	}
	else {
		printf("Error!\n");
		ERR(eBADVALIDPAGE_BM);
	}


}
uint32_t	BM_get_gc_victim(Block* blockArray, uint8_t* numValid_map[])
{
	/* victim block�� PBA�� ��ȯ�ϴ� �Լ� */
	/*
	* Parameter: Array(Heap) of numValid pointer(numValid_map)
	* Parameter�� Heap���� �־����Ƿ�, Heap ������ �̿��Ͽ� numValid�� max�� node��(max heap�� root) ã�Ƽ� �� PBA�� ��ȯ�Ѵ�.
	*/


	/* After this function, numValid_map will become Max-heap by numValid */
	BM_Maxheap_numValid(blockArray, numValid_map);

	/* Make Block_pointer from numValid_pointer */
	void* ptr_max_nV_block = numValid_map[0] - sizeof(bit_T)*NOP - sizeof(PBA_T);

	return *((PBA_T*)ptr_max_nV_block); // This means value of PBA of maxnV block

}
uint32_t	BM_get_worn_block(Block *blockArray, uint32_t* PE_map[])
{
	/* PE_map�� PE_cycle ������� Ascending order sorting�ϴ� �Լ� */
	/*@
	 * Parameter: Array of PE_cycle pointer(PE_map)
	 * PE_map�� �����ϸ� 
	 * ���� Flash ���� data�� SWAP�ϴ� ������ ���� �� ��
	 */

	/* � ������ �ұ�? 
	1. PE_cycle ������� sorting�ؼ� �� ������� �ƿ� PBA�� �ٲ������ 
	2. ���� PE_cycle�� ���� block�� ���� PE_cycle�� ���� block�� �����͸� ��ȯ�ϱ�(Heap���� �ϴ� �� ��������)(���� �� �� Badblock�� �ƴ� �ɷ�)
	*/
	

	/* ������ ���� */
	/*
	 * wear-leveling�� ���� �Ͼ�� �۾��̰�, �츮 ���������� wear-leveling ��ü�� �ƴϴϱ� �����ϰ� �����ϵ��� �Ѵ�.
	 * free block pool�� ���� ��, �� allocation ����� PE_cycle�� ���� ���� block�� return�ؼ� �ϴ� �������
	 * max�� min�� ���̰� ��� ���� �Ͼ �� wear-leveling�� �Ͼ�ų� ��..
	 */
	// �׷��� free block pool�� ��� ��������? �������ΰ�?

	return 0;
}