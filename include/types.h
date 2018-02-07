#ifndef __H_TYPES__
#define __H_TYPES__

#define FS_GET_T 0
#define FS_SET_T 1
#define FS_DELETE_T 2 
#define FS_AGAIN_R_T 3

typedef enum{
	block_bad,
	block_full,
	block_empty,
	block_he
}lower_status;
#endif
