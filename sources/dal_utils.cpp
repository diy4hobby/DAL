#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"


dalMemHooks_t		_dal_memHooks		= {nullptr};


//===============================================================================================================================
dalResult_e	dal_init(dalMemHooks_t* mem)
{
	//Set user-supplied memory allocation and deallocation methods
	_dal_memHooks		= *mem;

	return DAL_OK;
};

//Creating an empty object
dal_t*	dal_create(dalNodeType_e type)
{
	//Allocate memory for new object and check is really allocated
	dal_t*	node		= _dal_memHooks.alloc_node();
	if (node == nullptr)	return nullptr;
	dal_bytedata_zero(node->_key, DAL_KEY_SIZE);
	node->_key_len		= 0;
	node->_key_hash		= 0;
	node->_parent		= nullptr;
	node->_child		= nullptr;
	node->_last			= nullptr;
	node->_prev			= nullptr;
	node->_next			= nullptr;
	//Set node type - object
	node->_type			= type;
	node->_size			= 0;
	return node;
};

//Delete object and all childs
void	dal_delete(dal_t* node)
{
	if (node == nullptr)		return;

	//Making sure the node is removed from the parent object
	node->detach();

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
		case DT_ARRAY:
		case DT_OBJECT:{	dal_t*	child		= node->_child;
							while (child != NULL)
							{	dal_t*	next	= child->_next;
								dal_delete(child);
								child			= next;
							}
							_dal_memHooks.free_node(node);
							break;
						}
	}
}
//===============================================================================================================================
