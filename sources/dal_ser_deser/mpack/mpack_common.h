/*
 * MPACK common routines
 *
 *  Created on: 25 06 2022 г.
 *      Author: a.sheviakov
 */

#ifndef MPACK_COMMON_H_
#define MPACK_COMMON_H_
#include "../../dal_common/dal_common.h"



//===============================================================================================================================
//Result codes for library methods execution
typedef enum
{
	MPACK_OK				= 0x00,
	MPACK_BUF_OVERFLOW		= 0x01,
	MPACK_FORMAT_ERR		= 0x03,
}mpackResult_e;
//===============================================================================================================================


//===============================================================================================================================
//Methods for writing and reading data of a fixed size, taking into account the network byte order
#ifdef __cplusplus
extern "C" {
#endif

inline uint8_t mpack_read_u8(uint8_t** data, uint32_t* available)
{
	uint8_t	val			= *data[0];
	*data				+= sizeof(uint8_t);
	*available			-= sizeof(uint8_t);
	return val;
}

inline uint16_t mpack_read_u16(uint8_t** data, uint32_t* available)
{
	uint16_t	val		= (((uint16_t)(*data)[0]) << 8) | (uint16_t)(*data)[1];
	*data				+= sizeof(uint16_t);
	*available			-= sizeof(uint16_t);
	return val;
}

inline uint32_t mpack_read_u32(uint8_t** data, uint32_t* available)
{
	uint32_t	val		= (((uint32_t)(*data)[0]) << 24) | (((uint32_t)(*data)[1]) << 16)
							| (((uint32_t)(*data)[2]) << 8) | (uint32_t)(*data)[3];
	*data				+= sizeof(uint32_t);
	*available			-= sizeof(uint32_t);
	return val;
}

inline uint64_t mpack_read_u64(uint8_t** data, uint32_t* available)
{
	uint64_t	val		= (((uint64_t)(*data)[0]) << 56) | (((uint64_t)(*data)[1]) << 48)
							| (((uint64_t)(*data)[2]) << 40) | (((uint64_t)(*data)[3]) << 32)
							| (((uint64_t)(*data)[4]) << 24) | (((uint64_t)(*data)[5]) << 16)
							| (((uint64_t)(*data)[6]) << 8) | (uint64_t)(*data)[7];
	*data				+= sizeof(uint64_t);
	*available			-= sizeof(uint64_t);
	return val;
}

inline mpackResult_e mpack_write_u8(uint8_t** data, uint32_t* available, uint8_t val)
{
	if (*available < sizeof(uint8_t))		return MPACK_BUF_OVERFLOW;
	(*data)[0]			= val;
	*data				+= sizeof(uint8_t);
	*available			-= sizeof(uint8_t);
	return MPACK_OK;
}

inline mpackResult_e mpack_write_u16(uint8_t** data, uint32_t* available, uint16_t val)
{
	if (*available < sizeof(uint16_t))		return MPACK_BUF_OVERFLOW;
	(*data)[0]			= (val >> 8) & 0xFF;
	(*data)[1]			= val & 0xFF;
	*data				+= sizeof(uint16_t);
	*available			-= sizeof(uint16_t);
	return MPACK_OK;
}

inline mpackResult_e mpack_write_u32(uint8_t** data, uint32_t* available, uint32_t val)
{
	if (*available < sizeof(uint32_t))		return MPACK_BUF_OVERFLOW;
	(*data)[0]			= (val >> 24) & 0xFF;
	(*data)[1]			= (val >> 16) & 0xFF;
	(*data)[2]			= (val >> 8) & 0xFF;
	(*data)[3]			= val & 0xFF;
	*data				+= sizeof(uint32_t);
	*available			-= sizeof(uint32_t);
	return MPACK_OK;
}

inline mpackResult_e mpack_write_u64(uint8_t** data, uint32_t* available, uint64_t val)
{
	if (*available < sizeof(uint64_t))		return MPACK_BUF_OVERFLOW;
	(*data)[0]			= (val >> 56) & 0xFF;
	(*data)[1]			= (val >> 48) & 0xFF;
	(*data)[2]			= (val >> 40) & 0xFF;
	(*data)[3]			= (val >> 32) & 0xFF;
	(*data)[4]			= (val >> 24) & 0xFF;
	(*data)[5]			= (val >> 16) & 0xFF;
	(*data)[6]			= (val >> 8) & 0xFF;
	(*data)[7]			= val & 0xFF;
	*data				+= sizeof(uint64_t);
	*available			-= sizeof(uint64_t);
	return MPACK_OK;
}

inline int8_t  mpack_read_i8 (uint8_t** data, uint32_t* available) {return (int8_t) mpack_read_u8 (data, available);}
inline int16_t mpack_read_i16(uint8_t** data, uint32_t* available) {return (int16_t)mpack_read_u16(data, available);}
inline int32_t mpack_read_i32(uint8_t** data, uint32_t* available) {return (int32_t)mpack_read_u32(data, available);}
inline int64_t mpack_read_i64(uint8_t** data, uint32_t* available) {return (int64_t)mpack_read_u64(data, available);}
inline mpackResult_e mpack_write_i8 (uint8_t** data, uint32_t* available, int8_t  val) {return mpack_write_u8 (data, available, (uint8_t) val);}
inline mpackResult_e mpack_write_i16(uint8_t** data, uint32_t* available, int16_t val) {return mpack_write_u16(data, available, (uint16_t)val);}
inline mpackResult_e mpack_write_i32(uint8_t** data, uint32_t* available, int32_t val) {return mpack_write_u32(data, available, (uint32_t)val);}
inline mpackResult_e mpack_write_i64(uint8_t** data, uint32_t* available, int64_t val) {return mpack_write_u64(data, available, (uint64_t)val);}

inline float mpack_read_float(uint8_t** data, uint32_t* available)
{
	static_assert(sizeof(float) == sizeof(uint32_t), "float size not as expected");
	union
	{	float		f;
		uint32_t	u;
	}val;
	val.u 				= mpack_read_u32(data, available);
	return val.f;
}

inline double mpack_read_double(uint8_t** data, uint32_t* available)
{
	static_assert(sizeof(double) == sizeof(uint64_t), "double size not as expected");
	union
	{	double		d;
		uint64_t	u;
	}val;
	val.u				= mpack_read_u64(data, available);
	return val.d;
}

inline mpackResult_e mpack_write_float(uint8_t** data, uint32_t* available, float value)
{
	static_assert(sizeof(float) == sizeof(uint32_t), "float size not as expected");
	union
	{	float		f;
		uint32_t	u;
	}val;
    val.f				= value;
    return mpack_write_u32(data, available, val.u);
}

inline mpackResult_e mpack_write_double(uint8_t** data, uint32_t* available, double value)
{
	static_assert(sizeof(double) == sizeof(uint64_t), "double size not as expected");
	union
	{	double		d;
		uint64_t	u;
	}val;
	val.d				= value;
	return mpack_write_u64(data, available, val.u);
}

#ifdef __cplusplus
} // closing brace for extern "C"
#endif
//===============================================================================================================================



