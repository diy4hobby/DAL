/*
 * JSON common routines
 *
 *  Created on: 17 07 2022 г.
 *      Author: a.sheviakov
 */

#ifndef JSON_COMMON_H_
#define JSON_COMMON_H_

#include "../dal_num_to_string/dal_num_to_string.h"



//===============================================================================================================================
typedef struct
{
	bool_t			pretty;
	uint8_t			indent_level;
	bool_t			array_indent;
}jsonWriteContext_t;
//===============================================================================================================================


//===============================================================================================================================
#define	MAX_STRING_LEN	4294967280

typedef enum
{
	JSON_UNKN,
	JSON_OBJECT_BEG,
	JSON_ARRAY_BEG,
	JSON_DBL,
	JSON_INT,
	JSON_BOOL,
	JSON_STR,
	JSON_NULL
}jsonReadType_e;

typedef struct jsonReadToken_t
{
	struct jsonReadToken_t*		parent;
	struct jsonReadToken_t*		child;
	struct jsonReadToken_t*		next;
	jsonReadType_e				type;
	uint32_t					child_count;
	union{
		struct{
			char*		data;
			uint32_t	len;
		}					as_str;
		bool_t				as_bool;
		int64_t				as_int;
		double				as_dbl;
	};
}jsonReadToken_t;

typedef struct
{
	jsonReadToken_t*	curr;
	uint32_t			count;
	uint8_t*			allocMemory;
}jsonReadContext_t;

typedef enum
{
	JSON_READ_OK,
	JSON_READ_UNEXPECTED_EOF,
	JSON_READ_STRING_OVERFLOW,
	JSON_READ_CHARACTER_WRONG,
	JSON_READ_TRAILING_WRONG,
	JSON_READ_TOKEN_ALLOC_ERROR,
	JSON_READ_TOKEN_APPEND_ERROR,
	JSON_READ_VALUE_UNKNOWN
}jsonReadResult_e;


#ifdef __cplusplus
extern "C" {
#endif

	jsonReadResult_e	json_to_tokens(char* json, uint32_t jsonLen, jsonReadToken_t* tokens, uint32_t tokensMax, uint32_t* tokensReaded);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif


#endif /* JSON_COMMON_H_ */
