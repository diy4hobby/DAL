#include "dal_struct.h"
#include "dal_utils.h"


//===============================================================================================================================


template<> bool	dal_t::get(bool& value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	
	switch (this->_type)
	{
		default:			return false;
		case DT_BOOL:		value	= this->_as_bool;			break;
		case DT_UINT:		value	= (this->_as_uint != 0);	break;
		case DT_INT:		value	= (this->_as_int > 0);		break;
		case DT_DOUBLE:		value	= (this->_as_dbl > 0);		break;
	}
	return true;
};

template<> bool	dal_t::get(uint8_t& value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	
	switch (this->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_as_bool;		break;
		case DT_UINT:	value	= this->_as_uint;		break;
		case DT_INT:	value	= this->_as_int;		break;
		case DT_DOUBLE:	if (this->_as_dbl != this->_as_dbl)		return false;
						value	= this->_as_dbl;
						break;
	}
	return true;
};

template<> bool	dal_t::get(uint16_t& value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	
	switch (this->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_as_bool;		break;
		case DT_UINT:	value	= this->_as_uint;		break;
		case DT_INT:	value	= this->_as_int;		break;
		case DT_DOUBLE:	if (this->_as_dbl != this->_as_dbl)		return false;
						value	= this->_as_dbl;
						break;
	}
	return true;
};

template<> bool	dal_t::get(int& value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	
	switch (this->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_as_bool;		break;
		case DT_UINT:	value	= this->_as_uint;		break;
		case DT_INT:	value	= this->_as_int;		break;
		case DT_DOUBLE:	if (this->_as_dbl != this->_as_dbl)		return false;
						value	= this->_as_dbl;
						break;
	}
	return true;
};

template<> bool	dal_t::get(int64_t& value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	
	switch (this->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_as_bool;		break;
		case DT_UINT:	value	= this->_as_uint;		break;
		case DT_INT:	value	= this->_as_int;		break;
		case DT_DOUBLE:	if (this->_as_dbl != this->_as_dbl)		return false;
						value	= this->_as_dbl;
						break;
	}
	return true;
};

template<> bool	dal_t::get(unsigned int& value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	
	switch (this->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_as_bool;		break;
		case DT_UINT:	value	= this->_as_uint;		break;
		case DT_INT:	value	= this->_as_int;		break;
		case DT_DOUBLE:	if (this->_as_dbl != this->_as_dbl)		return false;
						value	= this->_as_dbl;
						break;
	}
	return true;
};

template<> bool	dal_t::get(uint64_t& value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	
	switch (this->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_as_bool;			break;
		case DT_UINT:	value	= this->_as_uint;			break;
		case DT_INT:	value	= this->_as_int;			break;
		case DT_DOUBLE:	if (this->_as_dbl != this->_as_dbl)		return false;
						value	= this->_as_dbl;
						break;
	}
	return true;
};

template<> bool	dal_t::get(double& value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	
	switch (this->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_as_bool;		break;
		case DT_UINT:	value	= this->_as_uint;		break;
		case DT_INT:	value	= this->_as_int;		break;
		case DT_DOUBLE:	value	= this->_as_dbl;		break;
	}
	return true;
};

template<> bool	dal_t::get(float& value, dalNodeType_e type)
{
	double	val		= value;
	bool	result	= this->get<double>(val, type);
	value			= val;
	return result;
};

template<> bool	dal_t::get(dal_t*& value, dalNodeType_e type)
{
	if (!(this->_type & TUPLE_TYPE & type))		return false;
	value	= this;
	return true;
};

template<> bool	dal_t::get(dalStr_t& value, dalNodeType_e type)
{
	if (!(this->_type & DT_STRING & type))		return false;

	value.data	= this->_as_str;
	value.size	= this->_size;
	return true;
};

template<> bool	dal_t::get(dalBlobRef_t& value, dalNodeType_e type)
{
	if (!(this->_type & (DT_BLOB | DT_BLOB_REF) & type))		return false;

	value.data	= this->_as_blob;
	value.size	= this->_size;
	return true;
};


