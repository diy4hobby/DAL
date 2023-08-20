#include "mpack_common.h"


//===============================================================================================================================
mpackResult_e	dal_mpack_write_str(uint8_t** buf, uint32_t* available, const char* data, uint32_t dataLen)
{
	if (dataLen <= 31)
	{	//fixstr stores a byte array whose length is upto 31 bytes:
		//+--------+========+
		//|101XXXXX|  data  |
		//+--------+========+
		if (*available < (dataLen + MPACK_TAG_SIZE_FIXSTR))		return MPACK_BUF_OVERFLOW;
		dal_mpack_write_u8(buf, available, 0xa0 | dataLen);
	}else
	if (dataLen <= 255)
	{	//str 8 stores a byte array whose length is upto (2^8)-1 bytes:
		//+--------+--------+========+
		//|  0xd9  |YYYYYYYY|  data  |
		//+--------+--------+========+
		if (*available < (dataLen + MPACK_TAG_SIZE_STR8))		return MPACK_BUF_OVERFLOW;
		dal_mpack_write_u8(buf, available, 0xd9);
		dal_mpack_write_u8(buf, available, dataLen);
	}else
	if (dataLen <= 65535)
	{	//str 16 stores a byte array whose length is upto (2^16)-1 bytes:
		//+--------+--------+--------+========+
		//|  0xda  |ZZZZZZZZ|ZZZZZZZZ|  data  |
		//+--------+--------+--------+========+
		if (*available < (dataLen + MPACK_TAG_SIZE_STR16))		return MPACK_BUF_OVERFLOW;
		dal_mpack_write_u8(buf, available, 0xda);
		dal_mpack_write_u16(buf, available, dataLen);
	}else
	{	//str 32 stores a byte array whose length is upto (2^32)-1 bytes:
		//+--------+--------+--------+--------+--------+========+
		//|  0xdb  |AAAAAAAA|AAAAAAAA|AAAAAAAA|AAAAAAAA|  data  |
		//+--------+--------+--------+--------+--------+========+
		if (*available < (dataLen + MPACK_TAG_SIZE_STR32))		return MPACK_BUF_OVERFLOW;
		dal_mpack_write_u8(buf, available, 0xdb);
		dal_mpack_write_u32(buf, available, dataLen);
	}

	dal_bytedata_copy(*buf, (void*)data, dataLen);
	*buf				+= dataLen;
	*available			-= dataLen;
	return MPACK_OK;
}

mpackResult_e	dal_mpack_write_nil(uint8_t** buf, uint32_t* available)
{
	return dal_mpack_write_u8(buf, available, 0xc0);
}

mpackResult_e	dal_mpack_write_bool(uint8_t** buf, uint32_t* available, bool_t value)
{
	if (value == TRUE)	return dal_mpack_write_u8(buf, available, 0xc3);
	else				return dal_mpack_write_u8(buf, available, 0xc2);
}

mpackResult_e	dal_mpack_write_uint(uint8_t** buf, uint32_t* available, uint64_t value)
{
	mpackResult_e	result;

	if (value <= 127)
	{	//positive fixint stores 7-bit positive integer
		//+--------+
		//|0XXXXXXX|
		//+--------+
		result		= dal_mpack_write_u8(buf, available, (uint8_t)value);
		return result;
	}

	if (value <= 255)
	{	//uint 8 stores a 8-bit unsigned integer
		//+--------+--------+
		//|  0xcc  |ZZZZZZZZ|
		//+--------+--------+
		result		= dal_mpack_write_u8(buf, available, 0xcc);
		if (result != MPACK_OK)		return result;
		return dal_mpack_write_u8(buf, available, (uint8_t)value);
	}

	if (value <= 65535)
	{	//uint 16 stores a 16-bit big-endian unsigned integer
		//+--------+--------+--------+
		//|  0xcd  |ZZZZZZZZ|ZZZZZZZZ|
		//+--------+--------+--------+
		result		= dal_mpack_write_u8(buf, available, 0xcd);
		if (result != MPACK_OK)		return result;
		return dal_mpack_write_u16(buf, available, (uint16_t)value);
	}

	if (value <= 4294967295)
	{	//uint 32 stores a 32-bit big-endian unsigned integer
		//+--------+--------+--------+--------+--------+
		//|  0xce  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
		//+--------+--------+--------+--------+--------+
		result		= dal_mpack_write_u8(buf, available, 0xce);
		if (result != MPACK_OK)		return result;
		return dal_mpack_write_u32(buf, available, (uint32_t)value);
	}

	//uint 64 stores a 64-bit big-endian unsigned integer
	//+--------+--------+--------+--------+--------+--------+--------+--------+--------+
	//|  0xcf  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
	//+--------+--------+--------+--------+--------+--------+--------+--------+--------+
	result		= dal_mpack_write_u8(buf, available, 0xcf);
	if (result != MPACK_OK)		return result;
	return dal_mpack_write_u64(buf, available, value);
}

