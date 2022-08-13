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
typedef struct
{
	const char*	data;
	uint32_t	size;
}dalStr_t;
//===============================================================================================================================

#endif /* DAL_STRING_STRUCT_H_ */
