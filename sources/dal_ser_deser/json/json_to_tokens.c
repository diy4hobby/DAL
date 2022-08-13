#include "json_common.h"
#include "../dal_string_to_num/dal_string_to_num.h"


const int8_t hexValueTable[55] =
{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, // 0-0
		-1, -1, -1, -1, -1, -1, -1,
		10, 11, 12, 13, 14, 15, // A-F
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1,
		10, 11, 12, 13, 14, 15 // a-f
};

typedef struct
{
	char*				beg;
	char*				pos;
	char*				end;
}_jsonReader_t;

typedef struct {
	jsonReadToken_t*	next;
	uint32_t			allocated;
	uint32_t			count;
}_jsonTokensPool_t;


uint32_t	json_string_to_uint(char* str, uint32_t len)
{
	uint32_t	value			= 0;
	uint32_t	cutoff			= 0xFFFFFFFF / 16;
	int32_t		cutoff_digit	= (int32_t)(0xFFFFFFFF - cutoff * 16);

	for (uint32_t i = 0; i != len; ++i)
	{
		uint8_t	b				= (uint8_t)str[i];
		int32_t	digit			= (b > '0'-1 && b < 'f'+1) ? hexValueTable[b-'0'] : -1;
		if (b == -1 ||			// bad digit
			(value > cutoff) ||	// overflow
			((value == cutoff) && (digit > cutoff_digit)) )
		{
			return 0xFFFFFFFF;
		}else
		{
			value = (value * 16) + digit;
		}
	}

	return value;
}

bool_t	json_check_for_digit(char val)
{
	if ((val >= '0') && (val <= '9'))	return TRUE;
	if ((val >= 'a') && (val <= 'f'))	return TRUE;
	if ((val == '-') || (val == '+'))	return TRUE;
	if (val == 'x')						return TRUE;
	if (val == '.')						return TRUE;
	if ((val >= 'A') && (val <= 'F'))	return TRUE;
	return FALSE;
}

jsonReadToken_t*	json_alloc_token(_jsonTokensPool_t* pool, jsonReadType_e type)
{
	if (pool->allocated == pool->count)		return NULL;

	jsonReadToken_t* result		= pool->next;
	++pool->next;
	++pool->allocated;

	result->type			= type;
	result->parent			= NULL;
	result->child			= NULL;
	result->next			= NULL;
	result->child_count		= 0;

	return result;
}

jsonReadResult_e	json_append_token(jsonReadToken_t** parent, jsonReadToken_t* token)
{
	if (token == NULL)		return JSON_READ_TOKEN_APPEND_ERROR;

	if (*parent == NULL)
	{	*parent		= token;
		return JSON_READ_OK;
	}

	token->parent				= *parent;
	if ((*parent)->child == NULL)
	{	(*parent)->child		= token;
		++(*parent)->child_count;
		return JSON_READ_OK;
	}

	jsonReadToken_t*	child	= (*parent)->child;

	while (child->next != NULL)
	{
		child					= child->next;
	}

	child->next					= token;
	++child->parent->child_count;

	return JSON_READ_OK;
}

jsonReadResult_e	json_read_string(_jsonReader_t* reader, uint32_t* len)
{
	reader->beg				= reader->pos;
	char	prevChar		= '"';

	while (1)
	{
		if (reader->pos == reader->end)		return JSON_READ_UNEXPECTED_EOF;
		if (*reader->pos == 0x00)			return JSON_READ_UNEXPECTED_EOF;

		if ((*reader->pos == '\\') && (prevChar == '\\'))	++reader->pos;

		if (prevChar == '\\')
		{
			switch (*reader->pos)
			{
				case 'b':
				case 'f':
				case 'n':
				case 'r':
				case 't':	++reader->pos;	break;
				case 'u':
					{
						if (reader->end - reader->pos <= 4)		return JSON_READ_CHARACTER_WRONG;
						uint32_t	utf16cp		= json_string_to_uint(reader->pos, 4);
						if (utf16cp == 0xFFFFFFFF)				return JSON_READ_CHARACTER_WRONG;
						uint32_t	cp			= (uint16_t)(0xffff & utf16cp);
						// Is lead surrogate?
						if (cp >= 0xd800u && cp <= 0xdbffu)		return JSON_READ_CHARACTER_WRONG;
						reader->pos				+= 4;
						break;
					}
				default:
					++reader->pos;
					break;
			}
		}else	//if (prevChar == '\\')
		{
			if (*reader->pos == '"')
			{
				*len	= (uint32_t)(reader->pos - reader->beg);
				break;
			}
		}
		++reader->pos;
	}//while (1)
	return JSON_READ_OK;
}

jsonReadResult_e	json_skip_comments(_jsonReader_t* reader)
{
	++reader->pos;
	if (reader->pos == reader->end)						return JSON_READ_OK;
	if (*reader->pos == '/')
	{
		while ((*reader->pos != '\r') && (*reader->pos != '\n'))
		{	++reader->pos;
			if (reader->pos == reader->end)				return JSON_READ_OK;
		}
	}
	if (*reader->pos == '*')
	{
		while (!((*++reader->pos == '*') && (*reader->pos == '/')))
		{	++reader->pos;
			if (reader->pos == reader->end)				return JSON_READ_OK;
		}
	}
	return JSON_READ_OK;
}




