#include "../dal_struct.h"
#include "dal_serializer.h"


dalResult_e	_dal_serialize_recurse(dalSerializer_t* ser, dal_t* node)
{
	dalResult_e		result	= DAL_OK;
	dalStr_t		valStr;
	dalBlob_t		valBlob;
	uint32_t		itemsCount;
	dal_t*			child;

	if (node == nullptr)	return DAL_OK;

	if (node->_key_len != 0)
	{
		dalStr_t	key;
		key.data	= node->_key;
		key.size	= node->_key_len;
		result		= ser->wr_key(ser->context, &ser->buf, &key);
		if (result != DAL_OK)		return result;
	}

	switch (node->_type)
	{
		case DT_UNKN:
			return DAL_FORMAT_ERR;

		case DT_BOOL:
			result		= ser->wr_bool(ser->context, &ser->buf, node->_as_bool);
			if (result != DAL_OK)	return result;
			break;

		case DT_UINT:
			result		= ser->wr_uint(ser->context, &ser->buf, node->_as_uint);
			if (result != DAL_OK)	return result;
			break;

		case DT_INT:
			result		= ser->wr_int(ser->context, &ser->buf, node->_as_int);
			if (result != DAL_OK)	return result;
			break;

		case DT_DOUBLE:
			result		= ser->wr_dbl(ser->context, &ser->buf, node->_as_dbl);
			if (result != DAL_OK)	return result;
			break;

		case DT_STRING:
			valStr.data	= node->_as_str;
			valStr.size	= node->_size;
			result		= ser->wr_str(ser->context, &ser->buf, &valStr);
			if (result != DAL_OK)	return result;
			break;
		case DT_BLOB: case DT_BLOB_REF:
			valBlob.data	= node->_as_blob;
			valBlob.size	= node->_size;
			result			= ser->wr_blob(ser->context, &ser->buf, &valBlob);
			if (result != DAL_OK)	return result;
			break;

		case DT_ARRAY:
			itemsCount	= node->_size;
			result		= ser->wr_arr_beg(ser->context, &ser->buf, itemsCount);
			if (result != DAL_OK)	return result;
			child		= node->_child;
			for (uint32_t idx = 0; idx < itemsCount; idx++)
			{
				result		= _dal_serialize_recurse(ser, child);
				if (result != DAL_OK)			return result;
				child		= child->_next;
			}
			result		= ser->wr_arr_end(ser->context, &ser->buf);
			if (result != DAL_OK)	return result;
			break;

		case DT_OBJECT:
			itemsCount	= node->count_childs();
			result		= ser->wr_obj_beg(ser->context, &ser->buf, itemsCount);
			if (result != DAL_OK)	return result;
			child		= node->_child;
			for (uint32_t idx = 0; idx < itemsCount; idx++)
			{
				result		= _dal_serialize_recurse(ser, child);
				if (result != DAL_OK)			return result;
				child		= child->_next;
			}
			result		= ser->wr_obj_end(ser->context, &ser->buf);
			if (result != DAL_OK)	return result;
			break;
	}

	return result;
}
