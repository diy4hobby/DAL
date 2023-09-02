#include "dal_example_base.h"
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


int main()
{
	//Create structure with callbacks for allocate and free memory
	dalMemHooks_t	memHooks	= { dal_alloc_node, dal_alloc_data,
									dal_free_node, dal_free_data };
	//Initialize the DAL library and pass the previously created structure to it
	dal_init(&memHooks);

	//Create an empty object - then we can add other objects and values to it
	dal_t*		topObj			= dal_create();
	
	//Add key-value pairs to the previously created object
	topObj->add_val_str("string_value", "Hello from DAL");
	topObj->add_val_uint("int_value", 123456789);
	topObj->add_val_dbl("dbl_value", 10.987654321);

	//Adding a child object - making the structure more complex
	dal_t*		chldObj			= topObj->add_child("child_obj");
	int			testArr[5]		= {50, 40, 30, 20, 10};
	chldObj->add_arr_int("int_array", testArr, 5);
	memset(testArr, 0, 5 * sizeof(int));
	uint32_t	arrLen			= 5;
	chldObj->get_arr_int("int_array", testArr, arrLen);

	//Serialize the object in JSON to check the resulting structure
	uint8_t		buf[4096]	= {0};
	uint32_t	len			= topObj->to_mpack(buf, sizeof(buf));

	dal_delete(topObj);

	cout << buf << endl;
	return 0;
}
