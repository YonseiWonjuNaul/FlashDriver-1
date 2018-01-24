/* Badblock Manager */
#include "BM_Interface.h"


#define numBlock 10		// '���� ������ Block�� ����' �� ��ȯ�ؾ� �Ѵ�. ��� �� �� ã�ƺ���.

// �⺻ 4���� �������̽�

void		BM_invalidate_ppa(Block* ptrBlock[], uint32_t PPA)
{
	// array�� ���� invalid ���θ� ǥ��
	// parameter�� ���� PPA�� VALID���� INVALID�� �ٲٴ� �Լ��ε�]
	
	/*
	 * ��.. �̰�.... parameter�� ���� �ؾߵ���??? �⺻������ ptrBlock�� � ������ ���ĵǾ�����? ���� PPA ������ ���ĵǾ��ִٸ� PPA�� �ٷ�
	 * index ������ �� �ִµ�.. ptrBlock[PPA] �̷���.. �׷��� ���� �׷��� ���ĵǾ����� �ʴٸ�
	 * PPA�� ���� �ϴ� �װ� ptrBlock�� �� ��° index�� �ִ��� �˻��Ͽ� ã�ƾ� �Ѵ�.. �׷��� �˻��ϸ� ������ ��... 
	 */

}
int32_t		BM_is_invalid_ppa(Block* ptrBlock[], uint32_t size, uint32_t PPA) // ptrBlock�� ��� ������ �ξ�� ����? �ϴ� �� parameter�� ����
{
	// parameter�� ���� PPA�� VALID���� INVALID���� ��ȯ�ϴ� �Լ��ε�
	// 1bit�� �ϰ� �ص� �ǰ����� status�� VALID INVALID �ܿ� �� �������� �����Ƿ� �ϴ� char�� ��ȯ

	int32_t Block_index = BM_Find_BlockPlace_by_PPA(ptrBlock, size, PPA);	/* Index of ptrBlock[] corresponded to input PPA */
	uint8_t offset = PPA % PPB;						/* Page offset of input PPA(Range: 0~255) */
	Block* targetBlock = ptrBlock[Block_index];		/* Pointer of Target Block */
	uint8_t index;									/* bit index (0,1,2,3) */
	uint64_t targetBit;								/* Validity of parameter PPA(1 means VALID, 0 means INVALID) */


	/* Find the offset(index) of target bit */
	if (offset < 128) {		
		if (offset > 64) { index = 1; }
		else if (offset > 0) { index = 0; }
		else { ERR(eBADOFFSET_BM); }
	}
	else {
		if (offset < 192) { index = 2; }
		else if (offset < 256) { index = 3; }
		else { ERR(eBADOFFSET_BM); }
	}
	offset = offset % 64;	/* Example: page offset 253 becomes 253-192 = 61 */


	/* 
	 * Now, Range of offset is 0~63 
	 * targetBit indicates 'Validity of parameter PPA'. 1 means VALID, 0 means INVALID
	 */
	targetBit = targetBlock->bit[index] << 64 - (offset + 1); 
	targetBit = targetBit >> 63;								// Didn't merge two lines for more readability
	//�̰Ŵ� �׳� validity Ȯ���� ���ε�.. �� bit�� �� �ٲٷ��� OR ���� �ؾ��ҵ�?

	if (!targetBit)
		return 1; // INVALID
	else
		return 0; // VALID


}
uint32_t	BM_get_gc_victim(Block *ptrBlock[], uint32_t size)
{
	/* victim block�� PBA�� ��ȯ�ϴ� �Լ� */
	/* 
	 * Parameter: Array(Heap) of Block structure
	 * Parameter�� Heap���� �־����Ƿ�, Heap ������ �̿��Ͽ� cnt(P/E)�� max�� node��(max heap�� root) ã�Ƽ� �� PPA(PBA?)�� ��ȯ�Ѵ�.
	 * �ƴ� PPA�� �ƴ϶� Block ��ü�� ��ȯ�ϴ� �ǰ�?
	 */

	/* Make Max-heap by invalid count, cnt */
	//build_max_heap_cnt(Block_list, numBlock); // numBlock ���� ��¥ size �־��..
	build_max_heap_cnt_(ptrBlock, size);

	// ��... cnt �������� �ٲ۴� �ѵ�.. �ܼ��� SWAP�ϴ� �� �ƴ϶� 'cnt�� ���� ���� block�� �ش��ϴ� PBA'�� ã�ƾ� �Ѵ�. cnt array�� PBA array�� index�� �ٸ���!!
	// ������ �ȵǾ��ִµ� ���ľ� �Ѵ�..

	/* 0th offset of cnt array is the block of maximum cnt.. but is it corresponding PBA? */
	return ptrBlock[0]->PBA;

}
uint32_t	BM_get_weared_block(Block *ptrBlock[], uint32_t size)
{
	// �̰� ����
	return 0;
}