#include "../dal_struct.h"
#include "dal_deserializer.h"
#include "mpack/mpack_common.h"

//Forward declaration of deserilization method - realization in DAL library
dalResult_e	_dal_deserialize_recurse(uint8_t nesting, dalDeserializer_t* deser, dal_t* node);


dalResult_e	mpack_to_dal_read(void* ctx, uint8_t** buf, uint32_t* available, dalDeserializerValue_t* value)
{
	if (*available < sizeof(uint8_t))	return DAL_FORMAT_ERR;
	uint8_t			type		= dal_mpack_read_u8(buf, available);

	switch (type)
	{
		default:	return DAL_FORMAT_ERR;

		// positive fixnum
		case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
		case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0e: case 0x0f:
		case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
		case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
		case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26: case 0x27:
		case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2e: case 0x2f:
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
		case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
		case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5e: case 0x5f:
		case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
		case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6e: case 0x6f:
		case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
		case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
			value->type		= DVT_UINT;
			value->as_uint	= type;
			return DAL_OK;

		// negative fixnum
		case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe6: case 0xe7:
		case 0xe8: case 0xe9: case 0xea: case 0xeb: case 0xec: case 0xed: case 0xee: case 0xef:
		case 0xf0: case 0xf1: case 0xf2: case 0xf3: case 0xf4: case 0xf5: case 0xf6: case 0xf7:
		case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xfe: case 0xff:
			value->type		= DVT_INT;
			value->as_int	= static_cast<int8_t>(type);
			break;

		// fixmap
		case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
		case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
			value->type		= DVT_OBJECT;
			value->size		= type & ~0xf0u;
			break;

		// fixarray
		case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
		case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
			value->type		= DVT_ARRAY;
			value->size		= type & ~0xf0u;
			break;

		// fixstr
		case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7:
		case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf:
		case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7:
		case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
			value->type		= DVT_STR;
			value->size		= type & ~0xe0u;
			if (*available < value->size)		return DAL_FORMAT_ERR;
			value->as_str	= reinterpret_cast<char*>(*buf);
			*buf			+= value->size;
			*available		-= value->size;
			break;

		// nil
		case 0xc0:
			value->type		= DVT_UNKN;
			//Not supported in DAL - so just ignore
			break;

		// bool
		case 0xc2: case 0xc3:
			value->type		= DVT_BOOL;
			value->as_bool	= static_cast<bool>(type & 0x01);
			break;

		// bin8
		case 0xc4:
			if (*available < sizeof(uint8_t))	return DAL_FORMAT_ERR;
			value->size		= dal_mpack_read_u8(buf, available);
			if (*available < value->size)		return DAL_FORMAT_ERR;
			value->type		= DVT_BLOB;
			value->as_blob	= *buf;
			*buf			+= value->size;
			*available		-= value->size;
			break;

		// bin16
		case 0xc5:
			if (*available < sizeof(uint16_t))	return DAL_FORMAT_ERR;
			value->size		= dal_mpack_read_u16(buf, available);
			if (*available < value->size)		return DAL_FORMAT_ERR;
			value->type		= DVT_BLOB;
			value->as_blob	= *buf;
			*buf			+= value->size;
			*available		-= value->size;
			break;

		// bin32
		case 0xc6:
			if (*available < sizeof(uint32_t))	return DAL_FORMAT_ERR;
			value->size		= dal_mpack_read_u32(buf, available);
			if (*available < value->size)		return DAL_FORMAT_ERR;
			value->type		= DVT_BLOB;
			value->as_blob	= *buf;
			*buf			+= value->size;
			*available		-= value->size;
			break;

		// float
		case 0xca:
			if (*available < sizeof(float))		return DAL_FORMAT_ERR;
			value->type		= DVT_DBL;
			value->as_dbl	= dal_mpack_read_float(buf, available);
			break;

		// double
		case 0xcb:
			if (*available < sizeof(double))	return DAL_FORMAT_ERR;
			value->type		= DVT_DBL;
			value->as_dbl	= dal_mpack_read_double(buf, available);
			break;

		// uint8
		case 0xcc:
			if (*available < sizeof(uint8_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_UINT;
			value->as_uint	= dal_mpack_read_u8(buf, available);
			break;

		// uint16
		case 0xcd:
			if (*available < sizeof(uint16_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_UINT;
			value->as_uint	= dal_mpack_read_u16(buf, available);
			break;

		// uint32
		case 0xce:
			if (*available < sizeof(uint32_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_UINT;
			value->as_uint	= dal_mpack_read_u32(buf, available);
			break;

		// uint64
		case 0xcf:
			if (*available < sizeof(uint64_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_UINT;
			value->as_uint	= dal_mpack_read_u64(buf, available);
			break;

		// int8
		case 0xd0:
			if (*available < sizeof(int8_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_INT;
			value->as_int	= dal_mpack_read_i8(buf, available);
			break;

		// int16
		case 0xd1:
			if (*available < sizeof(int16_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_INT;
			value->as_int	= dal_mpack_read_i16(buf, available);
			break;

		// int32
		case 0xd2:
			if (*available < sizeof(int32_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_INT;
			value->as_int	= dal_mpack_read_i32(buf, available);
			break;

		// int64
		case 0xd3:
			if (*available < sizeof(int64_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_INT;
			value->as_int	= dal_mpack_read_i64(buf, available);
			break;

		// str8
		case 0xd9:
			if (*available < sizeof(uint8_t))	return DAL_FORMAT_ERR;
			value->size		= dal_mpack_read_u8(buf, available);
			if (*available < value->size)		return DAL_FORMAT_ERR;
			value->type		= DVT_STR;
			value->as_str	= reinterpret_cast<char*>(*buf);
			*buf			+= value->size;
			*available		-= value->size;
			break;

		// str16
		case 0xda:
			if (*available < sizeof(uint16_t))	return DAL_FORMAT_ERR;
			value->size		= dal_mpack_read_u16(buf, available);
			if (*available < value->size)		return DAL_FORMAT_ERR;
			value->type		= DVT_STR;
			value->as_str	= reinterpret_cast<char*>(*buf);
			*buf			+= value->size;
			*available		-= value->size;
			break;

		// str32
		case 0xdb:
			if (*available < sizeof(uint32_t))	return DAL_FORMAT_ERR;
			value->size		= dal_mpack_read_u32(buf, available);
			if (*available < value->size)		return DAL_FORMAT_ERR;
			value->type		= DVT_STR;
			value->as_str	= reinterpret_cast<char*>(*buf);
			*buf			+= value->size;
			*available		-= value->size;
			break;

		// array16
		case 0xdc:
			if (*available < sizeof(uint16_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_ARRAY;
			value->size		= dal_mpack_read_u16(buf, available);
			break;

		// array32
		case 0xdd:
			if (*available < sizeof(uint32_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_ARRAY;
			value->size		= dal_mpack_read_u16(buf, available);
			break;

		// map16
		case 0xde:
			if (*available < sizeof(uint16_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_OBJECT;
			value->size		= dal_mpack_read_u16(buf, available);
			break;

		// map32
		case 0xdf:
			if (*available < sizeof(uint32_t))	return DAL_FORMAT_ERR;
			value->type		= DVT_OBJECT;
			value->size		= dal_mpack_read_u32(buf, available);
			break;
	}

	return DAL_OK;
};



//===============================================================================================================================
dalResult_e	dal_t::from_mpack(uint8_t* buf, uint32_t size)
{
	while (this->_child != nullptr)
	{
		dal_t*	next	= this->_child->_next;
		dal_delete(this->_child);
		this->_child	= next;
	}

	dalDeserializer_t	deser;
	deser.buf			= buf;
	deser.available		= size;
	deser.context		= nullptr;
	deser.read			= mpack_to_dal_read;

	return _dal_deserialize_recurse(0, &deser, this);
};
