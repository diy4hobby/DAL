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

//===============================================================================================================================

dal_t::dal_t()
{
	this->_type			= DT_OBJECT;
};

dal_t::dal_t(const dal_t& src)
{
	dal_t*	child	= src._child;
	while (child != nullptr)
	{
		this->_add_item(child->duplicate());
		child	= child->_next;
	}

	_copy_trivial(const_cast<dal_t*>(&src));
};

dal_t::dal_t(const dal_t* src)
{
	if (src == nullptr)		return;
	dal_t*	child	= src->_child;
	while (child != nullptr)
	{
		this->_add_item(child->duplicate());
		child	= child->_next;
	}

	_copy_trivial(const_cast<dal_t*>(src));
};

dal_t::dal_t(int format, char* data, uint32_t size)
{
	this->_type				= DT_OBJECT;
	dalResult_e		result	= DAL_FORMAT_ERR;
	uint8_t*		u8		= reinterpret_cast<uint8_t*>(data);
	if (format & SERDES_JSON)	result	= from_json(u8, size, DAL_JSON_MAX_TOKENS);
	if (result == DAL_OK)	return;
	if (format & SERDES_MPACK)	result	= from_mpack(u8, size);
	if (result == DAL_OK)	return;
	if (format & SERDES_WEBAPI)	result	= from_webapi(u8, size);
	if (result == DAL_OK)	return;

	this->_type		= DT_UNKN;
};

dal_t::dal_t(int format, uint8_t* data, uint32_t size) : dal_t(format, reinterpret_cast<char*>(data), size)
{
};

uint32_t dal_t::serialize(dalSerDesFormat_e format, uint8_t* buf, uint32_t size)
{
	switch (format)
	{
		case SERDES_JSON:	return this->to_json(buf, size);
		case SERDES_MPACK:	return this->to_mpack(buf, size);
		case SERDES_WEBAPI:	return 0;
		default:			return 0;
	}
};

uint32_t dal_t::serialize(dalSerDesFormat_e format, char* buf, uint32_t size)
{
	return serialize(format, reinterpret_cast<uint8_t*>(buf), size);
};

//===============================================================================================================================

dal_t::~dal_t()
{
	//Delete our child elements
	dal_t*	child		= this->_child;
	while (child != NULL)
	{	dal_t*	next	= child->_next;
		dal_delete(child);
		child			= next;
	}
	//Delete allocated memory for non-trivial types
	if (this->_type & ALLOCATED_MEM_TYPE)
	{
		_dal_memHooks.free_data(this->_mem_ptr);
	}
};

//===============================================================================================================================

dal_t& dal_t::operator=(const dal_t& src)
{
	if (this == &src)	return *this;
	
	//Delete our child elements
	dal_t*	child		= this->_child;
	while (child != NULL)
	{	dal_t*	next	= child->_next;
		dal_delete(child);
		child			= next;
	}
	//Delete allocated memory for non-trivial types
	if (this->_type & ALLOCATED_MEM_TYPE)	_dal_memHooks.free_data(this->_mem_ptr);

	//Duplicate the child elements of the source
	child				= src._child;
	while (child != nullptr)
	{
		this->_add_item(child->duplicate());
		child	= child->_next;
	}

	this->_size		= src._size;
	_copy_trivial(const_cast<dal_t*>(&src));
	return *this;
};

//===============================================================================================================================

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