//===============================================================================================================================


template<> bool	dal_t::get(const char* key, bool& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	return child->get(value);
};

template<> bool	dal_t::get(const char* key, uint8_t& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	return child->get(value);
};

template<> bool	dal_t::get(const char* key, uint16_t& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	return child->get(value);
};

template<> bool	dal_t::get(const char* key, int& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	return child->get(value);
};

template<> bool	dal_t::get(const char* key, int64_t& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	return child->get(value);
};

template<> bool	dal_t::get(const char* key, unsigned int& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	return child->get(value);
};

template<> bool	dal_t::get(const char* key, uint64_t& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	return child->get(value);
};

template<> bool	dal_t::get(const char* key, double& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	return child->get(value);
};

template<> bool	dal_t::get(const char* key, float& value, dalNodeType_e type)
{
	double	val		= value;
	bool	result	= this->get<double>(key, val);
	value			= val;
	return result;
};

template<> bool	dal_t::get(const char* key, dal_t*& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	if (child->_type & type)
	{		value	= child;
			return true;
	}else	return false;
};

template<> bool	dal_t::get(const char* key, dalStr_t& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)	return false;
	return child->get(value);
};

template<> bool	dal_t::get(const char* key, dalBlobRef_t& value, dalNodeType_e type)
{
	dal_t*	child	= this->_get_child_by_path(key);
	if (child == nullptr)		return false;
	return child->get(value);
};


//===============================================================================================================================


bool	dal_t::get_val_bool(const char* key, bool& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)							return false;
	
	if (!(child->_type & NUMBER_TYPE))				return false;
	switch (child->_type)
	{
		default:			return false;
		case DT_BOOL:		value	= child->_as_bool;			break;
		case DT_UINT:		value	= (child->_as_uint != 0);	break;
		case DT_INT:		value	= (child->_as_int > 0);		break;
	}

	return true;
};

bool	dal_t::get_val_int(const char* key, int& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)							return false;
	if (!(child->_type & NUMBER_TYPE))				return false;
	
	int64_t		tempVal;

	switch (child->_type)
	{
		default:		return false;
		case DT_BOOL:	tempVal	= child->_as_bool;		break;
		case DT_UINT:	if (child->_as_uint & 0x8000000000000000)	return false;
						else										tempVal	= child->_as_uint;
						break;
		case DT_INT:	tempVal	= child->_as_int;		break;
		case DT_DOUBLE:	if (child->_as_dbl != child->_as_dbl)		return false;
						tempVal	= static_cast<int64_t>(child->_as_dbl >= 0.0
														? child->_as_dbl + 0.5
														: child->_as_dbl - 0.5);
						break;
	}

	switch (sizeof(int))
	{
		default:	return false;
		case 2:		if (tempVal > 32767)				return false;
					else	if (tempVal < -32768)		return false;
							else						value	= static_cast<int>(tempVal);
					break;
		case 4:		if (tempVal > 2147483647)			return false;
					else	if (tempVal < -2147483647)	return false;
							else						value	= static_cast<int>(tempVal);
					break;
		case 8:		value	= static_cast<int>(tempVal);
					break;
	}

	return true;
};

bool	dal_t::get_val_uint(const char* key, unsigned int& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)							return false;
	if (!(child->_type & NUMBER_TYPE))				return false;
	
	uint64_t	tempVal;

	switch (child->_type)
	{
		default:		return false;
		case DT_BOOL:	tempVal	= child->_as_bool;		break;
		case DT_UINT:	tempVal	= child->_as_uint;		break;
		case DT_INT:	if (child->_as_uint & 0x8000000000000000)	return false;
						else										tempVal	= child->_as_int;
						break;
		case DT_DOUBLE:	if (child->_as_dbl != child->_as_dbl)		return false;
						if (child->_as_dbl < 0.0)					return false;
						else	tempVal	= static_cast<uint64_t>(child->_as_dbl + 0.5);
						break;
	}

	switch (sizeof(unsigned int))
	{
		default:	return false;
		case 2:		if (tempVal > 65535)				return false;
					else								value	= static_cast<unsigned int>(tempVal);
					break;
		case 4:		if (tempVal > 0xFFFFFFFF)			return false;
					else								value	= static_cast<unsigned int>(tempVal);
					break;
		case 8:		value	= static_cast<unsigned int>(tempVal);
					break;
	}

	return true;
};

