#include "dal_attach_node.h"
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
	
	topObj->add_val_str("string_value", "Hello from DAL");
	topObj->add_val_uint("int_value", 123456789);
	topObj->add_val_dbl("dbl_value", 10.987654321);

	dal_t*	chldObj				= topObj->create_child("child1");
	int		uArr[5]				= {50, 40, 30, 20, 10};
	chldObj->add_arr_int("uint8_array", uArr, 5);
	chldObj->add_val_str("name", "param1");
	chldObj->create_child("child2")->add_val_str("name2", "param2");
	
	cout << "topObj size before attach node: " << topObj->size() << endl;


	dal_t*		attachNode		= dal_create();
	attachNode->add_val_str("string_value", "newObj string value");
	attachNode->add_val_uint("int_value", 9999999);
	attachNode->add_val_dbl("dbl_value", 99.999999);
	attachNode->add_val_str("name", "param1");
	attachNode->create_child("child_node")->add_val_str("level2", "value2");

	topObj->attach(attachNode);

	cout << "topObj size with attached node: " << topObj->size() << endl;

	attachNode->detach();

	cout << "topObj size after detach node: " << topObj->size() << endl;

	dal_delete(attachNode);
	dal_delete(topObj);

	return 0;
}
