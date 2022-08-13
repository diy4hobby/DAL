#include "../dal_struct.h"
#include "../dal_common/dal_common.h"
#include "dal_deserializer.h"
#include "dal_string_to_num/dal_string_to_num.h"
#include "webapi/webapi_common.h"

//Forward declaration of deserilization method - realization in DAL library
dalResult_e	_dal_deserialize_recurse(uint8_t nesting, dalDeserializer_t* deser, dal_t* node);

//Read key from buffer - drop special characters and read string up to the next special character
dalResult_e	webpack_to_dal_key(uint8_t** buf, uint32_t* available, dalDeserializerValue_t* value)
{
	//Skip special characters before the key
	while ((**buf == '&') || (**buf == '?') || (**buf == ':')
			|| (**buf == '=') || (**buf == ',') || (**buf == ' '))
	{	if ((*available == 0) || (**buf == 0x00))					return DAL_FORMAT_ERR;
		++*buf;
		--*available;
	}
	value->type			= DVT_STR;
	value->as_str		= reinterpret_cast<char*>(*buf);
	while ((*available > 0) && (**buf != 0x00) && (**buf != '&')
			&& (**buf != '?') && (**buf != ':') && (**buf != '='))
	{	++*buf;
		--*available;
	}
	if ((*available == 0) || (*buf == 0x00))						return DAL_FORMAT_ERR;
	value->size			= static_cast<uint32_t>(*buf - reinterpret_cast<uint8_t*>(value->as_str));
	++*buf;	//Skip special symbol
	--*available;
	return DAL_OK;
};