bool	dal_t::get_val_int32(const char* key, int32_t& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)							return false;
	if (!(child->_type & NUMBER_TYPE))				return false;
	
	int64_t		tempVal;

	switch (child->_type)
	{
		default:		return false;
		case DT_BOOL:	tempVal	= child->_as_bool;		break;
		case DT_UINT:	if (child->_as_uint & 0x8000000000000000)	return false;
						else										tempVal	= child->_as_uint;
						break;
		case DT_INT:	tempVal	= child->_as_int;		break;
		case DT_DOUBLE:	if (child->_as_dbl != child->_as_dbl)		return false;
						tempVal	= static_cast<int64_t>(child->_as_dbl >= 0.0
														? child->_as_dbl + 0.5
														: child->_as_dbl - 0.5);
						break;
	}

	if (tempVal > 2147483647)			return false;
	else	if (tempVal < -2147483648)	return false;
			else						value	= static_cast<int32_t>(tempVal);

	return true;
};

bool	dal_t::get_val_uint32(const char* key, uint32_t& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)							return false;
	if (!(child->_type & NUMBER_TYPE))				return false;
	
	uint64_t	tempVal;

	switch (child->_type)
	{
		default:		return false;
		case DT_BOOL:	tempVal	= child->_as_bool;		break;
		case DT_UINT:	tempVal	= child->_as_uint;		break;
		case DT_INT:	if (child->_as_int & 0x8000000000000000)	return false;
						else										tempVal	= child->_as_int;
						break;
		case DT_DOUBLE:	if (child->_as_dbl != child->_as_dbl)		return false;
						if (child->_as_dbl < 0.0)					return false;
						else	tempVal	= static_cast<uint64_t>(child->_as_dbl + 0.5);
						break;
	}

	if (tempVal > 0xFFFFFFFF)			return false;
	else								value	= static_cast<uint32_t>(tempVal);

	return true;
};

bool	dal_t::get_val_int64(const char* key, int64_t& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return false;
	if (!(child->_type & NUMBER_TYPE))	return false;
	
	switch (child->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= child->_as_bool;		break;
		case DT_UINT:	if (child->_as_uint & 0x8000000000000000)	return false;
						else										value	= child->_as_uint;
						break;
		case DT_INT:	value	= child->_as_int;		break;
		case DT_DOUBLE:	if (child->_as_dbl != child->_as_dbl)		return false;
						if (child->_as_dbl < INT64_MIN)				return false;
						if (child->_as_dbl > INT64_MAX)				return false;
						value	= static_cast<int64_t>(child->_as_dbl >= 0.0
														? child->_as_dbl + 0.5
														: child->_as_dbl - 0.5);
						break;
	}

	return true;
};

bool	dal_t::get_val_uint64(const char* key, uint64_t& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return false;
	if (!(child->_type & NUMBER_TYPE))	return false;
	
	switch (child->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= child->_as_bool;			break;
		case DT_UINT:	value	= child->_as_uint;			break;
		case DT_INT:	if (child->_as_int & 0x8000000000000000)	return false;
						else										value	= child->_as_int;
						break;
		case DT_DOUBLE:	if (child->_as_dbl != child->_as_dbl)		return false;
						if (child->_as_dbl < 0.0)					return false;
						if (child->_as_dbl > INT64_MAX)				return false;
						value	= static_cast<uint64_t>(child->_as_dbl >= 0.0
														? child->_as_dbl + 0.5
														: child->_as_dbl - 0.5);
						break;
	}

	return true;
};

