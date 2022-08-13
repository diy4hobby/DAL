#include "../dal_struct.h"
#include "mpack/mpack_common.h"
#include <limits>


//===============================================================================================================================







dalResult_e	_dal_to_mpack_recurse(uint8_t** buf, uint32_t* available, dal_t* node)
{
	dalResult_e		result	= DAL_OK;
	mpackResult_e	wrResult;
	dalStr_t		key;
	double			valDbl;
	dalStr_t		valStr;
	dalBlob_t		valBlob;
	uint32_t		itemsCount;

	if (node == nullptr)	return DAL_OK;

	key				= node->key();
	if (key.size != 0)
	{
		wrResult	= mpack_write_str(buf, available, key.data, key.size);
		if (wrResult != MPACK_OK)		return DAL_MEM_ERR;
	}

	switch (node->type())
	{
		case DT_UNKN:
			wrResult		= mpack_write_nil(buf, available);
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			break;
		case DT_BOOL:
			wrResult		= mpack_write_bool(buf, available, node->value(false));
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			break;
		case DT_UINT:
			wrResult		= mpack_write_uint(buf, available, node->value(static_cast<uint64_t>(0)));
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			break;
		case DT_INT:
			wrResult		= mpack_write_int(buf, available, node->value(static_cast<int64_t>(0)));
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			break;
		case DT_DOUBLE:
			valDbl			= node->value(static_cast<double>(0.0));
			if ((valDbl > std::numeric_limits<float>::max()) || (valDbl < -std::numeric_limits<float>::max()))
			{
				wrResult	= mpack_write_dbl(buf, available, valDbl);
			}else
			{
				wrResult	= mpack_write_flt(buf, available, valDbl);
			}
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			break;
		case DT_STRING:
			valStr			= node->value(static_cast<dalStr_t*>(nullptr));
			wrResult		= mpack_write_str(buf, available, valStr.data, valStr.size);
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			break;
		case DT_BLOB: case DT_BLOB_REF:
			valBlob			= node->value(static_cast<dalBlob_t*>(nullptr));
			wrResult		= mpack_write_blob(buf, available, valBlob.data, valBlob.size);
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
		break;
		case DT_ARRAY:
			itemsCount		= node->get_array_size();
			wrResult		= mpack_write_arr_beg(buf, available, itemsCount);
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			for (uint32_t idx = 0; idx < itemsCount; idx++)
			{
				result		= _dal_to_mpack_recurse(buf, available, node->get_array_item(idx));
				if (result != DAL_OK)			return DAL_MEM_ERR;
			}
			wrResult		= mpack_write_arr_end(buf, available);
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			break;
		case DT_OBJECT:
			itemsCount		= node->count_childs();
			wrResult		= mpack_write_map_beg(buf, available, itemsCount);
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			for (uint32_t idx = 0; idx < itemsCount; idx++)
			{
				result		= _dal_to_mpack_recurse(buf, available, node->get_child(idx));
				if (result != DAL_OK)			return DAL_MEM_ERR;
			}
			wrResult		= mpack_write_map_end(buf, available);
			if (wrResult != MPACK_OK)			return DAL_MEM_ERR;
			break;
	}

	return result;
}
/*
uint32_t	dal_t::to_mpack(uint8_t* buf, uint32_t size)
{
	uint32_t	available			= size;
	dalResult_e				result	= _dal_to_mpack_recurse(&buf, &available, this);
	if (result == DAL_OK)	return (size - available);
	else					return 0;
};
*/
//===============================================================================================================================
