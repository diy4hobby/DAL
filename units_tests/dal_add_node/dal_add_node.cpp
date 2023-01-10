#include "dal_add_node.h"
#include "dal.h"

using namespace std;


dal_t* dal_alloc_node()
{
	return static_cast<dal_t*>(malloc(sizeof(dal_t)));
}

void* dal_alloc_data(uint32_t size)
{
	return malloc(size);
};

void	dal_free_node(dal_t* data)
{
	free(data);
};

void	dal_free_data(void* data)
{
	free(data);
};

using namespace std;


int main()
{
	dalMemHooks_t	memHooks	= { dal_alloc_node, dal_alloc_data, dal_free_node, dal_free_data };
	dal_init(&memHooks);

	dal_t*	topObj				= dal_create();
	
	topObj->add_value("string_value", "Hello from DAL");
	topObj->add_value("int_value", 123456789);
	topObj->add_value("dbl_value", 10.987654321);

	dal_t*	chldObj				= topObj->create_child("child1");
	int		u8Arr[5]			= {50, 40, 30, 20, 10};
	chldObj->add_array("uint8_array", u8Arr, 5);
	chldObj->add_value("name", "param1");
	chldObj->create_child("child2")->add_value("name2", "param2");
	
	uint8_t		buf[4096]		= { 0 };
	uint32_t	len;
	len		= topObj->to_mpack(buf, sizeof(buf));

	dal_delete(topObj);

	dal_t*		newObj			= dal_create(); //new object to read from mpack
	dal_t*		copyObj			= dal_create(); //test object for memory leaks 
	
	// Making newObj not empty before using it for deserialize
	newObj->add_value("string_value", "newObj string value");
	newObj->add_value("int_value", 9999999);
	newObj->add_value("dbl_value", 99.999999);
	newObj->add_value("name", "param1");
	newObj->create_child("child_node")->add_value("level2", "value2");

	copyObj->add_node(newObj->get_child("child_node"));
	cout << "Try to deserialize from MPACK:" << endl;
	cout << "newObj size before: " << newObj->size() << endl;

	dalResult_e	result = newObj->from_mpack(buf, sizeof(buf));

	dal_delete(copyObj);
	dal_delete(newObj);

	return 0;
}
