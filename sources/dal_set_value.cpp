#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"

extern	dalMemHooks_t		_dal_memHooks;



//===============================================================================================================================
dal_t&	dal_t::operator=(bool value)
{
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);
	this->_type			= DT_BOOL;
	this->_size			= sizeof(bool);
	this->_as_bool		= value;
	return *this;
};

dal_t&	dal_t::operator=(int value)
{
	*this				= static_cast<int64_t>(value);
	return *this;
};

dal_t&	dal_t::operator=(uint64_t value)
{
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);
	this->_type			= DT_UINT;
	this->_size			= sizeof(uint64_t);
	this->_as_uint		= value;
	return *this;
};

dal_t&	dal_t::operator=(int64_t value)
{
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);
	this->_type			= DT_INT;
	this->_size			= sizeof(int64_t);
	this->_as_int		= value;
	return *this;
};

dal_t&	dal_t::operator=(double value)
{
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);
	this->_type			= DT_DOUBLE;
	this->_size			= sizeof(double);
	this->_as_dbl		= value;
	return *this;
};

dal_t&	dal_t::operator=(char* value)
{
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);
	this->_type			= DT_STRING;
	this->_size			= dal_string_length(value, DAL_MAX_CSTR_LEN);
	this->_mem_ptr		= _dal_memHooks.alloc_data(this->_size + 8);
	if (this->_mem_ptr == nullptr)
	{	this->_type		= DT_UNKN;
		this->_size		= 0;
		return *this;
	}
	this->_as_str		= reinterpret_cast<char*>(dal_pointer_align_8(this->_mem_ptr));
	dal_bytedata_copy(this->_as_str, value, this->_size);
	return *this;
};

dal_t&	dal_t::operator=(dalStr_t& value)
{
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);
	this->_type			= DT_STRING;
	this->_size			= value.size;
	this->_mem_ptr		= _dal_memHooks.alloc_data(this->_size + 8);
	if (this->_mem_ptr == nullptr)
	{	this->_type		= DT_UNKN;
		this->_size		= 0;
		return *this;
	}
	this->_as_str		= reinterpret_cast<char*>(dal_pointer_align_8(this->_mem_ptr));
	dal_string_copy(this->_as_str, value.data, this->_size);
	return *this;
};

dal_t&	dal_t::operator=(dalBlob_t& value)
{
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);
	this->_type			= DT_BLOB;
	this->_size			= value.size;
	this->_mem_ptr		= _dal_memHooks.alloc_data(this->_size + 8);
	if (this->_mem_ptr == nullptr)
	{	this->_type		= DT_UNKN;
		this->_size		= 0;
		return *this;
	}
	this->_as_blob		= reinterpret_cast<uint8_t*>(dal_pointer_align_8(this->_mem_ptr));
	dal_bytedata_copy(this->_as_blob, value.data, this->_size);
	return *this;
};

dal_t&	dal_t::operator=(dalBlobRef_t& value)
{
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);
	this->_type			= DT_BLOB_REF;
	this->_size			= value.size;
	this->_as_blob		= reinterpret_cast<uint8_t*>(value.data);
	return *this;
};
//===============================================================================================================================
