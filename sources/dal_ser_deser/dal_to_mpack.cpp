#include "../dal_struct.h"
#include "dal_serializer.h"
#include "mpack/mpack_common.h"

//Forward declaration of serialization function - realized in DAL library
dalResult_e	_dal_serialize_recurse(dalSerializer_t* ser, dal_t* node);


dalResult_e	dal_to_mpack_write_key(void* ctx, dalBlob_t* buf, dalStr_t* key)
{
	if (dal_mpack_write_str(&buf->data, &buf->size, key->data, key->size) != MPACK_OK)		return DAL_MEM_ERR;
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_bool(void* ctx, dalBlob_t* buf, bool value)
{
	if (dal_mpack_write_bool(&buf->data, &buf->size, value) != MPACK_OK)	return DAL_MEM_ERR;
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_uint(void* ctx, dalBlob_t* buf, uint64_t value)
{
	if (dal_mpack_write_uint(&buf->data, &buf->size, value) != MPACK_OK)	return DAL_MEM_ERR;
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_int(void* ctx, dalBlob_t* buf, int64_t value)
{
	if (dal_mpack_write_int(&buf->data, &buf->size, value) != MPACK_OK)		return DAL_MEM_ERR;
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_dbl(void* ctx, dalBlob_t* buf, double value)
{
	if (dal_mpack_write_dbl(&buf->data, &buf->size, value) != MPACK_OK)		return DAL_MEM_ERR;
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_str(void* ctx, dalBlob_t* buf, dalStr_t* value)
{
	if (dal_mpack_write_str(&buf->data, &buf->size, value->data, value->size) != MPACK_OK)		return DAL_MEM_ERR;
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_blob(void* ctx, dalBlob_t* buf, dalBlob_t* value)
{
	if (dal_mpack_write_blob(&buf->data, &buf->size, value->data, value->size) != MPACK_OK)		return DAL_MEM_ERR;
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_obj_beg(void* ctx, dalBlob_t* buf, uint32_t count)
{
	if (dal_mpack_write_map_beg(&buf->data, &buf->size, count) != MPACK_OK)		return DAL_MEM_ERR;
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_obj_end(void* ctx, dalBlob_t* buf)
{
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_arr_beg(void* ctx, dalBlob_t* buf, uint32_t count)
{
	if (dal_mpack_write_arr_beg(&buf->data, &buf->size, count) != MPACK_OK)		return DAL_MEM_ERR;
	return DAL_OK;
};

dalResult_e	dal_to_mpack_write_arr_end(void* ctx, dalBlob_t* buf)
{
	return DAL_OK;
};



//===============================================================================================================================
uint32_t	dal_t::to_mpack(uint8_t* buf, uint32_t size)
{
	dalSerializer_t	ser;
	ser.buf.data			= buf;
	ser.buf.size			= size;
	ser.context				= nullptr;
	ser.wr_key				= dal_to_mpack_write_key;
	ser.wr_bool				= dal_to_mpack_write_bool;
	ser.wr_uint				= dal_to_mpack_write_uint;
	ser.wr_int				= dal_to_mpack_write_int;
	ser.wr_dbl				= dal_to_mpack_write_dbl;
	ser.wr_str				= dal_to_mpack_write_str;
	ser.wr_blob				= dal_to_mpack_write_blob;
	ser.wr_obj_beg			= dal_to_mpack_write_obj_beg;
	ser.wr_obj_end			= dal_to_mpack_write_obj_end;
	ser.wr_arr_beg			= dal_to_mpack_write_arr_beg;
	ser.wr_arr_end			= dal_to_mpack_write_arr_end;

	dalResult_e				result	= _dal_serialize_recurse(&ser, this);
	if (result == DAL_OK)	return (size - ser.buf.size);
	else					return 0;
};