dalResult_e	webpack_to_dal_read(void* ctx, uint8_t** buf, uint32_t* available, dalDeserializerValue_t* value)
{
	webapiReadContext_t*	context	= static_cast<webapiReadContext_t*>(ctx);
	dalResult_e				result	= DAL_OK;

	if (context->top_obj_fill == TRUE)
	{	value->type				= DVT_OBJECT;
		value->size				= context->top_obj_size;
		context->next_type		= WEBAPI_KEY;
		context->top_obj_fill	= false;
		return DAL_OK;
	}

	switch (context->next_type)
	{
		case WEBAPI_KEY:
			result				= webpack_to_dal_key(buf, available, value);
			//Next must be value (or object or array) for this key
			context->next_type	= WEBAPI_VALUE;
			break;

		case WEBAPI_VALUE:
			//Skip special characters before the value
			while ((**buf == ' ') || (**buf == '\r') || (**buf == '\n') || (**buf == '\t') || (**buf == ':') || (**buf == ','))
			{	if ((*available == 0) || (**buf == 0x00))					return DAL_FORMAT_ERR;
				++*buf;
				--*available;
			}

			switch (**buf)
			{
				default:{
					//Check if the next value is number
					char*		str			= reinterpret_cast<char*>(*buf);
					uint32_t	remain		= *available;
					dalStrtonumResult_t	strtonum	= dal_strntonum(&str, &remain);

					if (strtonum.int_valid == TRUE)
					{	*buf				= reinterpret_cast<uint8_t*>(str);
						*available			= remain;
						value->type			= DVT_INT;
						value->as_int		= strtonum.int_value;
						context->next_type	= (context->parent_type == WEBAPI_PARENT_ARR) ? WEBAPI_VALUE : WEBAPI_KEY;
						break;
					}
					if (strtonum.dbl_valid == TRUE)
					{	*buf				= reinterpret_cast<uint8_t*>(str);
						*available			= remain;
						value->type			= DVT_DBL;
						value->as_dbl		= strtonum.dbl_value;
						context->next_type	= (context->parent_type == WEBAPI_PARENT_ARR) ? WEBAPI_VALUE : WEBAPI_KEY;
						break;
					}
					//String is not a number - fill it as a string
					value->type			= DVT_STR;
					value->as_str		= reinterpret_cast<char*>(*buf);
					while ((*available > 0) && (**buf != 0x00) && (**buf != '&'))
					{	++*buf;
						--*available;
					}
					value->size			= static_cast<uint32_t>(*buf - reinterpret_cast<uint8_t*>(value->as_str));
					++*buf;	//Skip special symbol
					--*available;
					context->next_type	= (context->parent_type == WEBAPI_PARENT_ARR) ? WEBAPI_VALUE : WEBAPI_KEY;
					}
					break;

				case '"':
					++*buf;
					--*available;
					value->type			= DVT_STR;
					value->as_str		= reinterpret_cast<char*>(*buf);
					while ((*available > 0) && (**buf != 0x00) && (**buf != '"'))
					{	++*buf;
						--*available;
					}
					if ((*available == 0) || (*buf == 0x00))						return DAL_FORMAT_ERR;
					value->size			= static_cast<uint32_t>(*buf - reinterpret_cast<uint8_t*>(value->as_str));
					++*buf;	//Skip special symbol
					--*available;
					context->next_type	= (context->parent_type == WEBAPI_PARENT_ARR) ? WEBAPI_VALUE : WEBAPI_KEY;
					break;

				case 't':
					if (*available <= 3)	return DAL_FORMAT_ERR;
					if (((*buf)[1] != 'r') || ((*buf)[2] != 'u') || ((*buf)[3] != 'e'))	return DAL_FORMAT_ERR;
					value->type			= DVT_BOOL;
					value->as_bool		= true;
					*buf				+= 4;
					*available			-= 4;
					context->next_type	= (context->parent_type == WEBAPI_PARENT_ARR) ? WEBAPI_VALUE : WEBAPI_KEY;
					break;

				case 'T':
					if (*available <= 3)	return DAL_FORMAT_ERR;
					if (((*buf)[1] != 'R') || ((*buf)[2] != 'U') || ((*buf)[3] != 'E'))	return DAL_FORMAT_ERR;
					value->type			= DVT_BOOL;
					value->as_bool		= true;
					*buf				+= 4;
					*available			-= 4;
					context->next_type	= (context->parent_type == WEBAPI_PARENT_ARR) ? WEBAPI_VALUE : WEBAPI_KEY;
					break;

				case 'f':
					if (*available <= 4)	return DAL_FORMAT_ERR;
					if (((*buf)[1] != 'a') || ((*buf)[2] != 'l') || ((*buf)[3] != 's') || ((*buf)[4] != 'e'))	return DAL_FORMAT_ERR;
					value->type			= DVT_BOOL;
					value->as_bool		= false;
					*buf				+= 5;
					*available			-= 5;
					context->next_type	= (context->parent_type == WEBAPI_PARENT_ARR) ? WEBAPI_VALUE : WEBAPI_KEY;
					break;

				case 'F':
					if (*available <= 4)	return DAL_FORMAT_ERR;
					if (((*buf)[1] != 'A') || ((*buf)[2] != 'L') || ((*buf)[3] != 'S') || ((*buf)[4] != 'E'))	return DAL_FORMAT_ERR;
					value->type			= DVT_BOOL;
					value->as_bool		= false;
					*buf				+= 5;
					*available			-= 5;
					context->next_type	= (context->parent_type == WEBAPI_PARENT_ARR) ? WEBAPI_VALUE : WEBAPI_KEY;
					break;

				case '{': case '}':		return DAL_FORMAT_ERR;

				case '[':{
					if (context->parent_type == WEBAPI_PARENT_ARR)					return DAL_FORMAT_ERR;
					++*buf;
					--*available;
					//Skipping spaces
					while ((*available > 0) && (**buf != 0x00) && (**buf == ' '))
					{	++*buf;
						--*available;
					}
					//Quickly processing an empty array
					if (**buf == ']')
					{	value->type				= DVT_ARRAY;
						value->size				= 0;
						++*buf;	//Skip special symbol
						--*available;
						context->next_type		= WEBAPI_KEY;
						context->parent_type	= WEBAPI_PARENT_OBJ;
						break;
					}
					//Determine the number of elements in the array - count the number of commas
					char*		pos		= reinterpret_cast<char*>(*buf);
					uint32_t	size	= *available;
					uint32_t	count	= 1;
					while ((size > 0) && (*pos != 0x00) && (*pos != ']'))
					{
						if (*pos == ',')	++count;
						if (*pos == '"')
						{	++pos;
							--size;
							while ((size > 0) && (*pos != 0x00) && (*pos != '"'))
							{	++pos;
								--size;
							}
						}
						++pos;
						--size;
					}
					value->type				= DVT_ARRAY;
					value->size				= count;
					context->next_type		= WEBAPI_VALUE;
					context->parent_type	= WEBAPI_PARENT_ARR;
					}
					break;

				case ']':
					++*buf;	//Skip special symbol
					--*available;
					//After the closing array character must be either the key or the end of the file, so read the key
					result					= webpack_to_dal_key(buf, available, value);
					//Next must be value (or object or array) for this key
					context->next_type		= WEBAPI_VALUE;
					context->parent_type	= WEBAPI_PARENT_OBJ;
					break;
			}
			break;

		default:	return DAL_FORMAT_ERR;
	}

	return result;
};

//===============================================================================================================================
dalResult_e	dal_t::from_webapi(uint8_t* buf, uint32_t size)
{
	while (this->_child != nullptr)
	{
		dal_t*	next	= this->_child->_next;
		dal_delete(this->_child);
		this->_child	= next;
	}

	if (size == 0)			return DAL_OK;

	//Determine the number of child elements in the top objecr - count the number of '&'
	char*		pos			= reinterpret_cast<char*>(buf);
	uint32_t	available	= size;
	uint32_t	count		= 1;	//Because the real number of key+value mappings is always 1 more than the number of characters &
	while ((available > 0) && (*pos != 0x00))
	{	if (*pos == '&')	++count;
		if (*pos == '"')
		{	while ((available > 0) && (*pos != 0x00) && (*pos != '"'))
			{	++pos;
				--available;
			}
		}
		++pos;
		--available;
	}

	webapiReadContext_t	context;
	context.next_type		= WEBAPI_KEY;
	context.parent_type		= WEBAPI_PARENT_OBJ;
	context.top_obj_fill	= TRUE;
	context.top_obj_size	= count;

	dalDeserializer_t	deser;
	deser.buf				= buf;
	deser.available			= size;
	deser.context			= &context;
	deser.read				= webpack_to_dal_read;

	return _dal_deserialize_recurse(0, &deser, this);
};
