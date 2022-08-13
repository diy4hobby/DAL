/*
 * Data Abstraction Layer (DAL)
 *
 *  Created on: 08 06 2022 г.
 *      Author: a.sheviakov
 */

#ifndef DAL_BLOB_STRUCT_H_
#define DAL_BLOB_STRUCT_H_

#include "stdint.h"


//===============================================================================================================================
//Aggregate types for binary data - used in assignment methods for more convenient implementation
typedef struct
{
	uint8_t*	data;
	uint32_t	size;
}dalBlob_t;

typedef struct
{
	uint8_t*	data;
	uint32_t	size;
}dalBlobRef_t;
//===============================================================================================================================


#endif /* DAL_BLOB_STRUCT_H_ */
