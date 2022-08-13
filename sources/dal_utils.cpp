#include "dal_struct.h"
#include "dal_utils.h"


dalMemHooks_t		_dal_memHooks		= {nullptr};


//===============================================================================================================================
dalResult_e	dal_init(dalMemHooks_t* mem)
{
	//Set user-supplied memory allocation and deallocation methods
	_dal_memHooks		= *mem;

	return DAL_OK;
};

//Creating an empty object
dal_t*	dal_create()
{
	//Allocate memory for new object and check is really allocated
	dal_t*	node		= _dal_memHooks.alloc_node();
	if (node == nullptr)	return nullptr;
	node->_key[0]		= 0x00;
	node->_key_len		= 0;
	node->_key_hash		= 0;
	node->_parent		= nullptr;
	node->_child		= nullptr;
	node->_prev			= nullptr;
	node->_next			= nullptr;
	//Set node type - object
	node->_type			= DT_OBJECT;
	node->_size			= 0;
	return node;
};

//Delete object and all childs
void	dal_delete(dal_t* node)
{
	if (node == nullptr)		return;

	//For arrays, the removing of child elements is done separately, because pointer to child element
	//actually points to the memory allocated for the array of nodes
	if (node->_type != DT_ARRAY)
	{
		if (node->_child != nullptr)
		{
			dal_t*	child		= node->_child;
			while (child != nullptr)
			{	dal_t*	next	= child->_next;
				dal_delete(child);
				child			= next;
			}
		}
	}

	switch (node->_type)
	{
		case DT_UNKN:
		case DT_BOOL:
		case DT_UINT:
		case DT_INT:
		case DT_DOUBLE:		_dal_memHooks.free_node(node);
							break;
		case DT_STRING:		_dal_memHooks.free_data(node->_mem_ptr);
							_dal_memHooks.free_node(node);
							break;
		case DT_BLOB:		_dal_memHooks.free_data(node->_mem_ptr);
							_dal_memHooks.free_node(node);
							break;
		case DT_BLOB_REF:	_dal_memHooks.free_node(node);			break;
		case DT_ARRAY:		for (uint32_t idx = 0; idx < node->_size; idx++)
							{
								dal_t*	child	= node->_child[idx]._child;
								while (child != nullptr)
								{
									dal_t*	next	= child->_next;
									dal_delete(child);
									child			= next;
								}
							}
							_dal_memHooks.free_data(node->_child);
							_dal_memHooks.free_node(node);
							break;
		case DT_OBJECT:		_dal_memHooks.free_node(node);
							break;
	}
}
//===============================================================================================================================
