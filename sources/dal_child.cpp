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

dal_t*	dal_t::create_child(dalNodeType_e type)
{
	dal_t*		newChild	= dal_create(type);
	if (newChild == nullptr)					return nullptr;
	if (this->_add_item(newChild) == DAL_OK)	return newChild;
	dal_delete(newChild);
	return nullptr;
};

dal_t*	dal_t::create_child(const char* key, dalNodeType_e type)
{
	dal_t*		newChild	= create_child(type);
	if (newChild == nullptr)	return nullptr;
	newChild->rename(key);
	return newChild;
};

dal_t*	dal_t::get_child(const char* key)
{
	//Calculate sdbm hash of specified key value
	uint32_t	hash	= dal_hash_sdbm(key, DAL_KEY_SIZE);
	//Find child with specified key value, prefer checking hash
	dal_t*		child	= this->_child;
	while (child != nullptr)
	{
		if (child->_key_hash == hash)
		{
			if (dal_string_compare(child->_key, key, child->_key_len) == TRUE)		break;
		}
		child			= child->_next;
	}
	return child;
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

dal_t*	dal_t::get_next()
{
	return this->_next;
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

