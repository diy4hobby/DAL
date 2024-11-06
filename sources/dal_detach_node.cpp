#include "dal_struct.h"



//===============================================================================================================================
void	dal_t::detach()
{
	if (this->_parent != nullptr)
	{
		if (this->_parent->_type & TUPLE_TYPE)	this->_parent->_size--;
		//If this node is the last one of the parent, then we need to replace it with the previous one
		if (this->_parent->_last == this)
		{	this->_parent->_last	= this->_prev;
		}
		//If the detach node has a parent, then we need to check if is this node the first in the list of children, if yes
		//before we need to remove it from the list of children and assign the next one as the first node
		if (this->_parent->_child == this)
		{	this->_parent->_child	= this->_next;
			this->_parent			= nullptr;
		}
		this->_parent	= nullptr;
	}
	//Now we bind the nodes to the left and right
	if (this->_prev != nullptr)		this->_prev->_next	= this->_next;
	if (this->_next != nullptr)		this->_next->_prev	= this->_prev;
	this->_prev			= nullptr;
	this->_next			= nullptr;
};
//===============================================================================================================================
