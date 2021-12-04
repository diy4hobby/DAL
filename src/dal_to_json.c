#include "dal_to_json.h"
#include "string.h"
#include "limits.h"
#include "dal_core/dal_utils.h"
#include "dal_helpers/dal_helpers.h"
#include "libjson_sax/json.h"

#define				NULL			((void *)0)

typedef struct
{
	char*			buf;
    uint32_t		bufSize;
    uint32_t		dataLen;
    json_printer	writer;
    uint8_t			minimize;
}dal2jsonContext_t;





dalResult_e	dal2json_checkSpace(dal2jsonContext_t* ctx, uint32_t size)
{	if (size > (ctx->bufSize - ctx->dataLen))		return DAL_MEM_ERR;
	return DAL_OK;
}

int	dal2json_writer(void* userdata, const char* s, uint32_t len)
{
	dal2jsonContext_t*	wrBuff	= userdata;
	if (dal2json_checkSpace(wrBuff, len) != DAL_OK)	return JSON_ERROR_NO_MEMORY;
	memcpy(wrBuff->buf, s, len);
	wrBuff->dataLen				+= len;
	wrBuff->buf					+= len;
	return 0;
};



uint32_t	dal2json_wr_bool(void* ctx, char* key, uint8_t value)
{
	dal2jsonContext_t*	wrCtx	= ctx;
	if (key != NULL)	json_print(&wrCtx->writer, JSON_KEY, key, strlen(key), wrCtx->minimize);
	if (value == 0)		json_print(&wrCtx->writer, JSON_FALSE, "", 0, wrCtx->minimize);
	else				json_print(&wrCtx->writer, JSON_TRUE, "", 0, wrCtx->minimize);
	return DAL_OK;
};

uint32_t	dal2json_wr_uint(void* ctx, char* key, uint64_t value)
{
	dal2jsonContext_t*	wrCtx	= ctx;
	if (key != NULL)	json_print(&wrCtx->writer, JSON_KEY, key, strlen(key), wrCtx->minimize);
	char		strVal[24];
	uint32_t	len;
	if (value > UINT32_MAX)		u64toa(value, strVal, &len);
	else						u32toa((uint32_t)value, strVal, &len);
	json_print(&wrCtx->writer, JSON_INT, strVal, len, wrCtx->minimize);
	return DAL_OK;
};

uint32_t	dal2json_wr_int(void* ctx, char* key, int64_t value)
{
	dal2jsonContext_t*	wrCtx	= ctx;
	if (key != NULL)	json_print(&wrCtx->writer, JSON_KEY, key, strlen(key), wrCtx->minimize);
	char		strVal[24];
	uint32_t	len;
	if ((value < INT32_MIN) || (value > INT32_MAX))		i64toa(value, strVal, &len);
	else												i32toa((int32_t)value, strVal, &len);
	json_print(&wrCtx->writer, JSON_INT, strVal, len, wrCtx->minimize);
	return DAL_OK;
};

uint32_t	dal2json_wr_dbl(void* ctx, char* key, double value)
{
	dal2jsonContext_t*	wrCtx	= ctx;
	if (key != NULL)	json_print(&wrCtx->writer, JSON_KEY, key, strlen(key), wrCtx->minimize);
	char		strVal[24];
	uint32_t	len;
	dtoa(value, strVal, &len);
	json_print(&wrCtx->writer, JSON_FLOAT, strVal, len, wrCtx->minimize);
	return DAL_OK;
};

uint32_t	dal2json_wr_str(void* ctx, char* key, char* value, uint32_t len)
{
	dal2jsonContext_t*	wrCtx	= ctx;
	if (key != NULL)	json_print(&wrCtx->writer, JSON_KEY, key, strlen(key), wrCtx->minimize);
	json_print(&wrCtx->writer, JSON_STRING, value, len, wrCtx->minimize);
	return DAL_OK;
};


static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