jsonReadResult_e	json_to_tokens(char* json, uint32_t jsonLen, jsonReadToken_t* tokens, uint32_t tokensMax, uint32_t* tokensReaded)
{
	jsonReadResult_e	result	= JSON_READ_OK;

	if ((jsonLen >= 3)
		&& (json[0] == 0xEF)
		&& (json[1] == 0xBB)
		&& (json[2] == 0xBF))
	{
		json	+= 3;
		jsonLen	-= 3;
	}

	_jsonReader_t		reader;
	reader.beg						= json;
	reader.pos						= json;
	reader.end						= json + jsonLen;
	_jsonTokensPool_t	pool;
	pool.next						= tokens;
	pool.allocated					= 0;
	pool.count						= tokensMax;
	jsonReadToken_t*	token		= NULL;
	jsonReadToken_t*	child		= NULL;

	for ( ; reader.pos < reader.end; ++reader.pos)
	{
		switch (*reader.pos)
		{
			case ' ': case '\r': case '\n': case '\t': case ':':
				continue;


			case '{':
				child			= json_alloc_token(&pool, JSON_OBJECT_BEG);
				if (child == NULL)						return JSON_READ_TOKEN_ALLOC_ERROR;
				result			= json_append_token(&token, child);
				if (result != JSON_READ_OK)				return result;
				token			= child;
				continue;

			case '[':
				child			= json_alloc_token(&pool, JSON_ARRAY_BEG);
				if (child == NULL)						return JSON_READ_TOKEN_ALLOC_ERROR;
				result			= json_append_token(&token, child);
				if (result != JSON_READ_OK)				return result;
				token			= child;
				continue;

			case '}':
				if (token == NULL)						return JSON_READ_CHARACTER_WRONG;
				else	token	= token->parent;
				continue;

			case ']':
				if (token == NULL)						return JSON_READ_CHARACTER_WRONG;
				else	token	= token->parent;
				continue;

			case 't':
				if ((reader.end - reader.pos) <= 3)		return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[1] != 'r')				return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[2] != 'u')				return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[3] != 'e')				return JSON_READ_VALUE_UNKNOWN;
				child			= json_alloc_token(&pool, JSON_BOOL);
				if (child == NULL)						return JSON_READ_TOKEN_ALLOC_ERROR;
				result			= json_append_token(&token, child);
				if (result != JSON_READ_OK)				return result;
				child->as_bool	= TRUE;
				reader.pos	+= 4;
				continue;

			case 'f':
				if ((reader.end - reader.pos) <= 4)		return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[1] != 'a')				return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[2] != 'l')				return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[3] != 's')				return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[4] != 'e')				return JSON_READ_VALUE_UNKNOWN;
				child			= json_alloc_token(&pool, JSON_BOOL);
				if (child == NULL)						return JSON_READ_TOKEN_ALLOC_ERROR;
				result			= json_append_token(&token, child);
				if (result != JSON_READ_OK)				return result;
				child->as_bool	= FALSE;
				reader.pos	+= 5;
				continue;

			case 'n':
				if ((reader.end - reader.pos) <= 3)		return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[1] != 'u')				return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[2] != 'l')				return JSON_READ_VALUE_UNKNOWN;
				if (reader.pos[3] != 'l')				return JSON_READ_VALUE_UNKNOWN;
				child			= json_alloc_token(&pool, JSON_NULL);
				if (child == NULL)						return JSON_READ_TOKEN_ALLOC_ERROR;
				result			= json_append_token(&token, child);
				if (result != JSON_READ_OK)				return result;
				reader.pos	+= 4;
				continue;

			case '"':
				++reader.pos;
				child					= json_alloc_token(&pool, JSON_STR);
				if (child == NULL)						return JSON_READ_TOKEN_ALLOC_ERROR;
				result					= json_append_token(&token, child);
				if (result != JSON_READ_OK)				return result;
				child->as_str.data		= reader.pos;
				result					= json_read_string(&reader, &child->as_str.len);
				if (result != JSON_READ_OK)				return result;
				continue;

			case '/':	//Skip comments - lines and blocks
				++reader.pos;
				result					= json_skip_comments(&reader);
				if (result != JSON_READ_OK)				return result;
				continue;

			default:{
				//Check if the next value is number
				uint32_t	len			= (uint32_t)(reader.end - reader.pos);
				dalStrtonumResult_t	strtonum	= dal_strntonum(&reader.pos, &len);
				if (strtonum.int_valid == TRUE)
				{	child				= json_alloc_token(&pool, JSON_INT);
					if (child == NULL)					return JSON_READ_TOKEN_ALLOC_ERROR;
					result				= json_append_token(&token, child);
					if (result != JSON_READ_OK)			return result;
					child->as_int		= strtonum.int_value;
					continue;
				}
				if (strtonum.dbl_valid == TRUE)
				{	child				= json_alloc_token(&pool, JSON_DBL);
					if (child == NULL)					return JSON_READ_TOKEN_ALLOC_ERROR;
					result				= json_append_token(&token, child);
					if (result != JSON_READ_OK)			return result;
					child->as_dbl		= strtonum.dbl_value;
					continue;
				}
				switch (*reader.pos)
				{
					case 0x00:										return JSON_READ_UNEXPECTED_EOF;
					case ',': case '}': case ']':					break;
					case ' ': case '\r': case '\n': case '\t':		break;
					default:	++reader.pos;						continue;
				}
				}
		}
	}

	if (tokensReaded != NULL)	*tokensReaded	= pool.allocated;
	return JSON_READ_OK;
};




