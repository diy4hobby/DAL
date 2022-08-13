/*
 * WEBAPI common routines
 *
 *  Created on: 05 08 2022 г.
 *      Author: a.sheviakov
 */

#ifndef WEBAPI_COMMON_H_
#define WEBAPI_COMMON_H_

#include "../dal_num_to_string/dal_num_to_string.h"


//===============================================================================================================================
typedef enum
{
	WEBAPI_UNKN,
	WEBAPI_KEY,
	WEBAPI_VALUE
}webapiReadType_e;

typedef enum
{
	WEBAPI_PARENT_OBJ,
	WEBAPI_PARENT_ARR
}webapiParentType_e;

typedef struct
{
	webapiReadType_e	next_type;
	webapiParentType_e	parent_type;
	bool_t				top_obj_fill;
	uint32_t			top_obj_size;
}webapiReadContext_t;
//===============================================================================================================================




#endif /* WEBAPI_COMMON_H_ */