uint32_t	dal2json_wr_blob(void* ctx, char* key, void* value, uint32_t len)
{
	dal2jsonContext_t*	wrCtx		= ctx;
	if (key != NULL)	json_print(&wrCtx->writer, JSON_KEY, key, strlen(key), wrCtx->minimize);
	wrCtx->writer.callback(ctx,  "\"", 1);
	wrCtx->writer.callback(ctx,  "data:application/octet-stream;base64,", 37);
	//Кодируем данные в base64
	uint32_t			idx			= 0;
	uint32_t			octet_a;
	uint32_t			octet_b;
	uint32_t			octet_c;
    uint32_t			triple;
	char				encoded_data[4];
	while (len > 2)
	{
		octet_a 		= ((uint8_t*)value)[idx++];
		octet_b			= ((uint8_t*)value)[idx++];
		octet_c 		= ((uint8_t*)value)[idx++];
        triple			= (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
        encoded_data[0]	= encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[1]	= encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[2]	= encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[3]	= encoding_table[(triple >> 0 * 6) & 0x3F];
        wrCtx->writer.callback(ctx,  encoded_data, 4);
		len					-= 3;
	}
	if (len == 2)
	{	octet_a 		= ((uint8_t*)value)[idx++];
		octet_b			= ((uint8_t*)value)[idx++];
		triple			= (octet_a << 0x10) + (octet_b << 0x08);
		encoded_data[0]	= encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[1]	= encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[2]	= encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[3]	= '=';
		wrCtx->writer.callback(ctx,  encoded_data, 4);
	}else
	{	octet_a 		= ((uint8_t*)value)[idx++];
		triple			= (octet_a << 0x10) + (octet_b << 0x08);
		encoded_data[0]	= encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[1]	= encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[2]	= '=';
		encoded_data[3]	= '=';
		wrCtx->writer.callback(ctx,  encoded_data, 4);
	}
	wrCtx->writer.callback(ctx,  "\",", 2);
	if (wrCtx->minimize == 0)
	{
		wrCtx->writer.callback(ctx, "\n", 1);
		for (uint8_t i = 0; i < wrCtx->writer.indentlevel; i++)
			wrCtx->writer.callback(ctx, wrCtx->writer.indentstr, strlen(wrCtx->writer.indentstr));
	}
	return DAL_OK;
};

uint32_t	dal2json_wr_obj_beg(void* ctx, char* key, uint32_t items)
{
	dal2jsonContext_t*	wrCtx	= ctx;
	if (key != NULL)	json_print(&wrCtx->writer, JSON_KEY, key, strlen(key), wrCtx->minimize);
	json_print(&wrCtx->writer, JSON_OBJECT_BEGIN, NULL, 0, wrCtx->minimize);
	return DAL_OK;
};

uint32_t	dal2json_wr_obj_end(void* ctx)
{
	dal2jsonContext_t*	wrCtx	= ctx;
	json_print(&wrCtx->writer, JSON_OBJECT_END, NULL, 0, wrCtx->minimize);
	return DAL_OK;
};

uint32_t	dal2json_wr_arr_beg(void* ctx, char* key, uint32_t items)
{
	dal2jsonContext_t*	wrCtx	= ctx;
	if (key != NULL)	json_print(&wrCtx->writer, JSON_KEY, key, strlen(key), wrCtx->minimize);
	json_print(&wrCtx->writer, JSON_ARRAY_BEGIN, NULL, 0, wrCtx->minimize);
	return DAL_OK;
};

uint32_t	dal2json_wr_arr_end(void* ctx)
{
	dal2jsonContext_t*	wrCtx	= ctx;
	json_print(&wrCtx->writer, JSON_ARRAY_END, NULL, 0, wrCtx->minimize);
	return DAL_OK;
};


dalResult_e		dal2json_serialize(dal_t* obj, void* buf, uint32_t bufSize, uint32_t* len, uint8_t minimize)
{
	dal2jsonContext_t	wrCtx	= {	.buf = buf, .bufSize = bufSize, .dataLen = 0, .minimize = minimize};
	json_print_init(&wrCtx.writer, dal2json_writer, &wrCtx);

	writerHooks_t		wrHooks		= {	dal2json_wr_bool,
									dal2json_wr_uint,
									dal2json_wr_int,
									dal2json_wr_dbl,
									dal2json_wr_str,
									dal2json_wr_blob,
									dal2json_wr_obj_beg,
									dal2json_wr_obj_end,
									dal2json_wr_arr_beg,
									dal2json_wr_arr_end
									};

	dal_serialize(&wrCtx, &wrHooks, obj);
	if (len != NULL)	*len		= wrCtx.dataLen;

	json_print_free(&wrCtx.writer);
	return DAL_OK;
};


