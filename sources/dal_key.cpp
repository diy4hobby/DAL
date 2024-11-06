#include "dal_struct.h"
#include "dal_common/dal_common.h"


//===============================================================================================================================
dalStr_t	dal_t::key()
{
	dalStr_t	result;
	result.data					= this->_key;
	result.size					= this->_key_len;
	return result;
};

void	dal_t::_rename(const char* key, uint32_t len, uint32_t hash)
{
	dal_string_copy(this->_key, key, len);
	this->_key_len				= len;
	this->_key[this->_key_len]	= 0x00;
	this->_key_hash				= hash;
};

void	dal_t::rename(const char* key, uint32_t len)
{
	if (key == nullptr)			return;
	if (len == 0)				return;
	if (len >= DAL_KEY_SIZE)	this->_key_len	= (DAL_KEY_SIZE - 1);
	else						this->_key_len	= len;
	dal_string_copy(this->_key, key, this->_key_len);
	this->_key[this->_key_len]	= 0x00;
	this->_key_hash				= dal_hash_sdbm(this->_key, this->_key_len);
};

void	dal_t::rename(dalStr_t* key)
{
	if (key == nullptr)			return;
	if (key->size >= DAL_KEY_SIZE)		this->_key_len	= (DAL_KEY_SIZE - 1);
	else								this->_key_len	= key->size;
	dal_string_copy(this->_key, key->data, this->_key_len);
	this->_key[this->_key_len]	= 0x00;
	this->_key_hash				= dal_hash_sdbm(this->_key, this->_key_len);
};

void	dal_t::rename(const char* newName)
{
	dalStr_t	key;
	key.data					= newName;
	key.size					= dal_string_length(newName, DAL_KEY_SIZE);
	this->rename(&key);
};
//===============================================================================================================================

