#include "dal_core/dal_utils.h"
#include "dal_to_mpack.h"
#include "mpack/mpack-writer.h"


uint32_t	dal2mpack_wr_bool(void* ctx, char* key, uint8_t value)
{
	mpack_writer_t*	writer	= ctx;
	if (key != NULL)	mpack_write_cstr(writer, key);
	if (value > 0)	value	= 1;
	mpack_write_bool(writer, value);
	return DAL_OK;
};

uint32_t	dal2mpack_wr_uint(void* ctx, char* key, uint64_t value)
{
	mpack_writer_t*	writer	= ctx;
	if (key != NULL)	mpack_write_cstr(writer, key);
	mpack_write_uint(writer, value);
	return DAL_OK;
};

uint32_t	dal2mpack_wr_int(void* ctx, char* key, int64_t value)
{
	mpack_writer_t*	writer	= ctx;
	if (key != NULL)	mpack_write_cstr(writer, key);
	mpack_write_int(writer, value);
	return DAL_OK;
};

uint32_t	dal2mpack_wr_dbl(void* ctx, char* key, double value)
{
	mpack_writer_t*	writer	= ctx;
	if (key != NULL)	mpack_write_cstr(writer, key);
	mpack_write_double(writer, value);
	return DAL_OK;
};

uint32_t	dal2mpack_wr_str(void* ctx, char* key, char* value, uint32_t len)
{
	mpack_writer_t*	writer	= ctx;
	if (key != NULL)	mpack_write_cstr(writer, key);
	mpack_write_utf8_cstr(writer, value);
	return DAL_OK;
};

uint32_t	dal2mpack_wr_blob(void* ctx, char* key, void* value, uint32_t len)
{
	mpack_writer_t*	writer	= ctx;
	if (key != NULL)	mpack_write_cstr(writer, key);
	mpack_write_bin(writer, value, len);
	return DAL_OK;
};

uint32_t	dal2mpack_wr_obj_beg(void* ctx, char* key, uint32_t items)
{
	mpack_writer_t*	writer	= ctx;
	if (key != NULL)	mpack_write_cstr(writer, key);
	mpack_start_map(writer, items);
	return DAL_OK;
};

uint32_t	dal2mpack_wr_obj_end(void* ctx)
{
	mpack_writer_t*	writer	= ctx;
	mpack_finish_map(writer);
	return DAL_OK;
};

uint32_t	dal2mpack_wr_arr_beg(void* ctx, char* key, uint32_t items)
{
	mpack_writer_t*	writer	= ctx;
	if (key != NULL)	mpack_write_cstr(writer, key);
	mpack_start_array(writer, items);
	return DAL_OK;
};

uint32_t	dal2mpack_wr_arr_end(void* ctx)
{
	mpack_writer_t*	writer	= ctx;
	mpack_finish_array(writer);
	return DAL_OK;
};



dalResult_e		dal2mpack_serialize(dal_t* obj, void* buf, uint32_t bufSize, uint32_t* len)
{
	writerHooks_t	wrHooks		= {	dal2mpack_wr_bool,
									dal2mpack_wr_uint,
									dal2mpack_wr_int,
									dal2mpack_wr_dbl,
									dal2mpack_wr_str,
									dal2mpack_wr_blob,
									dal2mpack_wr_obj_beg,
									dal2mpack_wr_obj_end,
									dal2mpack_wr_arr_beg,
									dal2mpack_wr_arr_end
									};
	mpack_writer_t	writer;
	mpack_writer_init(&writer, buf, bufSize);
	dal_serialize(&writer, &wrHooks, obj);
	if (writer.error == mpack_ok)
	{	*len		= mpack_writer_buffer_used(&writer);
		mpack_writer_destroy(&writer);
		return DAL_OK;
	}else
	{	*len		= 0;
		mpack_writer_destroy(&writer);
		return DAL_MEM_ERR;
	}
};


