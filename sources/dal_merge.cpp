#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"

extern dalMemHooks_t		_dal_memHooks;

//===============================================================================================================================
void	_dal_merge_recurse(dal_t* first, dal_t* second)
{
	if (first->_type == second->_type)
	{
		switch (first->_type)
		{
			case DT_UNKN:	return;
			
			case DT_BOOL:	first->_as_bool		= second->_as_bool;		return;
			
			case DT_UINT:	first->_as_uint		= second->_as_uint;		return;
			
			case DT_INT:	first->_as_int		= second->_as_int;		return;
			
			case DT_DOUBLE:	first->_as_dbl		= second->_as_dbl;		return;
			
			case DT_STRING:
				if (first->_size != second->_size)
				{	_dal_memHooks.free_data(first->_mem_ptr);
					first->_mem_ptr		= _dal_memHooks.alloc_data(second->_size + 8);
					if (first->_mem_ptr == nullptr)		return;
				}
				first->_as_str			= reinterpret_cast<char*>(dal_pointer_align_8(first->_mem_ptr));
				dal_bytedata_copy(first->_as_str, second->_as_str, second->_size);
				first->_size			= second->_size;
			return;
			
			case DT_BLOB:
				if (first->_size != second->_size)
				{	_dal_memHooks.free_data(first->_mem_ptr);
					first->_mem_ptr		= _dal_memHooks.alloc_data(second->_size + 8);
					if (first->_mem_ptr == nullptr)		return;
				}
				first->_as_blob			= reinterpret_cast<uint8_t*>(dal_pointer_align_8(first->_mem_ptr));
				dal_bytedata_copy(first->_as_blob, second->_as_blob, second->_size);
				first->_size			= second->_size;
			return;
			
			case DT_BLOB_REF:	first->_as_blob		= second->_as_blob;		return;
			
			case DT_ARRAY:{
				dal_t*	firstItem		= first->_child;
				dal_t*	secondItem		= second->_child;
				while ((firstItem != nullptr) && (secondItem != nullptr))
				{
					_dal_merge_recurse(firstItem, secondItem);
					firstItem			= firstItem->_next;
					secondItem			= secondItem->_next;
				}
				while (firstItem != nullptr)
				{
					dal_t*	next	= firstItem->next();
					dal_delete(firstItem);
					firstItem		= next;
				}
				while (secondItem != nullptr)
				{
					dal_t*	newItem	= secondItem->duplicate();
					first->attach(newItem);
					secondItem		= secondItem->next();
				}
			return;}
			
			case DT_OBJECT:{
				dal_t*	secondChild	= second->_child;
				while (secondChild != nullptr)
				{
					dal_t*	firstChild	= first->get_child(secondChild->_key);
					if (firstChild == nullptr)
					{	
						first->attach(secondChild->duplicate());
					}else{
						_dal_merge_recurse(firstChild, secondChild);
					}
					secondChild		= secondChild->_next;
				}
			return;}
		}
	}else
	{
		dal_t*	parent	= first->_parent;
		if (parent == nullptr)		return;
		dal_delete(first);
		parent->attach(second->duplicate());
	}
}

void	dal_t::merge(dal_t* node)
{
	if (node == nullptr)	return;
	_dal_merge_recurse(this, node);
};
//===============================================================================================================================