mpackResult_e	dal_mpack_write_int(uint8_t** buf, uint32_t* available, int64_t value)
{
	mpackResult_e	result;

	if (value >= -32)
	{
		if (value < 0)
		{
			result		= dal_mpack_write_i8(buf, available, (int8_t)value);
			return result;
		}

		return dal_mpack_write_uint(buf, available, value);
	}

	if (value >= -128)
	{	//int 8 stores a 8-bit signed integer
		//+--------+--------+
		//|  0xd0  |ZZZZZZZZ|
		//+--------+--------+
		result		= dal_mpack_write_u8(buf, available, 0xd0);
		if (result != MPACK_OK)		return result;
		return dal_mpack_write_i8(buf, available, (int8_t)value);
	}

	if (value >= -32768)
	{	//int 16 stores a 16-bit big-endian signed integer
		//+--------+--------+--------+
		//|  0xd1  |ZZZZZZZZ|ZZZZZZZZ|
		//+--------+--------+--------+
		result		= dal_mpack_write_u8(buf, available, 0xd1);
		if (result != MPACK_OK)		return result;
		return dal_mpack_write_i16(buf, available, (int16_t)value);
	}

	if (value >= (-2147483647 - 1))
	{	//int 32 stores a 32-bit big-endian signed integer
		//+--------+--------+--------+--------+--------+
		//|  0xd2  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
		//+--------+--------+--------+--------+--------+
		result		= dal_mpack_write_u8(buf, available, 0xd2);
		if (result != MPACK_OK)		return result;
		return dal_mpack_write_i32(buf, available, (int32_t)value);
	}

	//int 64 stores a 64-bit big-endian signed integer
	//+--------+--------+--------+--------+--------+--------+--------+--------+--------+
	//|  0xd3  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
	//+--------+--------+--------+--------+--------+--------+--------+--------+--------+
	result		= dal_mpack_write_u8(buf, available, 0xd3);
	if (result != MPACK_OK)		return result;
	return dal_mpack_write_i64(buf, available, value);
}

mpackResult_e	dal_mpack_write_flt(uint8_t** buf, uint32_t* available, float value)
{
	mpackResult_e	result;
	result		= dal_mpack_write_u8(buf, available, 0xca);
	if (result != MPACK_OK)		return result;
	return dal_mpack_write_float(buf, available, value);
}

mpackResult_e	dal_mpack_write_dbl(uint8_t** buf, uint32_t* available, double value)
{
	mpackResult_e	result;
	result		= dal_mpack_write_u8(buf, available, 0xcb);
	if (result != MPACK_OK)		return result;
	return dal_mpack_write_double(buf, available, value);
}

