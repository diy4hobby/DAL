#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"

extern	dalMemHooks_t		_dal_memHooks;


//===============================================================================================================================
dal_t*	dal_t::add_value(dalStr_t* key, bool value)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;
	newChild->_type		= DT_BOOL;
	newChild->_as_bool	= value;
	newChild->_size		= sizeof(bool);
	newChild->rename(key);
	newChild->_parent	= this;
	newChild->_next		= this->_child;
	this->_child		= newChild;
	return newChild;
};

dal_t*	dal_t::add_value(dalStr_t* key, int value)
{
	return this->add_value(key, static_cast<int64_t>(value));
};

dal_t*	dal_t::add_value(dalStr_t* key, uint64_t value)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;
	newChild->_type		= DT_UINT;
	newChild->_as_uint	= value;
	newChild->_size		= sizeof(uint64_t);
	newChild->rename(key);
	newChild->_parent	= this;
	newChild->_next		= this->_child;
	this->_child		= newChild;
	return newChild;
};

dal_t*	dal_t::add_value(dalStr_t* key, int64_t value)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;
	newChild->_type		= DT_INT;
	newChild->_as_int	= value;
	newChild->_size		= sizeof(int64_t);
	newChild->rename(key);
	newChild->_parent	= this;
	newChild->_next		= this->_child;
	this->_child		= newChild;
	return newChild;
};

dal_t*	dal_t::add_value(dalStr_t* key, float value)
{
	return this->add_value(key, static_cast<double>(value));
};

dal_t*	dal_t::add_value(dalStr_t* key, double value)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;
	newChild->_type		= DT_DOUBLE;
	newChild->_as_dbl	= value;
	newChild->_size		= sizeof(double);
	newChild->rename(key);
	newChild->_parent	= this;
	newChild->_next		= this->_child;
	this->_child		= newChild;
	return newChild;
};

dal_t*	dal_t::add_value(dalStr_t* key, char* value)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= dal_string_length(value, DAL_MAX_CSTR_LEN);
	newChild->_mem_ptr	= _dal_memHooks.alloc_data(newChild->_size + 8);
	if (newChild->_mem_ptr == nullptr)
	{
		dal_delete(newChild);
		return nullptr;
	}else
	{
		newChild->_type		= DT_STRING;
		newChild->_as_str	= reinterpret_cast<char*>(dal_pointer_align_8(newChild->_mem_ptr));
		dal_bytedata_copy(newChild->_as_str, value, newChild->_size);
	}
	newChild->rename(key);
	newChild->_parent	= this;
	newChild->_next		= this->_child;
	this->_child		= newChild;
	return newChild;
};

dal_t*	dal_t::add_value(dalStr_t* key, const char* value)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= dal_string_length(value, DAL_MAX_CSTR_LEN);
	newChild->_mem_ptr	= _dal_memHooks.alloc_data(newChild->_size + 8);
	if (newChild->_mem_ptr == nullptr)
	{
		dal_delete(newChild);
		return nullptr;
	}else
	{
		newChild->_type		= DT_STRING;
		newChild->_as_str	= reinterpret_cast<char*>(dal_pointer_align_8(newChild->_mem_ptr));
		void*	tmp			= const_cast<void*>(static_cast<const void*>(value));
		dal_bytedata_copy(newChild->_as_str, tmp, newChild->_size);
	}
	newChild->rename(key);
	newChild->_parent	= this;
	newChild->_next		= this->_child;
	this->_child		= newChild;
	return newChild;
};

dal_t*	dal_t::add_value(dalStr_t* key, char* value, uint32_t len)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= len;
	newChild->_mem_ptr	= _dal_memHooks.alloc_data(newChild->_size + 8);
	if (newChild->_mem_ptr == nullptr)
	{
		dal_delete(newChild);
		return nullptr;
	}else
	{
		newChild->_type		= DT_STRING;
		newChild->_as_str	= reinterpret_cast<char*>(dal_pointer_align_8(newChild->_mem_ptr));
		dal_bytedata_copy(newChild->_as_str, value, newChild->_size);
	}
	newChild->rename(key);
	newChild->_parent	= this;
	newChild->_next		= this->_child;
	this->_child		= newChild;
	return newChild;
};

dal_t*	dal_t::add_value(dalStr_t* key, void** value, uint32_t len)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= len;
	newChild->_mem_ptr	= _dal_memHooks.alloc_data(newChild->_size + 8);
	if (newChild->_mem_ptr == nullptr)
	{
		dal_delete(newChild);
		*value			= nullptr;
		return nullptr;
	}else
	{
		newChild->_type		= DT_BLOB;
		newChild->_as_blob	= reinterpret_cast<uint8_t*>(dal_pointer_align_8(newChild->_mem_ptr));
		*value				= newChild->_as_blob;
	}
	newChild->rename(key);
	newChild->_parent	= this;
	newChild->_next		= this->_child;
	this->_child		= newChild;
	return newChild;
};

dal_t*	dal_t::add_value(dalStr_t* key, void* value, uint32_t len)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= len;
	newChild->_mem_ptr	= value;
	newChild->_type		= DT_BLOB_REF;
	newChild->_as_blob	= static_cast<uint8_t*>(value);
	newChild->rename(key);
	newChild->_parent	= this;
	newChild->_next		= this->_child;
	this->_child		= newChild;
	return newChild;
};

//===============================================================================================================================

dal_t*	dal_t::add_value(const char* key, bool value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, value);
};

dal_t*	dal_t::add_value(const char* key, int value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, static_cast<int64_t>(value));
};

dal_t*	dal_t::add_value(const char* key, uint64_t value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, value);
};

dal_t*	dal_t::add_value(const char* key, int64_t value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, value);
};

dal_t*	dal_t::add_value(const char* key, float value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, static_cast<double>(value));
};

dal_t*	dal_t::add_value(const char* key, double value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, value);
};

dal_t*	dal_t::add_value(const char* key, char* value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, value);
};

dal_t*	dal_t::add_value(const char* key, const char* value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, value);
};

dal_t*	dal_t::add_value(const char* key, char* value, uint32_t len)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, value, len);
};

dal_t*	dal_t::add_value(const char* key, void** value, uint32_t len)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, value, len);
};

dal_t*	dal_t::add_value(const char* key, void* value, uint32_t len)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_value(&dalStrKey, value, len);
};

//===============================================================================================================================

dal_t*	dal_t::add_node(dalStr_t* key, dal_t* node)
{
	if (node == nullptr)				return nullptr;

	node->detach();
	node->rename(key);
	if (this->_child == nullptr)	this->_child	= node;
	else
	{	this->_child->_prev	= node;
		node->_next			= this->_child;
		this->_child		= node;
		node->_parent		= this;
	}
	return node;
};

dal_t*	dal_t::add_node(char* key, dal_t* node)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_node(&dalStrKey, node);
};

dal_t*	dal_t::add_node(dal_t* node)
{
	if (node == nullptr)				return nullptr;

	node->detach();
	if (this->_child == nullptr)	this->_child	= node;
	else
	{	this->_child->_prev	= node;
		node->_next			= this->_child;
		this->_child		= node;
		node->_parent		= this;
	}
	return node;
};
//===============================================================================================================================
