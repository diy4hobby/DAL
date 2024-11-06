#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"

extern	dalMemHooks_t		_dal_memHooks;


//===============================================================================================================================
dal_t*	dal_t::add_val_bool(dalStr_t* key, bool value)
{
	dal_t*	newChild	= this->add_child(DT_BOOL);
	if (newChild == nullptr)			return nullptr;
	newChild->rename(key);
	newChild->_as_bool	= value;
	newChild->_size		= sizeof(bool);
	return newChild;
};

dal_t*	dal_t::add_val_int(dalStr_t* key, int64_t value)
{
	dal_t*	newChild	= this->add_child(DT_INT);
	if (newChild == nullptr)			return nullptr;
	newChild->rename(key);
	newChild->_as_int	= value;
	newChild->_size		= sizeof(int64_t);
	return newChild;
};

dal_t*	dal_t::add_val_uint(dalStr_t* key, uint64_t value)
{
	dal_t*	newChild	= this->add_child(DT_UINT);
	if (newChild == nullptr)			return nullptr;
	newChild->rename(key);
	newChild->_as_uint	= value;
	newChild->_size		= sizeof(uint64_t);
	return newChild;
};

dal_t*	dal_t::add_val_dbl(dalStr_t* key, double value)
{
	dal_t*	newChild	= this->add_child(DT_DOUBLE);
	if (newChild == nullptr)			return nullptr;
	newChild->rename(key);
	newChild->_as_dbl	= value;
	newChild->_size		= sizeof(double);
	return newChild;
};

dal_t*	dal_t::add_val_str(dalStr_t* key, char* value)
{
	dal_t*	newChild	= this->add_child(DT_STRING);
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= dal_string_length(value, DAL_MAX_CSTR_LEN);
	newChild->_mem_ptr	= _dal_memHooks.alloc_data(newChild->_size + 8 + 1);
	if (newChild->_mem_ptr == nullptr)
	{
		dal_delete(newChild);
		return nullptr;
	}else
	{
		newChild->_as_str	= reinterpret_cast<char*>(dal_pointer_align_8(newChild->_mem_ptr));
		dal_bytedata_copy(newChild->_as_str, value, newChild->_size);
		newChild->_as_str[newChild->_size]	= 0x00;
	}
	newChild->rename(key);
	return newChild;
};

dal_t*	dal_t::add_val_str(dalStr_t* key, const char* value)
{
	dal_t*	newChild	= this->add_child(DT_STRING);
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= dal_string_length(value, DAL_MAX_CSTR_LEN);
	newChild->_mem_ptr	= _dal_memHooks.alloc_data(newChild->_size + 8 + 1);
	if (newChild->_mem_ptr == nullptr)
	{
		dal_delete(newChild);
		return nullptr;
	}else
	{
		newChild->_as_str	= reinterpret_cast<char*>(dal_pointer_align_8(newChild->_mem_ptr));
		void*	tmp			= const_cast<void*>(static_cast<const void*>(value));
		dal_bytedata_copy(newChild->_as_str, tmp, newChild->_size);
		newChild->_as_str[newChild->_size]	= 0x00;
	}
	newChild->rename(key);
	return newChild;
};

dal_t*	dal_t::add_val_str(dalStr_t* key, char* value, uint32_t len)
{
	dal_t*	newChild	= this->add_child(DT_STRING);
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= len;
	newChild->_mem_ptr	= _dal_memHooks.alloc_data(newChild->_size + 8 + 1);
	if (newChild->_mem_ptr == nullptr)
	{
		dal_delete(newChild);
		return nullptr;
	}else
	{
		newChild->_as_str	= reinterpret_cast<char*>(dal_pointer_align_8(newChild->_mem_ptr));
		dal_bytedata_copy(newChild->_as_str, value, newChild->_size);
		newChild->_as_str[newChild->_size]	= 0x00;
	}
	newChild->rename(key);
	return newChild;
};

dal_t*	dal_t::add_val_blob(dalStr_t* key, void*& value, uint32_t len)
{
	dal_t*	newChild	= this->add_child(DT_BLOB);
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= len;
	newChild->_mem_ptr	= _dal_memHooks.alloc_data(newChild->_size + 8);
	if (newChild->_mem_ptr == nullptr)
	{
		dal_delete(newChild);
		value			= nullptr;
		return nullptr;
	}else
	{
		newChild->_as_blob	= reinterpret_cast<uint8_t*>(dal_pointer_align_8(newChild->_mem_ptr));
		value				= newChild->_as_blob;
	}
	newChild->rename(key);
	return newChild;
};

dal_t*	dal_t::add_val_bin(dalStr_t* key, void* value, uint32_t len)
{
	dal_t*	newChild	= this->add_child(DT_BLOB);
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= len;
	newChild->_mem_ptr	= _dal_memHooks.alloc_data(newChild->_size + 8);
	if (newChild->_mem_ptr == nullptr)
	{
		dal_delete(newChild);
		value			= nullptr;
		return nullptr;
	}else
	{
		newChild->_as_blob	= reinterpret_cast<uint8_t*>(dal_pointer_align_8(newChild->_mem_ptr));
		dal_bytedata_copy(newChild->_as_blob, value, len);
	}
	newChild->rename(key);
	return newChild;
};

dal_t*	dal_t::add_val_ref(dalStr_t* key, void* value, uint32_t len)
{
	dal_t*	newChild	= this->add_child(DT_BLOB_REF);
	if (newChild == nullptr)			return nullptr;

	newChild->_size		= len;
	newChild->_mem_ptr	= value;
	newChild->_type		= DT_BLOB_REF;
	newChild->_as_blob	= static_cast<uint8_t*>(value);
	newChild->rename(key);
	return newChild;
};

