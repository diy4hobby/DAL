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
typedef struct dalBlob_t
{
	uint8_t*	data	= nullptr;
	uint32_t	size	= 0;
	explicit operator bool() const	{return (this->data != nullptr);};
	dalBlob_t(void* d = nullptr, uint64_t s = 0) : data(static_cast<uint8_t*>(d)), size(s) {};
}dalBlob_t;

typedef struct dalBlobRef_t
{
	uint8_t*	data	= nullptr;
	uint32_t	size	= 0;
	explicit operator bool() const	{return (this->data != nullptr);};
	dalBlobRef_t(void* d = nullptr, uint64_t s = 0) : data(static_cast<uint8_t*>(d)), size(s) {}
}dalBlobRef_t;
//===============================================================================================================================


#endif /* DAL_BLOB_STRUCT_H_ */
