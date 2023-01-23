#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"

extern	dalMemHooks_t		_dal_memHooks;


//===============================================================================================================================
dalResult_e	dal_t::_copy_trivial(dal_t* src)
{
	if (src == nullptr)		return DAL_MEM_ERR;

	this->_key_len		= src->_key_len;
	this->_key_hash		= src->_key_hash;
	dal_bytedata_copy(this->_key, src->_key, src->_key_len);

	this->_type			= src->_type;
	this->_size			= src->_size;

	switch (src->_type)
	{
		default:			return DAL_FORMAT_ERR;
		case DT_UNKN:		return DAL_FORMAT_ERR;
		case DT_BOOL:
			this->_as_bool		= src->_as_bool;
			return DAL_OK;
		case DT_UINT:
			this->_as_uint		= src->_as_uint;
			return DAL_OK;
		case DT_INT:
			this->_as_int		= src->_as_int;
			return DAL_OK;
		case DT_DOUBLE:
			this->_as_dbl		= src->_as_dbl;
			return DAL_OK;
		case DT_STRING:
			this->_mem_ptr		= _dal_memHooks.alloc_data(this->_size + 8);
			if (this->_mem_ptr == nullptr)	return DAL_MEM_ERR;
			this->_as_str		= reinterpret_cast<char*>(dal_pointer_align_8(this->_mem_ptr));
			dal_bytedata_copy(this->_as_str, src->_as_str, this->_size);
			return DAL_OK;
		case DT_BLOB:
			this->_mem_ptr		= _dal_memHooks.alloc_data(this->_size + 8);
			if (this->_mem_ptr == nullptr)	return DAL_MEM_ERR;
			this->_as_blob		= reinterpret_cast<uint8_t*>(dal_pointer_align_8(this->_mem_ptr));
			dal_bytedata_copy(this->_as_blob, src->_as_blob, this->_size);
			return DAL_OK;
		case DT_BLOB_REF:
			this->_as_blob		= src->_as_blob;
			return DAL_OK;
		case DT_ARRAY:
		case DT_OBJECT:
			return DAL_FORMAT_ERR;
	}
};

//===============================================================================================================================
