#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"

extern	dalMemHooks_t		_dal_memHooks;



//===============================================================================================================================
dal_t*	dal_t::convert_to_array(uint32_t count)
{
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);

	dal_t*	child		= this->_child;
	dal_t*	next;
	while (child != nullptr)
	{	next			= child->_next;
		dal_delete(child);
		child			= next;
	}

	this->_type			= DT_ARRAY;
	this->_child		= static_cast<dal_t*>(_dal_memHooks.alloc_data(count * sizeof(dal_t)));

	if (this->_child == nullptr)
	{	this->_size		= 0;
		return nullptr;
	}

	dal_bytedata_zero(this->_child, count * sizeof(dal_t));

	this->_size			= count;
	return this->_child;
};


dal_t*	dal_t::add_array(dalStr_t* key, uint32_t count)
{
	dal_t*	newChild	= dal_create();
	if (newChild == nullptr)			return nullptr;
	//Allocate memory for nodes pointers array
	newChild->_child	= static_cast<dal_t*>(_dal_memHooks.alloc_data(count * sizeof(dal_t)));
	if (newChild->_child == nullptr)
	{	dal_delete(newChild);
		return nullptr;
	}

	dal_t*		item	= newChild->_child;
	uint32_t	itemIdx	= count;
	while (itemIdx-- > 0)
	{	item->_parent	= nullptr;
		item->_prev		= nullptr;
		item->_next		= nullptr;
		item->_child	= nullptr;
		item->_key[0]	= 0x00;
		item->_key_len	= 0;
		item->_key_hash	= 0;
		item->_type		= DT_UNKN;
		item->_size		= 0;
		++item;
	}

	newChild->_size		= count;
	newChild->_type		= DT_ARRAY;
	this->attach(key, newChild);

	return newChild->_child;
};

dal_t*	dal_t::add_arr_bool(dalStr_t* key, bool* arr, uint32_t count)
{
	dal_t*	newArray	= this->add_array(key, count);
	if (newArray == nullptr)			return nullptr;

	while (count-- > 0)
	{
		*newArray++		= *arr++;
	}

	return newArray;
};

dal_t*	dal_t::add_arr_int(dalStr_t* key, int* arr, uint32_t count)
{
	dal_t*	newArray	= this->add_array(key, count);
	if (newArray == nullptr)			return nullptr;
	int64_t		value;
	while (count-- > 0)
	{
		value			= *arr++;
		*newArray++		= value;
	}

	return newArray;
};

dal_t*	dal_t::add_arr_uint(dalStr_t* key, unsigned int* arr, uint32_t count)
{
	dal_t*	newArray	= this->add_array(key, count);
	if (newArray == nullptr)			return nullptr;
	uint64_t	value;
	while (count-- > 0)
	{
		value			= *arr++;
		*newArray++		= value;
	}

	return newArray;
};

dal_t*	dal_t::add_arr_int32(dalStr_t* key, int32_t* arr, uint32_t count)
{
	dal_t*	newArray	= this->add_array(key, count);
	if (newArray == nullptr)			return nullptr;
	int64_t		value;
	while (count-- > 0)
	{
		value			= *arr++;
		*newArray++		= value;
	}

	return newArray;
};

dal_t*	dal_t::add_arr_uint32(dalStr_t* key, uint32_t* arr, uint32_t count)
{
	dal_t*	newArray	= this->add_array(key, count);
	if (newArray == nullptr)			return nullptr;
	uint64_t	value;
	while (count-- > 0)
	{
		value			= *arr++;
		*newArray++		= value;
	}

	return newArray;
};

dal_t*	dal_t::add_arr_int64(dalStr_t* key, int64_t* arr, uint32_t count)
{
	dal_t*	newArray	= this->add_array(key, count);
	if (newArray == nullptr)			return nullptr;

	while (count-- > 0)
	{
		*newArray++		= *arr++;
	}

	return newArray;
};

dal_t*	dal_t::add_arr_uint64(dalStr_t* key, uint64_t* arr, uint32_t count)
{
	dal_t*	newArray	= this->add_array(key, count);
	if (newArray == nullptr)			return nullptr;

	while (count-- > 0)
	{
		*newArray++		= *arr++;
	}

	return newArray;
};

dal_t*	dal_t::add_arr_flt(dalStr_t* key, float* arr, uint32_t count)
{
	dal_t*	newArray	= this->add_array(key, count);
	if (newArray == nullptr)			return nullptr;

	while (count-- > 0)
	{
		*newArray++		= *arr++;
	}

	return newArray;
};

dal_t*	dal_t::add_arr_dbl(dalStr_t* key, double* arr, uint32_t count)
{
	dal_t*	newArray	= this->add_array(key, count);
	if (newArray == nullptr)			return nullptr;

	while (count-- > 0)
	{
		*newArray++		= *arr++;
	}

	return newArray;
};

//===============================================================================================================================

dal_t*	dal_t::add_array(const char* key, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_array(&dalStrKey, count);
};

dal_t*	dal_t::add_arr_bool(const char* key, bool* arr, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_arr_bool(&dalStrKey, arr, count);
};

dal_t*	dal_t::add_arr_int(const char* key, int* arr, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_arr_int(&dalStrKey, arr, count);
};

dal_t*	dal_t::add_arr_uint(const char* key, unsigned int* arr, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_arr_uint(&dalStrKey, arr, count);
};

dal_t*	dal_t::add_arr_int32(const char* key, int32_t* arr, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_arr_int32(&dalStrKey, arr, count);
};

dal_t*	dal_t::add_arr_uint32(const char* key, uint32_t* arr, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_arr_uint32(&dalStrKey, arr, count);
};

dal_t*	dal_t::add_arr_int64(const char* key, int64_t* arr, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_arr_int64(&dalStrKey, arr, count);
};

dal_t*	dal_t::add_arr_uint64(const char* key, uint64_t* arr, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_arr_uint64(&dalStrKey, arr, count);
};

dal_t*	dal_t::add_arr_flt(const char* key, float* arr, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_arr_flt(&dalStrKey, arr, count);
};

dal_t*	dal_t::add_arr_dbl(const char* key, double* arr, uint32_t count)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_arr_dbl(&dalStrKey, arr, count);
};

//===============================================================================================================================

uint32_t	dal_t::get_array_size()
{
	if (this->_type != DT_ARRAY)	return 0;
	return this->_size;
};

dal_t*	dal_t::get_array_item(uint32_t idx)
{
	if (idx > this->_size)	return nullptr;
	return &this->_child[idx];
};
//===============================================================================================================================