bool	dal_t::get_val_flt(const char* key, float& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return false;
	if (!(child->_type & NUMBER_TYPE))	return false;
	
	switch (child->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= child->_as_bool;							break;
		case DT_UINT:	value	= static_cast<float>(child->_as_uint);		break;
		case DT_INT:	value	= static_cast<float>(child->_as_int);		break;
		case DT_DOUBLE:	if (child->_as_dbl != child->_as_dbl)	value	= 0x7fc00000;	//NAN
						else									value	= static_cast<float>(child->_as_dbl);
						break;
	}
	
	return true;
};

bool	dal_t::get_val_dbl(const char* key, double& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return false;
	if (!(child->_type & NUMBER_TYPE))	return false;
	
	switch (child->_type)
	{
		default:		return false;
		case DT_BOOL:	value	= child->_as_bool;							break;
		case DT_UINT:	value	= static_cast<double>(child->_as_uint);		break;
		case DT_INT:	value	= static_cast<double>(child->_as_int);		break;
		case DT_DOUBLE:	value	= child->_as_dbl;		break;
	}
	
	return true;
};

bool	dal_t::get_val_str(const char* key, const char*& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return false;
	if (!(child->_type & DT_STRING))	return false;
	
	value	= child->_as_str;
	
	return true;
};

bool	dal_t::get_val_str(const char* key, char*& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return false;
	if (!(child->_type & DT_STRING))	return false;
	
	value	= child->_as_str;
	
	return true;
};
	
bool	dal_t::get_val_str(const char* key, dalStr_t& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return false;
	if (!(child->_type & DT_STRING))	return false;
	
	value.data	= child->_as_str;
	value.size	= child->_size;
	
	return true;
};

bool	dal_t::get_val_blob(const char* key, dalBlob_t& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return false;
	if (!(child->_type & DT_BLOB))		return false;
	
	value.data	= child->_as_blob;
	value.size	= child->_size;
	
	return true;
};

bool	dal_t::get_val_blob(const char* key, dalBlobRef_t& value)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return false;
	if (!(child->_type & DT_BLOB_REF))	return false;
	
	value.data	= child->_as_blob;
	value.size	= child->_size;
	
	return true;
};
//===============================================================================================================================


//===============================================================================================================================
bool	dal_t::get_val_bool(uint32_t idx, bool& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & NUMBER_TYPE))		return false;

	switch (this->_child[idx]._type)
	{
		default:			return false;
		case DT_BOOL:		value	= this->_as_bool;			break;
		case DT_UINT:		value	= (this->_as_uint != 0);	break;
		case DT_INT:		value	= (this->_as_int > 0);		break;
		case DT_DOUBLE:		if (this->_child[idx]._as_dbl != this->_child[idx]._as_dbl)		return false;
							value	= (this->_as_dbl > 0.0);	break;
	}

	return true;
};

bool	dal_t::get_val_int(uint32_t idx, int& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & NUMBER_TYPE))		return false;
	int64_t		tempVal;

	switch (this->_child[idx]._type)
	{
		default:		return false;
		case DT_BOOL:	tempVal	= this->_child[idx]._as_bool;		break;
		case DT_UINT:	if (this->_child[idx]._as_uint & 0x8000000000000000)	return false;
						else													tempVal	= this->_child[idx]._as_uint;
						break;
		case DT_INT:	tempVal	= this->_child[idx]._as_int;		break;
		case DT_DOUBLE:	if (this->_child[idx]._as_dbl != this->_child[idx]._as_dbl)		return false;
						tempVal	= static_cast<int64_t>(this->_child[idx]._as_dbl >= 0.0
														? this->_child[idx]._as_dbl + 0.5
														: this->_child[idx]._as_dbl - 0.5);
						break;
	}

	switch (sizeof(int))
	{
		default:		return false;
		case 2:			if (tempVal > 32767)				return false;
						else	if (tempVal < -32768)		return false;
								else						value	= static_cast<int>(tempVal);
						break;
		case 4:			if (tempVal > 2147483647)			return false;
						else	if (tempVal < -2147483647)	return false;
								else						value	= static_cast<int>(tempVal);
						break;
		case 8:			value	= static_cast<int>(tempVal);	break;
	}

	return true;
};

