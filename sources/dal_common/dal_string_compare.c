#include "dal_common.h"


//===============================================================================================================================
bool_t	dal_string_compare(char* str1, const char* str2, uint32_t len2)
{
	while (len2 > 0)
	{
		if (*str1 != *str2)				return FALSE;
		if ((*str1 & *str2) == 0x00)	return TRUE;
		++str1;
		++str2;
		--len2;
	}

	if (len2 == 0)	return TRUE;
	else			return FALSE;
}
//===============================================================================================================================
