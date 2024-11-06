#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"


//===============================================================================================================================


dalResult_e	dal_t::_add_item(dal_t* node)
{
	if (node == nullptr)	return DAL_MEM_ERR;
	if (this->_child == nullptr)
	{	this->_child		= node;
		this->_last			= node;
	}else{
		this->_last->_next	= node;
		node->_prev			= this->_last;
		this->_last			= node;
	}
	node->_parent			= this;
	if (this->_type & TUPLE_TYPE)	this->_size++;
	return DAL_OK;
};

dal_t*	dal_t::add_child(dalNodeType_e type)
{
	dal_t*		newChild	= dal_create(type);
	if (newChild == nullptr)					return nullptr;
	if (this->_add_item(newChild) == DAL_OK)	return newChild;
	dal_delete(newChild);
	return nullptr;
};

dal_t*	dal_t::add_child(const char* key, dalNodeType_e type)
{
	dal_t*		newChild	= add_child(type);
	if (newChild == nullptr)	return nullptr;
	newChild->rename(key);
	return newChild;
};

dal_t*	dal_t::_get_child_by_hash(uint32_t hash)
{
	for (dal_t* child = this->_child; child != nullptr; child = child->_next)
	{
		if (child->_key_hash != hash)	continue;
		else							return child;
	}
	return nullptr;
};

dal_t*	dal_t::_get_child_by_path(const char* path)
{
	dal_t*		result	= nullptr;
	const char* symbol	= path;
	uint32_t	len		= 0;
	uint32_t	hash	= 0;

	while ((*symbol != 0x00) && (*symbol != DAL_PATH_DIVIDER) && (*symbol != '['))
	{
		hash	= *symbol + (hash << 6) + (hash << 16) - hash;
		symbol++;
		len++;
		if (len == DAL_KEY_SIZE)	return nullptr;
	}

	if (*symbol == DAL_PATH_DIVIDER)
	{
		result	= this->_get_child_by_hash(hash);
		if (result == nullptr)		return nullptr;
		if	(dal_string_compare(result->_key, path, result->_key_len) == FALSE)	return nullptr;
		return result->_get_child_by_path(symbol+1);
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
			if ((num > 9) || (num < 0) || (numLen > 9))		return nullptr;
			idx	= 10 * idx + num;
			symbol++;
			numLen++;
		}

		result		= this->_get_child_by_hash(hash);
		if (result == nullptr)				return nullptr;
		
		if (dal_string_compare(result->_key, path, result->_key_len) == FALSE)	return nullptr;
		if (result->_type != DT_ARRAY)		return nullptr;		
		if (result->_size <= idx)			return nullptr;

		result	= result->_child;
		while (idx > 0)
		{
			result	= result->_next;
			idx--;
		}
		//Check whether the path is complete, if so, then exit returning the current result
		if (*(symbol+1) == 0x00)	return result;
		else{
			//+2 shifts on "]/"
			return result->_get_child_by_path(symbol+2);
		}
	}

	result				= this->_get_child_by_hash(hash);
	if (result == nullptr)					return nullptr;
	if (dal_string_compare(result->_key, path, result->_key_len) == FALSE)		return nullptr;

	return result;
};

dal_t*	dal_t::get_child(const char* key, uint32_t len)
{
	//Calculate sdbm hash of specified key value
	uint32_t	hash	= dal_hash_sdbm(key, len);
	//Find child with specified key value, prefer checking hash
	for (dal_t* child = this->_child; child != nullptr; child = child->_next)
	{
		if (child->_key_hash != hash)	continue;
		if (dal_string_compare(child->_key, key, child->_key_len) == TRUE)		return child;
	}
	return nullptr;
};

dal_t*	dal_t::get_child(const char* key)
{
	return get_child(key, DAL_KEY_SIZE);
};

dal_t*	dal_t::get_child(uint32_t idx)
{
	dal_t*		child	= this->_child;
	while ((child != nullptr) && (idx > 0))
	{
		child			= child->_next;
		idx--;
	}
	return child;
};

dal_t*	dal_t::get_child()
{
	return this->_child;
};

bool	dal_t::get_child(const char* key, dal_t*& value)
{
	dal_t*	child	= get_child(key);
	if (child != nullptr)
	{		value	= child;
			return true;
	}else	return false;
};

bool	dal_t::has_child(const char* key)
{
	//Calculate sdbm hash of specified key value
	uint32_t	hash	= dal_hash_sdbm(key, DAL_KEY_SIZE);
	//Find child with specified key value, prefer checking hash
	dal_t*		child	= this->_child;
	while (child != nullptr)
	{
		if (child->_key_hash == hash)
		{
			if (dal_string_compare(child->_key, key, child->_key_len) == TRUE)		return true;
		}
		child			= child->_next;
	}
	return false;
};

uint32_t	dal_t::count_childs()
{
	uint32_t	result	= 0;
	dal_t*		child	= this->_child;
	while (child != nullptr)
	{
		result++;
		child			= child->_next;
	}
	return result;
};
//===============================================================================================================================

