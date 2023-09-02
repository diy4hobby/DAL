#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"

extern	dalMemHooks_t		_dal_memHooks;



//===============================================================================================================================
dal_t*	dal_t::add_array(dalStr_t* key, uint32_t count)
{
	dal_t*	newChild	= this->create_child(DT_ARRAY);
	if (newChild == nullptr)			return nullptr;
	newChild->rename(key);
	
	//Allocate memory for nodes
	while (count > 0)
	{
		if (newChild->create_child() == nullptr)
		{	dal_delete(newChild);
			return nullptr;
		}
		count--;
	}

	return newChild;
};

dal_t*	dal_t::add_array(const char* key)
{
	dal_t*	newChild	= this->create_child(DT_ARRAY);
	if (newChild == nullptr)			return nullptr;
	newChild->rename(key);
	return newChild;
};

dal_t*	dal_t::add_arr_bool(dalStr_t* key, bool* arr, uint32_t count)
{
	dal_t*	array	= this->add_array(key, count);
	if (array == nullptr)			return nullptr;
	dal_t*	item		= array->_child;

	while (count-- > 0)
	{
		*item	= *arr++;
		item	= item->_next;
	}

	return array;
};

dal_t*	dal_t::add_arr_int(dalStr_t* key, int* arr, uint32_t count)
{
	dal_t*	array	= this->add_array(key, count);
	if (array == nullptr)			return nullptr;
	dal_t*	item		= array->_child;
	int64_t	value;

	while (count-- > 0)
	{
		value	= *arr++;
		*item	= value;
		item	= item->_next;
	}

	return array;
};

dal_t*	dal_t::add_arr_uint(dalStr_t* key, unsigned int* arr, uint32_t count)
{
	dal_t*	array	= this->add_array(key, count);
	if (array == nullptr)			return nullptr;
	dal_t*		item		= array->_child;
	uint64_t	value;

	while (count-- > 0)
	{
		value	= *arr++;
		*item	= value;
		item	= item->_next;
	}

	return array;
};

dal_t*	dal_t::add_arr_int32(dalStr_t* key, int32_t* arr, uint32_t count)
{
	dal_t*	array	= this->add_array(key, count);
	if (array == nullptr)			return nullptr;
	dal_t*		item		= array->_child;
	int64_t		value;

	while (count-- > 0)
	{
		value	= *arr++;
		*item	= value;
		item	= item->_next;
	}

	return array;
};

dal_t*	dal_t::add_arr_uint32(dalStr_t* key, uint32_t* arr, uint32_t count)
{
	dal_t*	array	= this->add_array(key, count);
	if (array == nullptr)			return nullptr;
	dal_t*		item		= array->_child;
	uint64_t	value;

	while (count-- > 0)
	{
		value	= *arr++;
		*item	= value;
		item	= item->_next;
	}

	return array;
};

dal_t*	dal_t::add_arr_int64(dalStr_t* key, int64_t* arr, uint32_t count)
{
	dal_t*	array	= this->add_array(key, count);
	if (array == nullptr)			return nullptr;
	dal_t*	item		= array->_child;

	while (count-- > 0)
	{
		*item	= *arr++;
		item	= item->_next;
	}

	return array;
};

dal_t*	dal_t::add_arr_uint64(dalStr_t* key, uint64_t* arr, uint32_t count)
{
	dal_t*	array	= this->add_array(key, count);
	if (array == nullptr)			return nullptr;
	dal_t*	item		= array->_child;

	while (count-- > 0)
	{
		*item	= *arr++;
		item	= item->_next;
	}

	return array;
};

dal_t*	dal_t::add_arr_flt(dalStr_t* key, float* arr, uint32_t count)
{
	dal_t*	array	= this->add_array(key, count);
	if (array == nullptr)			return nullptr;
	dal_t*	item		= array->_child;

	while (count-- > 0)
	{
		*item	= *arr++;
		item	= item->_next;
	}

	return array;
};

dal_t*	dal_t::add_arr_dbl(dalStr_t* key, double* arr, uint32_t count)
{
	dal_t*	array	= this->add_array(key, count);
	if (array == nullptr)			return nullptr;
	dal_t*	item		= array->_child;

	while (count-- > 0)
	{
		*item	= *arr++;
		item	= item->_next;
	}

	return array;
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
	dal_t*	child	= this->_child;
	while (idx-- > 0)	child	= child->_next;
	return child;
};
//===============================================================================================================================
