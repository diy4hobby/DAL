/*
 * Data Abstraction Layer (DAL)
 *
 *  Created on: 15 07 2022 г.
 *      Author: a.sheviakov
 */

#ifndef DAL_DESERIALIZER_H_
#define DAL_DESERIALIZER_H_

#include "../dal_result_codes.h"
#include "../dal_string_struct.h"
#include "../dal_blob_struct.h"


//===============================================================================================================================
typedef enum{
	DVT_UNKN		= 0x00,
	DVT_BOOL		= 0x01,
	DVT_UINT		= 0x02,
	DVT_INT			= 0x03,
	DVT_DBL			= 0x04,
	DVT_STR			= 0x05,
	DVT_BLOB		= 0x06,
	DVT_ARRAY		= 0x07,
	DVT_OBJECT		= 0x08
}dalDeserializerValueType_e;

typedef struct{
	dalDeserializerValueType_e	type;
	uint32_t					size;
	union{
		bool_t					as_bool;
		uint64_t				as_uint;
		int64_t					as_int;
		char*					as_str;
		uint8_t*				as_blob;
		double					as_dbl;
	};
}dalDeserializerValue_t;
//===============================================================================================================================


//===============================================================================================================================
//Data serialization function prototypes - provided by user depending on
//from the protocol into which the data is to be serialized
typedef struct
{
	uint8_t*			buf;
	uint32_t			available;
	void*				context;
	dalResult_e			(*read)(void* ctx, uint8_t** buf, uint32_t* available, dalDeserializerValue_t* value);
}dalDeserializer_t;
//===============================================================================================================================

#endif /* DAL_DESERIALIZER_H_ */