//===============================================================================================================================

dal_t*	dal_t::add_val_bool(const char* key, bool value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_bool(&dalStrKey, value);
};

dal_t*	dal_t::add_val_int(const char* key, int64_t value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_int(&dalStrKey, value);
};

dal_t*	dal_t::add_val_uint(const char* key, uint64_t value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_uint(&dalStrKey, value);
};

dal_t*	dal_t::add_val_dbl(const char* key, double value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_dbl(&dalStrKey, value);
};

dal_t*	dal_t::add_val_str(const char* key, char* value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_str(&dalStrKey, value);
};

dal_t*	dal_t::add_val_str(const char* key, const char* value)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_str(&dalStrKey, value);
};

dal_t*	dal_t::add_val_str(const char* key, char* value, uint32_t len)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_str(&dalStrKey, value, len);
};

dal_t*	dal_t::add_val_ref(const char* key, void* value, uint32_t len)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_ref(&dalStrKey, value, len);
};

dal_t*	dal_t::add_val_blob(const char* key, void*& value, uint32_t len)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_blob(&dalStrKey, value, len);
};

dal_t*	dal_t::add_val_bin(const char* key, void* value, uint32_t len)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->add_val_bin(&dalStrKey, value, len);
};

//===============================================================================================================================

dal_t&	dal_t::operator[] (const char* key)
{
	if (*key == 0x00)					return *this;
	//If we got here and the type of the node is not known, we convert it into an object - this allows us to correctly
	//work out paths like "blabla1/blabla2", if "blabla1" did not exist - it will be created, and its type will be unknown,
	//but then when passing the path node "blabla1" should become an object
	if (this->_type == DT_UNKN)			this->_type		= DT_OBJECT;
	while (*key == DAL_PATH_DIVIDER)	key++;
	dal_t*		result	= this;
	const char* symbol	= key;
	uint32_t	keyLen	= 0;
	uint32_t	keyHash	= 0;

	while ((*symbol != 0x00) && (*symbol != DAL_PATH_DIVIDER) && (*symbol != '['))
	{
		keyHash	= *symbol + (keyHash << 6) + (keyHash << 16) - keyHash;
		symbol++;
		keyLen++;
		if (keyLen == DAL_KEY_SIZE)
		{	result	= this->add_child("");
			return *result;
		}
	}
		
	if (*symbol == '[')
	{
		symbol++;
		uint32_t	idx	= 0;
		uint32_t	numLen	= 0;
		while (*symbol != ']')
		{
			int		num	= *symbol - '0';
			//Simple character and number length check, allowing up to a billion elements in an array
			if ((num > 9) || (num < 0) || (numLen > 9))
			{	result	= this->add_child(DT_ARRAY);
				result->_rename(key, keyLen, keyHash);
				return *result;
			}
			idx	= 10 * idx + num;
			symbol++;
			numLen++;
		}
		symbol++;

		if (keyLen > 0)
		{
			result		= this->_get_child_by_hash(keyHash);
			if (result != nullptr)
			{
				if (dal_string_compare(result->_key, key, result->_key_len) == TRUE)
				{
					if (result->_type != DT_ARRAY)
					{	dal_delete(result);
						result	= this->add_child(DT_ARRAY);
						result->_rename(key, keyLen, keyHash);
					}
				}else
				{	result	= this->add_child(DT_ARRAY);
					result->_rename(key, keyLen, keyHash);
				}
			}else
			{	result	= this->add_child(DT_ARRAY);
				result->_rename(key, keyLen, keyHash);
			}
		}else	result	= this;
		
		if (result->_size <= idx)
		{
			uint32_t	count	= idx - result->_size + 1;
			while (count > 0)
			{
				result->add_child(DT_UNKN);
				count--;
			}
		}

		result	= result->_child;
		while (idx > 0)
		{
			result	= result->_next;
			idx--;
		}
		return (*result)[symbol];
	}

	if (keyLen > 0)
	{
		result	= this->_get_child_by_hash(keyHash);
		if ((result == nullptr) || (dal_string_compare(result->_key, key, result->_key_len) == FALSE))
		{
			result	= this->add_child(DT_UNKN);
			if (result == nullptr)	return *this;
			result->_rename(key, keyLen, keyHash);
		}
		return (*result)[symbol];
	}

	/*
	result				= this->_get_child_by_hash(keyHash);
	if (result == nullptr)
	{	result	= this->add_child();
		result->_rename(key, keyLen, keyHash);
	}else
	{	bool	collision	= (dal_string_compare(result->_key, key, result->_key_len) == FALSE);
		if (collision)
		{	result	= this->add_child();
			result->_rename(key, keyLen, keyHash);
		}
	}
	*/
	return *result;
};

dal_t&	dal_t::operator[] (uint32_t idx)
{
	if (this->_type != DT_ARRAY)
	{
		while (this->_child != nullptr)
		{
			dal_delete(this->_child);
		}
	}

	if (this->_size < idx)
	{
		uint32_t	need	= idx - this->_size + 1;
		while (need > 0)
		{
			this->add_child(DT_UNKN);
			need--;
		}
	}

	dal_t*	result	= this->_child;
	while (idx > 0)
	{
		result	= result->_next;
		idx--;
	};
	return *result;
};

//===============================================================================================================================