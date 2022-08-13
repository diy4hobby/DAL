#include "../dal_struct.h"
#include "dal_serializer.h"
#include "json/json_common.h"
#include "../dal_common/dal_common.h"

//Forward declaration of serialization function - realized in DAL library
dalResult_e	_dal_serialize_recurse(dalSerializer_t* ser, dal_t* node);

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
								'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
								'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
								'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
								'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
								'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
								'w', 'x', 'y', 'z', '0', '1', '2', '3',
								'4', '5', '6', '7', '8', '9', '+', '/'};

dalResult_e	dal_to_json_write_indent(dalBlob_t* buf, uint32_t level)
{
	if (buf->size < 1)			return DAL_MEM_ERR;
	*buf->data++		= '\n';
	--buf->size;

	if (buf->size < level)		return DAL_MEM_ERR;
	for (uint8_t idx = 0; idx < level; idx++)
	{
		*buf->data++	= '\t';
		--buf->size;
	}
	return DAL_OK;
}


dalResult_e	dal_to_json_write_key(void* ctx, dalBlob_t* buf, dalStr_t* key)
{
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	if (context->pretty == TRUE)
	{
		dalResult_e		result	= dal_to_json_write_indent(buf, context->indent_level);
		if (result != DAL_OK)				return result;
	}
	if (buf->size < (key->size + 4))		return DAL_MEM_ERR;
	*buf->data++	= '"';
	--buf->size;
	void*	tmp		= const_cast<void*>(static_cast<const void*>(key->data));
	dal_bytedata_copy(buf->data, tmp, key->size);
	buf->data		+= key->size;
	buf->size		-= key->size;
	*buf->data++	= '"';
	buf->size--;
	dal_string_copy(reinterpret_cast<char*>(buf->data), ": ", 2);
	buf->data	+= 2;
	buf->size	-= 2;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_bool(void* ctx, dalBlob_t* buf, bool value)
{
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	if (context->array_indent == TRUE)
	{
		dalResult_e		result	= dal_to_json_write_indent(buf, context->indent_level);
		if (result != DAL_OK)				return result;
	}
	if (value == TRUE)
	{	//Length of "TRUE" is 4
		if (buf->size < 4)					return DAL_MEM_ERR;
		void*	data	= const_cast<void*>(static_cast<const void*>(buf->data));
		void*	text	= const_cast<void*>(static_cast<const void*>("true"));
		dal_bytedata_copy(data, text, 4);
		buf->data		+= 4;
		buf->size		-= 4;
	}else
	{	//Length of "false" is 5
		if (buf->size < 5)					return DAL_MEM_ERR;
		void*	data	= const_cast<void*>(static_cast<const void*>(buf->data));
		void*	text	= const_cast<void*>(static_cast<const void*>("false"));
		dal_bytedata_copy(data, text, 5);
		buf->data		+= 5;
		buf->size		-= 5;
	}
	*buf->data++		= ',';
	--buf->size;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_uint(void* ctx, dalBlob_t* buf, uint64_t value)
{
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	if (context->array_indent == TRUE)
	{
		dalResult_e		result	= dal_to_json_write_indent(buf, context->indent_level);
		if (result != DAL_OK)				return result;
	}
	char		strVal[24];
	uint32_t	len;
	dal_u64toa(value, strVal, &len);
	if (buf->size < len)					return DAL_MEM_ERR;
	dal_bytedata_copy(buf->data, strVal, len);
	buf->data			+= len;
	buf->size			-= len;
	*buf->data++		= ',';
	--buf->size;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_int(void* ctx, dalBlob_t* buf, int64_t value)
{
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	if (context->array_indent == TRUE)
	{
		dalResult_e		result	= dal_to_json_write_indent(buf, context->indent_level);
		if (result != DAL_OK)				return result;
	}
	char		strVal[24];
	uint32_t	len;
	dal_i64toa(value, strVal, &len);
	if (buf->size < len)					return DAL_MEM_ERR;
	dal_bytedata_copy(buf->data, strVal, len);
	buf->data			+= len;
	buf->size			-= len;
	*buf->data++		= ',';
	--buf->size;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_dbl(void* ctx, dalBlob_t* buf, double value)
{
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	if (context->array_indent == TRUE)
	{
		dalResult_e		result	= dal_to_json_write_indent(buf, context->indent_level);
		if (result != DAL_OK)				return result;
	}
	char		strVal[24];
	uint32_t	len;
	dal_dtoa(value, strVal, &len);
	if (buf->size < len)					return DAL_MEM_ERR;
	dal_bytedata_copy(buf->data, strVal, len);
	buf->data			+= len;
	buf->size			-= len;
	*buf->data++		= ',';
	--buf->size;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_str(void* ctx, dalBlob_t* buf, dalStr_t* value)
{
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	if (context->array_indent == TRUE)
	{
		dalResult_e		result	= dal_to_json_write_indent(buf, context->indent_level);
		if (result != DAL_OK)				return result;
	}
	if (buf->size < (value->size + 2))		return DAL_MEM_ERR;
	*buf->data++		= '"';
	buf->size--;
	void*	text		= const_cast<void*>(static_cast<const void*>(value->data));
	dal_bytedata_copy(buf->data, text, value->size);
	buf->data			+= value->size;
	buf->size			-= value->size;
	*buf->data++		= '"';
	--buf->size;
	*buf->data++		= ',';
	--buf->size;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_blob(void* ctx, dalBlob_t* buf, dalBlob_t* value)
{
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	if (context->array_indent == TRUE)
	{
		dalResult_e		result	= dal_to_json_write_indent(buf, context->indent_level);
		if (result != DAL_OK)				return result;
	}
	if (buf->size < (1 + 37))				return DAL_MEM_ERR;
	*buf->data++		= '\"';
	--buf->size;
	void*	bufData		= const_cast<void*>(static_cast<const void*>(buf->data));
	void*	text		= const_cast<void*>(static_cast<const void*>("data:application/octet-stream;base64,"));
	dal_bytedata_copy(bufData, text, 37);
	buf->data			+= 37;
	buf->size			-= 37;
	//Encode data in base64
	uint32_t			idx			= 0;
	uint32_t			octet_a;
	uint32_t			octet_b;
	uint32_t			octet_c;
    uint32_t			triple;
	char				encoded_data[4];
	uint32_t			len			= value->size;
	uint8_t*			data		= value->data;

	while (len > 2)
	{
		if (buf->size < 4)					return DAL_MEM_ERR;
		octet_a 		= data[idx++];
		octet_b			= data[idx++];
		octet_c 		= data[idx++];
		triple			= (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
		encoded_data[0]	= encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[1]	= encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[2]	= encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[3]	= encoding_table[(triple >> 0 * 6) & 0x3F];
		dal_bytedata_copy(buf->data, encoded_data, 4);
    	buf->data		+= 4;
    	buf->size		-= 4;
		len				-= 3;
	}

	if (len == 2)
	{
		if (buf->size < 4)					return DAL_MEM_ERR;
		octet_a 		= data[idx++];
		octet_b			= data[idx++];
		triple			= (octet_a << 0x10) + (octet_b << 0x08);
		encoded_data[0]	= encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[1]	= encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[2]	= encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[3]	= '=';
		dal_bytedata_copy(buf->data, encoded_data, 4);
    	buf->data		+= 4;
    	buf->size		-= 4;
	}else
	if (len == 1)
	{
		if (buf->size < 4)					return DAL_MEM_ERR;
		octet_a 		= data[idx++];
		triple			= (octet_a << 0x10) + (octet_b << 0x08);
		encoded_data[0]	= encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[1]	= encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[2]	= '=';
		encoded_data[3]	= '=';
		dal_bytedata_copy(buf->data, encoded_data, 4);
    	buf->data		+= 4;
    	buf->size		-= 4;
	}
	*buf->data++		= ',';
	--buf->size;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_obj_beg(void* ctx, dalBlob_t* buf, uint32_t count)
{
	if (buf->size < 1)					return DAL_MEM_ERR;
	*buf->data++		= '{';
	--buf->size;
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	context->indent_level++;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_obj_end(void* ctx, dalBlob_t* buf)
{
	if (buf->data[-1] == ',')
	{	buf->data--;
		buf->size++;
	}
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	context->indent_level--;
	if (context->pretty == TRUE)
	{
		dalResult_e		result	= dal_to_json_write_indent(buf, context->indent_level);
		if (result != DAL_OK)			return result;
	}
	*buf->data++		= '}';
	--buf->size;
	*buf->data++		= ',';
	--buf->size;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_arr_beg(void* ctx, dalBlob_t* buf, uint32_t count)
{
	if (buf->size < 1)					return DAL_MEM_ERR;
	*buf->data++		= '[';
	--buf->size;
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	context->indent_level++;
	context->array_indent	= context->pretty;
	return DAL_OK;
};

dalResult_e	dal_to_json_write_arr_end(void* ctx, dalBlob_t* buf)
{
	if (buf->data[-1] == ',')
	{	buf->data--;
		buf->size++;
	}
	jsonWriteContext_t*	context	= static_cast<jsonWriteContext_t*>(ctx);
	context->indent_level--;
	context->array_indent	= false;
	if (context->pretty == TRUE)
	{
		dalResult_e		result	= dal_to_json_write_indent(buf, context->indent_level);
		if (result != DAL_OK)			return result;
	}
	*buf->data++		= ']';
	--buf->size;
	*buf->data++		= ',';
	--buf->size;
	return DAL_OK;
};



//===============================================================================================================================
uint32_t	dal_t::to_json(uint8_t* buf, uint32_t size, bool pretty)
{
	jsonWriteContext_t	ctx;
	ctx.pretty			= pretty;
	ctx.indent_level	= 0;
	ctx.array_indent	= false;

	dalSerializer_t	ser;
	ser.buf.data		= buf;
	ser.buf.size		= size;
	ser.context			= &ctx;
	ser.wr_key			= dal_to_json_write_key;
	ser.wr_bool			= dal_to_json_write_bool;
	ser.wr_uint			= dal_to_json_write_uint;
	ser.wr_int			= dal_to_json_write_int;
	ser.wr_dbl			= dal_to_json_write_dbl;
	ser.wr_str			= dal_to_json_write_str;
	ser.wr_blob			= dal_to_json_write_blob;
	ser.wr_obj_beg		= dal_to_json_write_obj_beg;
	ser.wr_obj_end		= dal_to_json_write_obj_end;
	ser.wr_arr_beg		= dal_to_json_write_arr_beg;
	ser.wr_arr_end		= dal_to_json_write_arr_end;

	dalResult_e				result	= _dal_serialize_recurse(&ser, this);
	if (result == DAL_OK)
	{	//Remove the "ending" comma - it should not be after the top object
		ser.buf.data[-1]	= 0x00;
		ser.buf.size++;
		return (size - ser.buf.size);
	}
	else					return 0;
};
