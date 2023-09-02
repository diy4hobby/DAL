#include "dal_struct.h"
#include "dal_utils.h"


//===============================================================================================================================
bool	dal_t::get_arr_bool(const char* key, bool* value, uint32_t& len)
{
	if (value == nullptr)						return false;
	
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	if (child->_type != DT_ARRAY)				return false;
	if (len > child->get_array_size())	len	= child->get_array_size();
	dal_t*	item	= child->_child;

	while (len-- > 0)
	{
		switch (item->_type)
		{
			default:			len	= 0;	return false;
			case DT_BOOL:		*value	= item->_as_bool;			break;
			case DT_UINT:		*value	= (item->_as_uint != 0);	break;
			case DT_INT:		*value	= (item->_as_int > 0);		break;
			case DT_DOUBLE:		*value	= (item->_as_dbl > 0);		break;
		}

		item	= item->_next;
		value++;
	}

	return true;
};

bool	dal_t::get_arr_int(const char* key, int* value, uint32_t& len)
{
	if (value == nullptr)						return false;

	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	if (child->_type != DT_ARRAY)				return false;
	if (len > child->get_array_size())	len	= child->get_array_size();
	dal_t*	item	= child->_child;
	
	int64_t		tempVal;

	while (len-- > 0)
	{
		switch (item->_type)
		{
			default:
				len	= 0;
				return false;
			case DT_BOOL:
				tempVal	= item->_as_bool;
				break;
			case DT_UINT:
				if (item->_as_uint & 0x8000000000000000)		return false;
				else	tempVal	= item->_as_uint;
				break;
			case DT_INT:
				tempVal	= item->_as_int;
				break;
			case DT_DOUBLE:
				if (item->_as_dbl > 0x7FFFFFFFFFFFFFFF)			return false;
				else	if (item->_as_dbl < 0x8000000000000000)	return false;
						else	tempVal	= static_cast<int64_t>(item->_as_dbl);
				break;
		}

		switch (sizeof(int))
		{
			default:
				len	= 0;
				return false;
			case 2:
				if (tempVal > 32767)				return false;
				else	if (tempVal < -32768)		return false;
						else						*value	= static_cast<int>(tempVal);
				break;
			case 4:
				if (tempVal > 2147483647)			return false;
				else	if (tempVal < -2147483648)	return false;
						else						*value	= static_cast<int>(tempVal);
				break;
			case 8:
				*value	= static_cast<int>(tempVal);
				break;
		}

		item	= item->_next;
		value++;
	}

	return true;
};

bool	dal_t::get_arr_uint(const char* key, unsigned int* value, uint32_t& len)
{
	if (value == nullptr)						return false;

	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	if (child->_type != DT_ARRAY)				return false;
	if (len > child->get_array_size())	len	= child->get_array_size();
	dal_t*	item	= child->_child;
	
	uint64_t	tempVal;

	while (len-- > 0)
	{
		switch (item->_type)
		{
			default:
				len	= 0;
				return false;
			case DT_BOOL:
				tempVal	= item->_as_bool;
				break;
			case DT_UINT:
				tempVal	= item->_as_uint;
				break;
			case DT_INT:
				if (item->_as_int & 0x8000000000000000)			return false;
				else	tempVal	= item->_as_int;
				break;
			case DT_DOUBLE:
				if (item->_as_dbl > 0xFFFFFFFFFFFFFFFF)			return false;
				else	if (item->_as_dbl < 0.0)				return false;
						else	tempVal	= static_cast<uint64_t>(item->_as_dbl);
				break;
		}

		switch (sizeof(unsigned int))
		{
			default:
				len	= 0;
				return false;
			case 2:
				if (tempVal > 65535)		return false;
				else						*value	= static_cast<unsigned int>(tempVal);
				break;
			case 4:
				if (tempVal > 0xFFFFFFFF)	return false;
				else						*value	= static_cast<unsigned int>(tempVal);
				break;
			case 8:
				*value	= static_cast<unsigned int>(tempVal);
				break;
		}

		item	= item->_next;
		value++;
	}

	return true;
};

bool	dal_t::get_arr_int32(const char* key, int32_t* value, uint32_t& len)
{
	if (value == nullptr)						return false;

	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	if (child->_type != DT_ARRAY)				return false;
	if (len > child->get_array_size())	len	= child->get_array_size();
	dal_t*	item	= child->_child;
	
	int64_t		tempVal;

	while (len-- > 0)
	{
		switch (item->_type)
		{
			default:
				len	= 0;
				return false;
			case DT_BOOL:
				tempVal	= item->_as_bool;
				break;
			case DT_UINT:
				if (item->_as_uint & 0x8000000000000000)		return false;
				else	tempVal	= item->_as_uint;
				break;
			case DT_INT:
				tempVal	= item->_as_int;
				break;
			case DT_DOUBLE:
				if (item->_as_dbl > 2147483647)					return false;
				else	if (item->_as_dbl < -2147483648)		return false;
						else	tempVal	= static_cast<int64_t>(item->_as_dbl);
				break;
		}

		if (tempVal > 2147483647)			return false;
		else	if (tempVal < -2147483648)	return false;
				else						*value	= static_cast<int>(tempVal);
	
		item	= item->_next;
		value++;
	}

	return true;
};

