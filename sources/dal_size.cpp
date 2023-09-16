#include "dal_struct.h"
#include "dal_utils.h"



//===============================================================================================================================
uint32_t	_dal_size_recurse(dal_t* node)
{
	if (node == nullptr)		return 0;
	uint32_t	size		= 0;

	if (node->_child != nullptr)
	{
		dal_t*	child		= node->_child;
		while (child != nullptr)
		{
			size			+= _dal_size_recurse(child);
			child			= child->_next;
		}
	}

	switch (node->_type)
	{
		case DT_UNKN:
		case DT_BOOL:		return (2 + sizeof(bool) + node->_key_len);
		case DT_UINT:		return (2 + sizeof(uint64_t) + node->_key_len);
		case DT_INT:		return (2 + sizeof(int64_t) + node->_key_len);
		case DT_DOUBLE:		return (2 + sizeof(double) + node->_key_len);
		case DT_STRING:		return (2 + node->_size + node->_key_len);
		case DT_BLOB:		return (2 + node->_size + node->_key_len);
		case DT_BLOB_REF:	return (2 + node->_size + node->_key_len);
		case DT_ARRAY:		return (2 + size + node->_key_len);
		case DT_OBJECT:		return (2 + size + node->_key_len);
	}

	return size;
}

uint32_t	dal_t::size()
{
	return _dal_size_recurse(this);
};
//===============================================================================================================================
