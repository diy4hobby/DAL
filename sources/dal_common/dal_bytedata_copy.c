#include "dal_common.h"
#ifdef ENABLE_MEMCPY	//defined in "dal_definitions.h"
	#include "string.h"
#endif // ENABLE_MEMCPY



//===============================================================================================================================
void	dal_bytedata_copy(void* data1, void* data2, uint32_t len)
{
#ifdef ENABLE_MEMCPY
	memcpy(data1, data2, len);
#else
	if (len >= 8)
	{	uint64_t*	val1	= (uint64_t*)data1;
		uint64_t*	val2	= (uint64_t*)data2;
		while (len >= 8)
		{	*val1 	= *val2;
			val1++;
			val2++;
			len		-= 8;
		}
		if (len == 0)				return;
		data1				= (uint8_t*)val1;
		data2				= (uint8_t*)val2;
	}
	if  (len >= 4)
	{	uint32_t*	val1	= (uint32_t*)data1;
		uint32_t*	val2	= (uint32_t*)data2;
		while (len >= 4)
		{	*val1 	= *val2;
			val1++;
			val2++;
			len		-= 4;
		}
		if (len == 0)				return;
		data1				= (uint8_t*)val1;
		data2				= (uint8_t*)val2;
	}
	if  (len >= 2)
	{	uint16_t*	val1	= (uint16_t*)data1;
		uint16_t*	val2	= (uint16_t*)data2;
		while (len >= 2)
		{	*val1 	= *val2;
			val1++;
			val2++;
			len		-= 2;
		}
		if (len == 0)				return;
		data1				= (uint8_t*)val1;
		data2				= (uint8_t*)val2;
	}
	if (len > 0)
	{	uint8_t*	val1	= (uint8_t*)data1;
		uint8_t*	val2	= (uint8_t*)data2;
		*val1 		= *val2;
	}
#endif
}
//===============================================================================================================================
