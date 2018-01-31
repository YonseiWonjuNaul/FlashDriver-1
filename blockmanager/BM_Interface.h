#ifndef _BM_INTERFACE_H_
#define _BM_INTERFACE_H_

#include "BM_common.h"
#include "BM_Block.h"
#include "BM_Heap.h"


/* parameter�� ���� PPA�� �ش��ϴ� Page�� ���¸� INVALID�� �ٲٴ� �Լ� */
int32_t		BM_invalidate_ppa(Block* blockArray, uint32_t PPA);


/* parameter�� ���� PPA�� valid���� invalid���� Ȯ���Ͽ� ��ȯ�ϴ� �Լ�*/
int32_t		BM_is_invalid_ppa(Block* blockArray, uint32_t PPA);


/*
 * GC victim block�� PBA�� ��ȯ�ϴ� �Լ�
 * numValid_map���� numValid(�� block�� valid page�� ����)�� ���� �����͵��� ���������, numValid�� ���� Max-heap���� ���ĵ� ��
 * numValid�� ���� ū block�� GC victim���� �����Ͽ� PBA�� ��ȯ��
 */
uint32_t	BM_get_gc_victim(Block* blockArray, uint8_t* numValid_map[]);



/* ���� ��ü������ �������� ���Ͽ� �������� ���� ����
    PE_cycle�� ���� ���� block�� return�Ͽ� free block pool�� �Ҵ��ϰų� PE_cycle ������ �����س��� ���� ����� �� �� ����
    �����̿� ������ �ʿ䰡 ���� ����
	*/
uint32_t	BM_get_worn_block(Block *blockArray, uint32_t* PE_map[]);



/* GC�� ������ �ҷ��� �ϴ� �Լ�*/


#endif // !_BM_INTERFACE_H_
