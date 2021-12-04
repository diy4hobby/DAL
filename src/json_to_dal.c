#include "json_to_dal.h"
#include "string.h"
#include "dal_helpers/dal_helpers.h"
#include "dal_core/dal_utils.h"
#include "libjson_sax/json.h"

extern dalMemHooks_t		__dal_mem_hooks;

#define TRUE		1U
#define FALSE		0U

static const int b64idx[256] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };


typedef struct
{
	dal_t*			obj;
    char			key[DAL_KEY_SIZE];
}json2dalContext_t;





int	_json2dal_deserialize(void* userdata, int type, const char* data, uint32_t length)
{
	json2dalContext_t*	ctx			= (json2dalContext_t*)userdata;

	switch (type)
	{
		case JSON_NONE:	break;
		case JSON_ARRAY_BEGIN:
			ctx->obj	= dal_add_arr(ctx->obj, ctx->key);
			ctx->key[0]	= 0x00;
			break;
		case JSON_OBJECT_BEGIN:
			if (ctx->obj == NULL)	ctx->obj	= dal_create_obj();
			else
			{	ctx->obj	= dal_add_obj(ctx->obj, ctx->key);
				ctx->key[0]	= 0x00;
			}
			break;
		case JSON_ARRAY_END:
			ctx->obj	= ctx->obj->parent;
			break;
		case JSON_OBJECT_END:
			if (ctx->obj->parent != NULL)	ctx->obj	= ctx->obj->parent;
			break;
		case JSON_INT:;
			int64_t		intVal;
			if (strToNum((char*)data, NULL, &intVal) == TRUE)	dal_add_int(ctx->obj, ctx->key, intVal);
			ctx->key[0]	= 0x00;
			break;
		case JSON_FLOAT:;
			double		dblVal;
			if (strToNum((char*)data, &dblVal, NULL) == TRUE)	dal_add_double(ctx->obj, ctx->key, dblVal);
			ctx->key[0]	= 0x00;
			break;
		case JSON_STRING:
			if (memcmp(data, "data:application/octet-stream;base64,", strlen("data:application/octet-stream;base64,")) == 0)
			{	//ѕришли бинарные данные - реобразуем их из base64 в uint8_t
				data		+= strlen("data:application/octet-stream;base64,");
				length		-= strlen("data:application/octet-stream;base64,");
				if (length % 4 != 0)
				{	data		-= strlen("data:application/octet-stream;base64,");
					length		+= strlen("data:application/octet-stream;base64,");
					goto COPY_STR;
				}
				uint32_t	binLen	= (length / 4) * 3;
				if (data[length-1] == '=')		binLen--;
				if (data[length-2] == '=')		binLen--;
				dal_t*		blobObj	= dal_add_blob(ctx->obj, ctx->key, (void*)data, binLen);
				if (blobObj != NULL)
				{
					for (uint32_t i = 0, j = 0; i < length;)
					{
						uint32_t	sextet_a	= data[i] == '=' ? 0 & i++ : b64idx[(uint8_t)data[i++]];
						uint32_t	sextet_b	= data[i] == '=' ? 0 & i++ : b64idx[(uint8_t)data[i++]];
						uint32_t	sextet_c	= data[i] == '=' ? 0 & i++ : b64idx[(uint8_t)data[i++]];
						uint32_t	sextet_d	= data[i] == '=' ? 0 & i++ : b64idx[(uint8_t)data[i++]];

						uint32_t	triple		= (sextet_a << 3 * 6) + (sextet_b << 2 * 6)
													+ (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

						if (j < binLen) blobObj->asBLOB[j++]	= (triple >> 2 * 8) & 0xFF;
						if (j < binLen) blobObj->asBLOB[j++]	= (triple >> 1 * 8) & 0xFF;
						if (j < binLen) blobObj->asBLOB[j++]	= (triple >> 0 * 8) & 0xFF;
					}
				}
				ctx->key[0]	= 0x00;
				break;
			}
COPY_STR:	dal_add_str(ctx->obj, ctx->key, (char*)data, length);
			ctx->key[0]	= 0x00;
			break;
		case JSON_KEY:
			if (length >= sizeof(ctx->key))		length	= sizeof(ctx->key) - 1;
			memcpy(ctx->key, data, length);
			ctx->key[length]	= 0x00;
			break;
		case JSON_TRUE:
			dal_add_bool(ctx->obj, ctx->key, TRUE);
			ctx->key[0]	= 0x00;
			break;
		case JSON_FALSE:
			dal_add_bool(ctx->obj, ctx->key, FALSE);
			ctx->key[0]	= 0x00;
			break;
		case JSON_NULL:
			ctx->key[0]	= 0x00;
			break;
		case JSON_BSTRING:
			dal_add_blob(ctx->obj, ctx->key, (void*)data, length);
			ctx->key[0]	= 0x00;
			break;
	}
	return DAL_OK;
}





dal_t*	json2dal_deserialize(void* buf, uint32_t len)
{
	uint32_t			stackSize	= 256;
	void*				stack		= __dal_mem_hooks.alloc_data(stackSize);
	uint32_t			parsBufSize	= 8 * 65536;
	void*				parsBuf		= __dal_mem_hooks.alloc_data(parsBufSize);
	json2dalContext_t	ctx			= {0};

	json_parser			parser		= {	.config		= {	.buffer_initial_size	= parsBufSize,
														.max_nesting			= stackSize,
														.max_data				= parsBufSize,
														.allow_c_comments		= 0,
														.allow_yaml_comments	= 0,
														.user_calloc			= NULL,
														.user_realloc			= NULL,
														.free					= NULL
														},
										.callback	= _json2dal_deserialize,
										.userdata	= &ctx,
										.state		= 0,
										.save_state	= 0,
										.expecting_key			= 0,
										.utf8_multibyte_left	= 0,
										.unicode_multi			= 0,
										.type					= 0,
										.stack					= stack,
										.stack_offset			= 0,
										.stack_size				= stackSize,
										.buffer					= parsBuf,
										.buffer_size			= parsBufSize,
										.buffer_offset			= 0
										};

	dalResult_e		result		= json_parser_string(&parser, buf, len, NULL);
	__dal_mem_hooks.free_data(stack);
	__dal_mem_hooks.free_data(parsBuf);
	if (result == 0)	return ctx.obj;
	dal_delete(ctx.obj);
	return NULL;
};


