#include "dal_common.h"


//===============================================================================================================================
void	dal_string_copy(char* dst, const char* src, uint32_t maxLen)
{
	char*	tmp		= (char*)src;

	while ((*tmp != 0x00) && (maxLen > 0))
	{	*dst		= *tmp;
		++dst;
		++tmp;
		--maxLen;
	};
}
//===============================================================================================================================
