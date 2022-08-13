#include "../dal_struct.h"
#include "../dal_common/dal_common.h"
#include "dal_deserializer.h"
#include "dal_string_to_num/dal_string_to_num.h"
#include "json/json_common.h"


//Declaration of external dynamic memory allocation and freeing functions
//- initialized during DAL initialization
extern	dalMemHooks_t		_dal_memHooks;

//Forward declaration of deserilization method - realization in DAL library
dalResult_e	_dal_deserialize_recurse(uint8_t nesting, dalDeserializer_t* deser, dal_t* node);



static const int b64idx[256] =
{	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
	56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
	7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
	0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

uint8_t* _dal_string_to_blob(char* data, uint32_t len)
{
	uint32_t	blobLen		= (len / 4) * 3;
	if (data[len - 1] == '=')	blobLen--;
	if (data[len - 2] == '=')	blobLen--;

	uint8_t*	blob		= static_cast<uint8_t*>(_dal_memHooks.alloc_data(blobLen));
	if (blob == nullptr)		return nullptr;

	uint32_t	i			= 0;
	uint32_t	j			= 0;
	while (i < len)
	{
		uint32_t	sextet_a	= data[i] == '=' ? 0 & i++ : b64idx[(uint8_t)data[i++]];
		uint32_t	sextet_b	= data[i] == '=' ? 0 & i++ : b64idx[(uint8_t)data[i++]];
		uint32_t	sextet_c	= data[i] == '=' ? 0 & i++ : b64idx[(uint8_t)data[i++]];
		uint32_t	sextet_d	= data[i] == '=' ? 0 & i++ : b64idx[(uint8_t)data[i++]];
		uint32_t	triple		= (sextet_a << 3 * 6) + (sextet_b << 2 * 6)
									+ (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

		if (j < blobLen)	blob[j++]	= (triple >> 2 * 8) & 0xFF;
		if (j < blobLen)	blob[j++]	= (triple >> 1 * 8) & 0xFF;
		if (j < blobLen)	blob[j++]	= (triple >> 0 * 8) & 0xFF;
	}
	return blob;
}



dalResult_e	json_to_dal_read(void* ctx, uint8_t** buf, uint32_t* available, dalDeserializerValue_t* value)
{
	jsonReadContext_t*	context		= static_cast<jsonReadContext_t*>(ctx);
	//Release previously allocated memory - it could be used, for example, for BLOB
	if (context->allocMemory != nullptr)
	{	_dal_memHooks.free_data(context->allocMemory);
		context->allocMemory		= nullptr;
	}

	switch (context->curr->type)
	{
		default:	return DAL_FORMAT_ERR;

		case JSON_OBJECT_BEG:
			value->type		= DVT_OBJECT;
			//Divide by two, because components of a key:value pair are counted separately
			value->size		= context->curr->child_count / 2;
			context->curr++;
			break;

		case JSON_ARRAY_BEG:
			value->type		= DVT_ARRAY;
			value->size		= context->curr->child_count;
			context->curr++;
			break;

		case JSON_DBL:
			value->type		= DVT_DBL;
			value->as_dbl	= context->curr->as_dbl;
			context->curr++;
			break;

		case JSON_INT:
			value->type		= DVT_INT;
			value->as_int	= context->curr->as_int;
			context->curr++;
			break;

		case JSON_BOOL:
			value->type		= DVT_BOOL;
			value->as_bool	= context->curr->as_bool;
			context->curr++;
			break;

		case JSON_STR:{
			//37 = strlen("data:application/octet-stream;base64,")
			if ((context->curr->as_str.len - 37) % 4 != 0)
			{	value->type		= DVT_STR;
				value->as_str	= context->curr->as_str.data;
				value->size		= context->curr->as_str.len;
				context->curr++;
				return DAL_OK;
			}
			bool	isBlob	= dal_string_compare(context->curr->as_str.data, "data:application/octet-stream;base64,", 37);
			if (isBlob == false)
			{	value->type		= DVT_STR;
				value->as_str	= context->curr->as_str.data;
				value->size		= context->curr->as_str.len;
				context->curr++;
				return DAL_OK;
			}else
			{	context->curr->as_str.data	+= 37;
				context->curr->as_str.len	-= 37;
				uint8_t*	blob		= _dal_string_to_blob(context->curr->as_str.data, context->curr->as_str.len);
				if (blob == nullptr)		return DAL_MEM_ERR;
				value->type				= DVT_BLOB;
				value->as_blob			= blob;
				context->allocMemory	= blob;
				context->curr++;
			}
			}
			break;
	}

	return DAL_OK;
};



//===============================================================================================================================
dalResult_e	dal_t::from_json(uint8_t* buf, uint32_t size)
{
	while (this->_child != nullptr)
	{
		dal_t*	next	= this->_child->_next;
		dal_delete(this->_child);
		this->_child	= next;
	}

	uint32_t			tokensMax	= 1024;
	jsonReadToken_t*	tokens		= static_cast<jsonReadToken_t*>(_dal_memHooks.alloc_data(tokensMax * sizeof(jsonReadToken_t)));

	uint32_t			tokensReaded= 0;
	jsonReadResult_e	readResult	= json_to_tokens(reinterpret_cast<char*>(buf), size, tokens, tokensMax, &tokensReaded);
	if (readResult != JSON_READ_OK)
	{	_dal_memHooks.free_data(tokens);
		return DAL_FORMAT_ERR;
	}

	jsonReadContext_t	context;
	context.curr			= tokens;
	context.count			= tokensReaded;
	context.allocMemory		= nullptr;

	dalDeserializer_t	deser;
	deser.buf				= buf;
	deser.available			= size;
	deser.context			= &context;
	deser.read				= json_to_dal_read;

	dalResult_e			result		= _dal_deserialize_recurse(0, &deser, this);

	_dal_memHooks.free_data(tokens);
	if (context.allocMemory != nullptr)	_dal_memHooks.free_data(context.allocMemory);

	return result;
};
