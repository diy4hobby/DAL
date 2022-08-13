#include "dal_common.h"


//===============================================================================================================================
bool_t	dal_bytedata_compare(uint8_t* data1, uint8_t* data2, uint32_t len1, uint32_t len2)
{
	if (len1 != len2)				return FALSE;

	if (len2 >= 8)
	{	uint64_t*	val1	= (uint64_t*)data1;
		uint64_t*	val2	= (uint64_t*)data2;
		while (len2 >= 8)
		{	if (*val1 != *val2)		return FALSE;
			val1++;
			val2++;
			len2	-= 8;
		}
		if (len2 == 0)				return TRUE;
		data1				= (uint8_t*)val1;
		data2				= (uint8_t*)val2;
	}
	if  (len2 >= 4)
	{	uint32_t*	val1	= (uint32_t*)data1;
		uint32_t*	val2	= (uint32_t*)data2;
		while (len2 >= 4)
		{	if (*val1 != *val2)		return FALSE;
			val1++;
			val2++;
			len2	-= 4;
		}
		if (len2 == 0)				return TRUE;
		data1				= (uint8_t*)val1;
		data2				= (uint8_t*)val2;
	}
	if  (len2 >= 2)
	{	uint16_t*	val1	= (uint16_t*)data1;
		uint16_t*	val2	= (uint16_t*)data2;
		while (len2 >= 2)
		{	if (*val1 != *val2)		return FALSE;
			val1++;
			val2++;
			len2	-= 2;
		}
		if (len2 == 0)				return TRUE;
		data1				= (uint8_t*)val1;
		data2				= (uint8_t*)val2;
	}
	while (len2 >= 0)
	{	if (*data1 != *data2)		return FALSE;
		data1++;
		data2++;
		len2	-= 1;
	}
	return TRUE;
}
//===============================================================================================================================