mpackResult_e	dal_mpack_write_blob(uint8_t** buf, uint32_t* available, uint8_t* value, uint32_t len)
{
	mpackResult_e	result;

	if (len <= 255)
	{	//bin 8 stores a byte array whose length is upto (2^8)-1 bytes:
		//+--------+--------+========+
		//|  0xc4  |XXXXXXXX|  data  |
		//+--------+--------+========+
		result		= dal_mpack_write_u8(buf, available, 0xc4);
		if (result != MPACK_OK)		return result;
		result		= dal_mpack_write_u8(buf, available, len);
		if (result != MPACK_OK)		return result;
	}else
	if (len <= 65535)
	{	//bin 16 stores a byte array whose length is upto (2^16)-1 bytes:
		//+--------+--------+--------+========+
		//|  0xc5  |YYYYYYYY|YYYYYYYY|  data  |
		//+--------+--------+--------+========+
		result		= dal_mpack_write_u8(buf, available, 0xc5);
		if (result != MPACK_OK)		return result;
		result		= dal_mpack_write_u16(buf, available, len);
		if (result != MPACK_OK)		return result;
	}else
	if (len <= 4294967295)
	{	//bin 32 stores a byte array whose length is upto (2^32)-1 bytes:
		//+--------+--------+--------+--------+--------+========+
		//|  0xc6  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|  data  |
		//+--------+--------+--------+--------+--------+========+
		result		= dal_mpack_write_u8(buf, available, 0xc6);
		if (result != MPACK_OK)		return result;
		result		= dal_mpack_write_u32(buf, available, len);
		if (result != MPACK_OK)		return result;
	}else
	{
		return MPACK_BUF_OVERFLOW;
	}

	if (*available < len)		return MPACK_BUF_OVERFLOW;

	dal_bytedata_copy(*buf, value, len);
	*buf		+= len;
	*available	-= len;
	return MPACK_OK;
}

mpackResult_e	dal_mpack_write_map_beg(uint8_t** buf, uint32_t* available, uint32_t count)
{
	mpackResult_e	result;

	if (count <= 15)
	{	//fixmap stores a map whose length is upto 15 elements
		//+--------+~~~~~~~~~~~~~~~~~+
		//|1000XXXX|   N*2 objects   |
		//+--------+~~~~~~~~~~~~~~~~~+
		return dal_mpack_write_u8(buf, available, 0x80 | count);
	}

	if (count <= 65535)
	{	//map 16 stores a map whose length is upto (2^16)-1 elements
		//+--------+--------+--------+~~~~~~~~~~~~~~~~~+
		//|  0xde  |YYYYYYYY|YYYYYYYY|   N*2 objects   |
		//+--------+--------+--------+~~~~~~~~~~~~~~~~~+
		result		= dal_mpack_write_u8(buf, available, 0xde);
		if (result != MPACK_OK)		return result;
		return dal_mpack_write_u16(buf, available, count);
	}

	//map 32 stores a map whose length is upto (2^32)-1 elements
	//+--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
	//|  0xdf  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|   N*2 objects   |
	//+--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
	result		= dal_mpack_write_u8(buf, available, 0xdf);
	if (result != MPACK_OK)		return result;
	return dal_mpack_write_u32(buf, available, count);
}

mpackResult_e	dal_mpack_write_map_end(uint8_t** buf, uint32_t* available)
{
	return MPACK_OK;
}

mpackResult_e	dal_mpack_write_arr_beg(uint8_t** buf, uint32_t* available, uint32_t count)
{
	mpackResult_e	result;

	if (count <= 15)
	{	//fixarray stores an array whose length is upto 15 elements:
		//+--------+~~~~~~~~~~~~~~~~~+
		//|1001XXXX|    N objects    |
		//+--------+~~~~~~~~~~~~~~~~~+
		return dal_mpack_write_u8(buf, available, 0x90 | count);
	}

	if (count <= 65535)
	{	//array 16 stores an array whose length is upto (2^16)-1 elements:
		//+--------+--------+--------+~~~~~~~~~~~~~~~~~+
		//|  0xdc  |YYYYYYYY|YYYYYYYY|    N objects    |
		//+--------+--------+--------+~~~~~~~~~~~~~~~~~+
		result		= dal_mpack_write_u8(buf, available, 0xdc);
		if (result != MPACK_OK)		return result;
		return dal_mpack_write_u16(buf, available, count);
	}

	//array 32 stores an array whose length is upto (2^32)-1 elements:
	//+--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
	//|  0xdd  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|    N objects    |
	//+--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
	result		= dal_mpack_write_u8(buf, available, 0xdd);
	if (result != MPACK_OK)		return result;
	return dal_mpack_write_u32(buf, available, count);
}

mpackResult_e	dal_mpack_write_arr_end(uint8_t** buf, uint32_t* available)
{
	return MPACK_OK;
}
//===============================================================================================================================








