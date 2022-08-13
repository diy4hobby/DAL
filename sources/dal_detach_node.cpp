#include "dal_struct.h"



//===============================================================================================================================
void	dal_t::detach()
{
	if (this->_parent != nullptr)
	{
		if (this->_parent->_child == this)
		{	this->_parent->_child	= this->_next;
			this->_parent			= nullptr;
		}
	}
	if (this->_prev != nullptr)		this->_prev->_next	= this->_next;
	if (this->_next != nullptr)		this->_next->_prev	= this->_prev;
	this->_prev			= nullptr;
	this->_next			= nullptr;
};
//===============================================================================================================================
