/*
 * Data Abstraction Layer (DAL)
 *
 *  Created on: 15 07 2022 г.
 *      Author: a.sheviakov
 */

#ifndef DAL_SERIALIZER_H_
#define DAL_SERIALIZER_H_

#include "../dal_result_codes.h"
#include "../dal_string_struct.h"
#include "../dal_blob_struct.h"

//===============================================================================================================================
//Data serialization context - provided by user depending on
//from the protocol into which the data is to be serialized
typedef struct
{
	dalBlob_t			buf;
	void*				context;		//Protocol specific parameters
	dalResult_e			(*wr_key)(void* ctx, dalBlob_t* buf, dalStr_t* key);
	dalResult_e			(*wr_bool)(void* ctx, dalBlob_t* buf, bool value);
	dalResult_e			(*wr_uint)(void* ctx, dalBlob_t* buf, uint64_t value);
	dalResult_e			(*wr_int)(void* ctx, dalBlob_t* buf, int64_t value);
	dalResult_e			(*wr_dbl)(void* ctx, dalBlob_t* buf, double value);
	dalResult_e			(*wr_str)(void* ctx, dalBlob_t* buf, dalStr_t* value);
	dalResult_e			(*wr_blob)(void* ctx, dalBlob_t* buf, dalBlob_t* value);
	dalResult_e			(*wr_obj_beg)(void* ctx, dalBlob_t* buf, uint32_t count);
	dalResult_e			(*wr_obj_end)(void* ctx, dalBlob_t* buf);
	dalResult_e			(*wr_arr_beg)(void* ctx, dalBlob_t* buf, uint32_t count);
	dalResult_e			(*wr_arr_end)(void* ctx, dalBlob_t* buf);
}dalSerializer_t;
//===============================================================================================================================

#endif /* DAL_SERIALIZER_H_ */