bool	dal_t::get_arr_uint32(const char* key, uint32_t* value, uint32_t& len)
{
	if (value == nullptr)						return false;

	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	if (child->_type != DT_ARRAY)				return false;
	if (len > child->get_array_size())	len	= child->get_array_size();
	dal_t*	item	= child->_child;
	
	uint64_t	tempVal;

	while (len-- > 0)
	{
		switch (item->_type)
		{
			default:
				len	= 0;
				return false;
			case DT_BOOL:
				tempVal	= item->_as_bool;
				break;
			case DT_UINT:
				tempVal	= item->_as_uint;
				break;
			case DT_INT:
				if (item->_as_int & 0x8000000000000000)		return false;
				else	tempVal	= item->_as_int;
				break;
			case DT_DOUBLE:
				if (item->_as_dbl > 0xFFFFFFFF)				return false;
				else	if (item->_as_dbl < 0.0)			return false;
						else	tempVal	= static_cast<uint64_t>(item->_as_dbl);
				break;
		}

		if (tempVal > 0xFFFFFFFF)	return false;
		else						*value	= static_cast<unsigned int>(tempVal);

		item	= item->_next;
		value++;
	}

	return true;
};

bool	dal_t::get_arr_int64(const char* key, int64_t* value, uint32_t& len)
{
	if (value == nullptr)						return false;

	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	if (child->_type != DT_ARRAY)				return false;
	if (len > child->get_array_size())	len	= child->get_array_size();
	dal_t*	item	= child->_child;
	
	while (len-- > 0)
	{
		switch (item->_type)
		{
			default:
				len	= 0;
				return false;
			case DT_BOOL:
				*value	= item->_as_bool;
				break;
			case DT_UINT:
				if (item->_as_uint & 0x8000000000000000)		return false;
				else	*value	= item->_as_uint;
				break;
			case DT_INT:
				*value	= item->_as_int;
				break;
			case DT_DOUBLE:
				if (item->_as_dbl > 0x7FFFFFFFFFFFFFFF)			return false;
				else	if (item->_as_dbl < 0x8000000000000000)	return false;
						else	*value	= static_cast<int64_t>(item->_as_dbl);
				break;
		}

		item	= item->_next;
		value++;
	}

	return true;
};

bool	dal_t::get_arr_uint64(const char* key, uint64_t* value, uint32_t& len)
{
	if (value == nullptr)						return false;

	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	if (child->_type != DT_ARRAY)				return false;
	if (len > child->get_array_size())	len	= child->get_array_size();
	dal_t*	item	= child->_child;
	
	while (len-- > 0)
	{
		switch (item->_type)
		{
			default:
				len	= 0;
				return false;
			case DT_BOOL:
				*value	= item->_as_bool;
				break;
			case DT_UINT:
				*value	= item->_as_uint;
				break;
			case DT_INT:
				if (item->_as_int & 0x8000000000000000)		return false;
				else	*value	= item->_as_int;
				break;
			case DT_DOUBLE:
				if (item->_as_dbl > 0xFFFFFFFFFFFFFFFF)		return false;
				else	if (item->_as_dbl < 0.0)			return false;
						else	*value	= static_cast<uint64_t>(item->_as_dbl);
				break;
		}

		item	= item->_next;
		value++;
	}

	return true;
};

bool	dal_t::get_arr_flt(const char* key, float* value, uint32_t& len)
{
	if (value == nullptr)						return false;

	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	if (child->_type != DT_ARRAY)				return false;
	if (len > child->get_array_size())	len	= child->get_array_size();
	dal_t*	item	= child->_child;
	
	while (len-- > 0)
	{
		switch (item->_type)
		{
			default:
				len	= 0;
				return false;
			case DT_BOOL:
				*value	= item->_as_bool;
				break;
			case DT_UINT:
				*value	= static_cast<float>(item->_as_uint);
				break;
			case DT_INT:
				*value	= static_cast<float>(item->_as_int);
				break;
			case DT_DOUBLE:
				if (child->_as_dbl != child->_as_dbl)	*value	= 0x7fc00000;	//NAN
				else									*value	= static_cast<float>(child->_as_dbl);
				break;
		}

		item	= item->_next;
		value++;
	}

	return true;
};

bool	dal_t::get_arr_dbl(const char* key, double* value, uint32_t& len)
{
	if (value == nullptr)						return false;

	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	if (child->_type != DT_ARRAY)				return false;
	if (len > child->get_array_size())	len	= child->get_array_size();
	dal_t*	item	= child->_child;
	
	while (len-- > 0)
	{
		switch (item->_type)
		{
			default:
				len	= 0;
				return false;
			case DT_BOOL:
				*value	= item->_as_bool;
				break;
			case DT_UINT:
				*value	= static_cast<double>(item->_as_uint);
				break;
			case DT_INT:
				*value	= static_cast<double>(item->_as_int);
				break;
			case DT_DOUBLE:
				*value	= item->_as_dbl;
				break;
		}

		item	= item->_next;
		value++;
	}

	return true;
};
//===============================================================================================================================