bool	dal_t::get_val_uint(uint32_t idx, unsigned int& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & NUMBER_TYPE))		return false;
	uint64_t	tempVal;

	switch (this->_child[idx]._type)
	{
		default:		return false;
		case DT_BOOL:	tempVal	= this->_child[idx]._as_bool;		break;
		case DT_UINT:	tempVal	= this->_child[idx]._as_uint;		break;
		case DT_INT:	if (this->_child[idx]._as_int & 0x8000000000000000)		return false;
						else	tempVal	= this->_child[idx]._as_int;
						break;
		case DT_DOUBLE:	if (this->_child[idx]._as_dbl != this->_child[idx]._as_dbl)		return false;
						if (this->_child[idx]._as_dbl < 0.0)							return false;
						else	tempVal	= static_cast<uint64_t>(this->_child[idx]._as_dbl + 0.5);
						break;
	}

	switch (sizeof(int))
	{
		default:		return false;
		case 2:			if (tempVal > 65535)				return false;
						else	value	= static_cast<unsigned int>(tempVal);
						break;
		case 4:			if (tempVal > 0xFFFFFFFF)			return false;
						else	value	= static_cast<unsigned int>(tempVal);
						break;
		case 8:			value	= static_cast<unsigned int>(tempVal);	break;
	}

	return true;
};

bool	dal_t::get_val_int32(uint32_t idx, int32_t& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & NUMBER_TYPE))		return false;
	
	int64_t		tempVal;

	switch (this->_child[idx]._type)
	{
		default:		return false;
		case DT_BOOL:	tempVal	= this->_child[idx]._as_bool;	break;
		case DT_UINT:	if (this->_child[idx]._as_uint & 0x8000000000000000)	return false;
						else	tempVal	= this->_child[idx]._as_uint;
						break;
		case DT_INT:	tempVal	= this->_child[idx]._as_int;	break;
		case DT_DOUBLE:	if (this->_child[idx]._as_dbl != this->_child[idx]._as_dbl)		return false;
						tempVal	= static_cast<int64_t>(this->_child[idx]._as_dbl >= 0.0
														? this->_child[idx]._as_dbl + 0.5
														: this->_child[idx]._as_dbl - 0.5);
						break;
	}

	if (tempVal > 2147483647)			return false;
	else	if (tempVal < -2147483648)	return false;
			else						value	= static_cast<int32_t>(tempVal);

	return true;
};

bool	dal_t::get_val_uint32(uint32_t idx, uint32_t& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & NUMBER_TYPE))		return false;
	
	uint64_t	tempVal;

	switch (this->_child[idx]._type)
	{
		default:		return false;
		case DT_BOOL:	tempVal	= this->_child[idx]._as_bool;	break;
		case DT_UINT:	tempVal	= this->_child[idx]._as_uint;
						break;
		case DT_INT:	if (this->_child[idx]._as_int & 0x8000000000000000)		return false;
						else													tempVal	= this->_child[idx]._as_int;
						break;
		case DT_DOUBLE:	if (this->_child[idx]._as_dbl != this->_child[idx]._as_dbl)		return false;
						if (this->_child[idx]._as_dbl < 0.0)							return false;
						else	tempVal	= static_cast<uint64_t>(this->_child[idx]._as_dbl + 0.5);
						break;
	}

	if (tempVal > 0xFFFFFFFF)			return false;
	else								value	= static_cast<uint32_t>(tempVal);

	return true;
};

