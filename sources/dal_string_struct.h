/*
 * Data Abstraction Layer (DAL)
 *
 *  Created on: 16 07 2022 г.
 *      Author: a.sheviakov
 */

#ifndef DAL_STRING_STRUCT_H_
#define DAL_STRING_STRUCT_H_

#include "stdint.h"


//===============================================================================================================================
//Aggregate types for string - used in assignment methods for more convenient implementation
typedef struct dalStr_t
{
	const char*	data	= nullptr;
	uint32_t	size	= 0;
	explicit operator bool() const	{return (this->data != nullptr);};
}dalStr_t;
//===============================================================================================================================

#endif /* DAL_STRING_STRUCT_H_ */
