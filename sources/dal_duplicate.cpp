#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"



//===============================================================================================================================
dal_t*	_dal_duplicate_recurse(dal_t* src, dal_t* parent)
{
	if (src == nullptr)							return parent;

	dal_t*		child;
	dal_t*		srcItem;
	dal_t*		dstItem;
	dal_t*		srcChild;
	uint32_t	count;

	switch (src->_type)
	{
		case DT_UNKN:	return parent;
		case DT_BOOL:
		case DT_UINT:
		case DT_INT:
		case DT_DOUBLE:
		case DT_STRING:
		case DT_BLOB:
		case DT_BLOB_REF:
			parent->_copy_trivial(src);
			return parent;
		case DT_ARRAY:
			if (parent == nullptr)	return parent;
			parent->_key_len	= src->_key_len;
			parent->_key_hash	= src->_key_hash;
			dal_bytedata_copy(parent->_key, src->_key, src->_key_len);
			parent->convert_to_array(src->_size);
			srcItem				= src->_child;
			dstItem				= parent->_child;
			count				= src->_size;
			while (count > 0)
			{
				_dal_duplicate_recurse(srcItem, dstItem);
				srcItem++;
				dstItem++;
				count--;
			}
			return parent;
		case DT_OBJECT:
			if (parent == nullptr)	return parent;
			parent->_key_len	= src->_key_len;
			parent->_key_hash	= src->_key_hash;
			dal_bytedata_copy(parent->_key, src->_key, src->_key_len);
			srcChild			= src->_child;
			while (srcChild != nullptr)
			{
				child			= parent->create_child();
				if (child == nullptr)	return parent;
				_dal_duplicate_recurse(srcChild, child);
				srcChild		= srcChild->_next;
			}
			return parent;
	}

	return parent;
}

dal_t*	dal_t::duplicate()
{
	dal_t*	obj		= dal_create();
	return _dal_duplicate_recurse(this, obj);
};
//===============================================================================================================================