//===============================================================================================================================
//The maximum size of a tag in bytes
#define		MPACK_TAG_SIZE_MAX			9

//Sizes in bytes of a different tags
#define		MPACK_TAG_SIZE_FIXUINT		1
#define		MPACK_TAG_SIZE_U8			2
#define		MPACK_TAG_SIZE_U16			3
#define		MPACK_TAG_SIZE_U32			5
#define		MPACK_TAG_SIZE_U64			9
#define		MPACK_TAG_SIZE_FIXINT		1
#define		MPACK_TAG_SIZE_I8			2
#define		MPACK_TAG_SIZE_I16			3
#define		MPACK_TAG_SIZE_I32			5
#define		MPACK_TAG_SIZE_I64			9
#define		MPACK_TAG_SIZE_FLOAT		5
#define		MPACK_TAG_SIZE_DOUBLE		9
#define		MPACK_TAG_SIZE_FIXARRAY		1
#define		MPACK_TAG_SIZE_ARRAY16		3
#define		MPACK_TAG_SIZE_ARRAY32		5
#define		MPACK_TAG_SIZE_FIXMAP		1
#define		MPACK_TAG_SIZE_MAP16		3
#define		MPACK_TAG_SIZE_MAP32		5
#define		MPACK_TAG_SIZE_FIXSTR		1
#define		MPACK_TAG_SIZE_STR8			2
#define		MPACK_TAG_SIZE_STR16		3
#define		MPACK_TAG_SIZE_STR32		5
#define		MPACK_TAG_SIZE_BIN8			2
#define		MPACK_TAG_SIZE_BIN16		3
#define		MPACK_TAG_SIZE_BIN32		5
#define		MPACK_TAG_SIZE_FIXEXT1		2
#define		MPACK_TAG_SIZE_FIXEXT2		2
#define		MPACK_TAG_SIZE_FIXEXT4		2
#define		MPACK_TAG_SIZE_FIXEXT8		2
#define		MPACK_TAG_SIZE_FIXEXT16		2
#define		MPACK_TAG_SIZE_EXT8			3
#define		MPACK_TAG_SIZE_EXT16		4
#define		MPACK_TAG_SIZE_EXT32		6
//===============================================================================================================================



//===============================================================================================================================
#ifdef __cplusplus
extern "C" {
#endif

	mpackResult_e	mpack_write_str(uint8_t** buf, uint32_t* available, const char* data, uint32_t dataLen);
	mpackResult_e	mpack_write_nil(uint8_t** buf, uint32_t* available);
	mpackResult_e	mpack_write_bool(uint8_t** buf, uint32_t* available, bool_t value);
	mpackResult_e	mpack_write_uint(uint8_t** buf, uint32_t* available, uint64_t value);
	mpackResult_e	mpack_write_int(uint8_t** buf, uint32_t* available, int64_t value);
	mpackResult_e	mpack_write_flt(uint8_t** buf, uint32_t* available, float value);
	mpackResult_e	mpack_write_dbl(uint8_t** buf, uint32_t* available, double value);
	mpackResult_e	mpack_write_blob(uint8_t** buf, uint32_t* available, uint8_t* value, uint32_t len);
	mpackResult_e	mpack_write_map_beg(uint8_t** buf, uint32_t* available, uint32_t count);
	mpackResult_e	mpack_write_map_end(uint8_t** buf, uint32_t* available);
	mpackResult_e	mpack_write_arr_beg(uint8_t** buf, uint32_t* available, uint32_t count);
	mpackResult_e	mpack_write_arr_end(uint8_t** buf, uint32_t* available);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif
//===============================================================================================================================





#endif /* MPACK_COMMON_H_ */
