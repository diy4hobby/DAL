#include "dal_common.h"
/*
typedef union {
	void*		ptr_val;
	uint64_t	uint_val;
}_dal_align_ptr_8_u;
*/

//===============================================================================================================================
void*	dal_pointer_align_8(void* ptr)
{
/*
	_dal_align_ptr_8_u	alignUnion;
	alignUnion.ptr_val		= ptr;
	alignUnion.uint_val		= (alignUnion.uint_val + 0x07) & ~(uint64_t)0x07;
	return alignUnion.ptr_val;
*/
	return (void*)(((uint64_t)ptr + 0x07)  & ~(uint64_t)0x07);
}
//===============================================================================================================================
