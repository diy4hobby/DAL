#include "dal_struct.h"
#include "dal_utils.h"



//===============================================================================================================================
bool	dal_t::value(bool def)
{
	if (!(this->_type & NUMBER_TYPE))	return def;
	switch (this->_type)
	{
		default:			return def;
		case DT_BOOL:		return this->_as_bool;
		case DT_UINT:		return (this->_as_uint != 0);
		case DT_INT:		return (this->_as_int > 0);
		case DT_DOUBLE:		if (this->_as_dbl != this->_as_dbl)	return def;
							else								return (this->_as_dbl > 0);
	}
	return def;
};

int	dal_t::value(int def)
{
	if (!(this->_type & NUMBER_TYPE))	return def;
	int64_t		tempVal;

	switch (this->_type)
	{
		default:			tempVal	= def;				break;
		case DT_BOOL:		tempVal	= this->_as_bool;	break;
		case DT_UINT:		tempVal	= this->_as_uint;	break;
		case DT_INT:		tempVal	= this->_as_int;	break;
		case DT_DOUBLE:		if (this->_as_dbl != this->_as_dbl)	tempVal	= def;
							else								tempVal	= static_cast<int64_t>(this->_as_dbl);
	}

	switch (sizeof(int))
	{
		default:	return 0;
		case 2:		if (tempVal > 32767)				return 32767;
					else	if (tempVal < -32768)		return -32768;
					else								return static_cast<int>(tempVal);
		case 4:		if (tempVal > 2147483647)			return 2147483647;
					else	if (tempVal < -2147483648)	return -2147483648;
					else								return static_cast<int>(tempVal);
		case 8:		return static_cast<int>(tempVal);
	}
};

uint64_t	dal_t::value(uint64_t def)
{
	if (!(this->_type & NUMBER_TYPE))	return def;
	switch (this->_type)
	{
		default:			return def;
		case DT_BOOL:		return this->_as_bool;
		case DT_UINT:		return this->_as_uint;
		case DT_INT:		if (this->_as_int < 0)				return 0;
							else								return this->_as_int;
		case DT_DOUBLE:		if (this->_as_dbl != this->_as_dbl)	return def;
							else	if (this->_as_dbl < 0)		return 0;
									else						return static_cast<uint64_t>(this->_as_dbl);
	}
	return def;
};

int64_t	dal_t::value(int64_t def)
{
	if (!(this->_type & NUMBER_TYPE))	return def;
	switch (this->_type)
	{
		default:			return def;
		case DT_BOOL:		return this->_as_bool;
		case DT_UINT:		return this->_as_uint;
		case DT_INT:		return this->_as_int;
		case DT_DOUBLE:		if (this->_as_dbl != this->_as_dbl)	return def;
							else								return static_cast<int64_t>(this->_as_dbl);
	}
	return def;
};

double	dal_t::value(double def)
{
	if (!(this->_type & NUMBER_TYPE))	return def;
	switch (this->_type)
	{
		default:			return def;
		case DT_BOOL:		return this->_as_bool;
		case DT_UINT:		return static_cast<double>(this->_as_uint);
		case DT_INT:		return static_cast<double>(this->_as_int);
		case DT_DOUBLE:		return this->_as_dbl;
	}
	return def;
};

float	dal_t::value(float def)
{
	if (!(this->_type & NUMBER_TYPE))	return def;

	switch (this->_type)
	{
		default:			return def;
		case DT_BOOL:		return this->_as_bool;
		case DT_UINT:		return static_cast<float>(this->_as_uint);
		case DT_INT:		return static_cast<float>(this->_as_int);
		case DT_DOUBLE:		if (this->_as_dbl != this->_as_dbl)	return 0x7fc00000;	//NAN
							else								return static_cast<float>(this->_as_dbl);
	}
};

char*	dal_t::value(char* def)
{
	if (this->_type != DT_STRING)		return def;

	return this->_as_str;
};

dalStr_t	dal_t::value(dalStr_t* def)
{
	if (this->_type != DT_STRING)
	{
		if (def != nullptr)				return *def;
		else							return dalStr_t{nullptr, 0};
	}

	dalStr_t		tempStr;
	tempStr.data	= this->_as_str;
	tempStr.size	= this->_size;
	return tempStr;
};

dalBlob_t	dal_t::value(dalBlob_t* def)
{
	if (!(this->_type & BLOB_TYPE))
	{
		if (def != nullptr)				return *def;
		else							return dalBlob_t{nullptr, 0};
	}

	dalBlob_t		tempBlob;
	tempBlob.data	= this->_as_blob;
	tempBlob.size	= this->_size;
	return tempBlob;
};
//===============================================================================================================================


//===============================================================================================================================
bool	dal_t::value(const char* key, bool def)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return def;

	return child->value(def);
};

int	dal_t::value(const char* key, int def)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return def;

	return child->value(def);
};

uint64_t	dal_t::value(const char* key, uint64_t def)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return def;

	return child->value(def);
};

int64_t	dal_t::value(const char* key, int64_t def)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return def;

	return child->value(def);
};

double	dal_t::value(const char* key, double def)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return def;

	return child->value(def);
};

float	dal_t::value(const char* key, float def)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return def;

	return child->value(def);
};

char*	dal_t::value(const char* key, char* def)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)				return def;

	return child->value(def);
};

dalStr_t	dal_t::value(const char* key, dalStr_t* def)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)
	{
		if (def != nullptr)				return *def;
		else							return dalStr_t{nullptr, 0};
	}

	return child->value(def);
};

dalBlob_t	dal_t::value(const char* key, dalBlob_t* def)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)
	{
		if (def != nullptr)				return *def;
		else							return dalBlob_t{nullptr, 0};
	}

	return child->value(def);
};

bool	dal_t::value(uint32_t idx, bool def)
{
	if (this->_type != DT_ARRAY)	return def;
	if (this->_size <= idx)			return def;
	return this->_child[idx].value(def);
};

int	dal_t::value(uint32_t idx, int def)
{
	if (this->_type != DT_ARRAY)	return def;
	if (this->_size <= idx)			return def;
	return this->_child[idx].value(def);
};

uint64_t	dal_t::value(uint32_t idx, uint64_t def)
{
	if (this->_type != DT_ARRAY)	return def;
	if (this->_size <= idx)			return def;
	return this->_child[idx].value(def);
};

int64_t	dal_t::value(uint32_t idx, int64_t def)
{
	if (this->_type != DT_ARRAY)	return def;
	if (this->_size <= idx)			return def;
	return this->_child[idx].value(def);
};

double	dal_t::value(uint32_t idx, double def)
{
	if (this->_type != DT_ARRAY)	return def;
	if (this->_size <= idx)			return def;
	return this->_child[idx].value(def);
};

float	dal_t::value(uint32_t idx, float def)
{
	if (this->_type != DT_ARRAY)	return def;
	if (this->_size <= idx)			return def;
	return this->_child[idx].value(def);
};

char*	dal_t::value(uint32_t idx, char* def)
{
	if (this->_type != DT_ARRAY)	return def;
	if (this->_size <= idx)			return def;
	return this->_child[idx].value(def);
};

dalStr_t	dal_t::value(uint32_t idx, dalStr_t* def)
{
	if (this->_type != DT_ARRAY)	return *def;
	if (this->_size <= idx)			return *def;
	return this->_child[idx].value(def);
};

dalBlob_t	dal_t::value(uint32_t idx, dalBlob_t* def)
{
	if (this->_type != DT_ARRAY)	return *def;
	if (this->_size <= idx)			return *def;
	return this->_child[idx].value(def);
};
//===============================================================================================================================
