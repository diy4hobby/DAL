#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"



//===============================================================================================================================

dal_t*	dal_t::create_child()
{
	dal_t*		newChild	= dal_create();
	newChild->_next			= this->_child;
	this->_child			= newChild;
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

