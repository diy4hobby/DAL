#include "dal_struct.h"
#include "dal_utils.h"
#include "dal_common/dal_common.h"



//===============================================================================================================================
bool	_dal_compare_recurse(dal_t* node1, dal_t* node2)
{
	if (node2 == nullptr)						return false;

	if (node1->_type != node2->_type)			return false;
	if (node1->_key_len != node2->_key_len)		return false;
	if (node1->_key_len > 0)
	{	if (node1->_key_hash != node2->_key_hash)										return false;
		if (dal_string_compare(node1->_key, node2->_key, node2->_key_len) == false)	return false;
	}

	switch (node1->_type)
	{
		case DT_UNKN:
		case DT_BOOL:		return (node1->_as_bool == node2->_as_bool);
		case DT_UINT:		return (node1->_as_uint == node2->_as_uint);
		case DT_INT:		return (node1->_as_int == node2->_as_int);
		case DT_DOUBLE:		return (node1->_as_dbl == node2->_as_dbl);
		case DT_STRING:
			if (node1->_size != node2->_size)	return false;
			return dal_string_compare(node1->_as_str, node2->_as_str, node2->_size);
		case DT_BLOB:
			return dal_bytedata_compare(node1->_as_blob, node2->_as_blob, node1->_size, node2->_size);
		case DT_BLOB_REF:
			if (node1->_size != node2->_size)	return false;
			return (node1->_as_blob == node2->_as_blob);
		case DT_ARRAY:
			if (node1->_size != node2->_size)	return false;
			else
			{	dal_t*		child1	= node1->_child;
				dal_t*		child2	= node2->_child;
				uint32_t	count	= node1->_size;
				while (--count > 0)
				{
					bool	result	= _dal_compare_recurse(child1, child2);
					if (result == false)			return false;
					child1++;
					child2++;
				}
			}
			break;
		case DT_OBJECT:
			if (node1->_child != nullptr)
			{
				dal_t*	child1		= node1->_child;
				dal_t*	child2		= node2->get_child(child1->_key);
				while ((child1 != nullptr) && (child2 != nullptr))
				{
					bool	result	= _dal_compare_recurse(child1, child2);
					if (result == false)				return false;
					child1			= child1->_next;
					if (child1 != nullptr)	child2	= node2->get_child(child1->_key);
					else					child2	= nullptr;
				}
				if (child1 != child2)					return false;
			}
			break;
	}

	return true;
}

bool	dal_t::compare(dal_t* node)
{
	return _dal_compare_recurse(this, node);
};


//===============================================================================================================================


template<> bool	dal_t::compare(bool value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	bool	myValue;
	if (get<bool>(myValue) == false)			return false;

	return (value && myValue);
};

template<> bool	dal_t::compare(char* value, dalNodeType_e type)
{
	if (!(this->_type & DT_STRING & type))	return false;
	
	return dal_string_compare(this->_as_str, value, dal_string_length(value, DAL_MAX_CSTR_LEN));
};

template<> bool	dal_t::compare(const char* value, dalNodeType_e type)
{
	if (!(this->_type & DT_STRING & type))	return false;
	
	return dal_string_compare(this->_as_str, value, dal_string_length(value, DAL_MAX_CSTR_LEN));
};

template<> bool	dal_t::compare(uint64_t value, dalNodeType_e type)
{
	if (!(this->_type & NUMBER_TYPE & type))	return false;
	
	switch (this->_type)
	{
		default:			return false;
		case DT_BOOL:		return (this->_as_bool == value);
		case DT_UINT:		return (this->_as_uint == value);
		case DT_INT:		return (this->_as_int == value);
		case DT_DOUBLE:		return (this->_as_dbl == value);
	}
};


//===============================================================================================================================


template<> bool	dal_t::compare(const char* key, bool value, dalNodeType_e type)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	return child->compare<bool>(value, type);
};

template<> bool	dal_t::compare(const char* key, char* value, dalNodeType_e type)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	return child->compare<char*>(value, type);
};

template<> bool	dal_t::compare(const char* key, const char* value, dalNodeType_e type)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	return child->compare<const char*>(value, type);
};

template<> bool	dal_t::compare(const char* key, uint64_t value, dalNodeType_e type)
{
	dal_t*	child	= this->get_child(key);
	if (child == nullptr)						return false;
	return child->compare<uint64_t>(value, type);
};

//===============================================================================================================================