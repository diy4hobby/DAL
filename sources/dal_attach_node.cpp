#include "dal_struct.h"
#include "dal_common/dal_common.h"



//===============================================================================================================================
dal_t*	dal_t::attach(dalStr_t* key, dal_t* node)
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

dal_t*	dal_t::attach(char* key, dal_t* node)
{
	dalStr_t	dalStrKey;
	dalStrKey.data			= key;
	dalStrKey.size			= dal_string_length(key, DAL_KEY_SIZE);
	return this->attach(&dalStrKey, node);
};

dal_t*	dal_t::attach(dal_t* node)
{
	if (node == nullptr)				return nullptr;

	node->detach();
	node->_parent			= this;
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