bool	dal_t::get_val_int64(uint32_t idx, int64_t& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & NUMBER_TYPE))		return false;
	
	switch (this->_child[idx]._type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_child[idx]._as_bool;		break;
		case DT_UINT:	if (this->_child[idx]._as_uint & 0x8000000000000000)	return false;
						else	value	= this->_child[idx]._as_uint;
						break;
		case DT_INT:	value	= this->_child[idx]._as_int;		break;
		case DT_DOUBLE:	if (this->_child[idx]._as_dbl != this->_child[idx]._as_dbl)		return false;
						if (this->_child[idx]._as_dbl < INT64_MIN)						return false;
						if (this->_child[idx]._as_dbl > INT64_MAX)						return false;
						value	= static_cast<int64_t>(this->_child[idx]._as_dbl >= 0.0
														? this->_child[idx]._as_dbl + 0.5
														: this->_child[idx]._as_dbl - 0.5);
						break;
	}

	return true;
};

bool	dal_t::get_val_uint64(uint32_t idx, uint64_t& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & NUMBER_TYPE))		return false;
	
	switch (this->_child[idx]._type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_child[idx]._as_bool;		break;
		case DT_UINT:	value	= this->_child[idx]._as_uint;		break;
		case DT_INT:	if (this->_child[idx]._as_int & 0x8000000000000000)		return false;
						else	value	= this->_child[idx]._as_int;
						break;
		case DT_DOUBLE:	if (this->_child[idx]._as_dbl != this->_child[idx]._as_dbl)		return false;
						if (this->_child[idx]._as_dbl < 0.0)							return false;
						if (this->_child[idx]._as_dbl > INT64_MAX)						return false;
						value	= static_cast<uint64_t>(this->_child[idx]._as_dbl >= 0.0
														? this->_child[idx]._as_dbl + 0.5
														: this->_child[idx]._as_dbl - 0.5);
						break;
	}

	return true;
};

bool	dal_t::get_val_flt(uint32_t idx, float& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & NUMBER_TYPE))		return false;
	
	switch (this->_child[idx]._type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_child[idx]._as_bool;							break;
		case DT_UINT:	value	= static_cast<float>(this->_child[idx]._as_uint);		break;
		case DT_INT:	value	= static_cast<float>(this->_child[idx]._as_int);		break;
		case DT_DOUBLE:	if (this->_child[idx]._as_dbl != this->_child[idx]._as_dbl)	value	= 0x7fc00000;	//NAN
						else														value	= static_cast<float>(this->_as_dbl);
						break;
	}
	
	return true;
};

bool	dal_t::get_val_dbl(uint32_t idx, double& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & NUMBER_TYPE))		return false;
	
	switch (this->_child[idx]._type)
	{
		default:		return false;
		case DT_BOOL:	value	= this->_child[idx]._as_bool;							break;
		case DT_UINT:	value	= static_cast<double>(this->_child[idx]._as_uint);		break;
		case DT_INT:	value	= static_cast<double>(this->_child[idx]._as_int);		break;
		case DT_DOUBLE:	value	= this->_child[idx]._as_dbl;							break;
	}
	
	return true;
};

bool	dal_t::get_val_str(uint32_t idx, const char*& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & DT_STRING))			return false;
	
	value	= this->_child[idx]._as_str;
	
	return true;
};

bool	dal_t::get_val_str(uint32_t idx, char*& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & DT_STRING))			return false;
	
	value	= this->_child[idx]._as_str;
	
	return true;
};

bool	dal_t::get_val_str(uint32_t idx, dalStr_t& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & DT_STRING))			return false;
	
	value.data	= this->_child[idx]._as_str;
	value.size	= this->_child[idx]._size;
	
	return true;
};

bool	dal_t::get_val_blob(uint32_t idx, dalBlob_t& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & DT_BLOB))			return false;
	
	value.data	= this->_child[idx]._as_blob;
	value.size	= this->_child[idx]._size;
	
	return true;
};

bool	dal_t::get_val_blob(uint32_t idx, dalBlobRef_t& value)
{
	if (this->_type != DT_ARRAY)						return false;
	if (this->_size <= idx)								return false;
	if (!(this->_child[idx]._type & DT_BLOB_REF))		return false;
	
	value.data	= this->_child[idx]._as_blob;
	value.size	= this->_child[idx]._size;
	
	return true;
};

//===============================================================================================================================
