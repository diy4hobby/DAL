#include "dal_common.h"


//===============================================================================================================================
void	dal_bytedata_zero(void* dst, uint32_t len)
{
	if (len >= 8)
	{	uint64_t*	dst64	= (uint64_t*)dst;
		while (len >= 8)
		{	*dst64 	= 0;
			dst64++;
			len		-= 8;
		}
		if (len == 0)				return;
		dst					= (void*)dst64;
	}
	if  (len >= 4)
	{	uint32_t*	dst32	= (uint32_t*)dst;
		while (len >= 4)
		{	*dst32 	= 0;
			dst32++;
			len		-= 4;
		}
		if (len == 0)				return;
		dst					= (void*)dst32;
	}
	if  (len >= 2)
	{	uint16_t*	dst16	= (uint16_t*)dst;
		while (len >= 2)
		{	*dst16 	= 0;
			dst16++;
			len		-= 2;
		}
		if (len == 0)				return;
		dst					= (void*)dst16;
	}
	if (len > 0)
	{	uint8_t*	dst8	= (uint8_t*)dst;
		*dst8 		= 0;
	}
}
//===============================================================================================================================
