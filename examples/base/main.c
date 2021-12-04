#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dal.h"



void* dal_alloc_node()
{
	return malloc(sizeof(dal_t));
}

void* dal_alloc_data(uint32_t size)
{
	return malloc(size);
};

void	dal_free_node(void* data)
{
	free(data);
};

void	dal_free_data(void* data)
{
	free(data);
};



void main()
{
	//Create structure with callbacks for allocate and free memory
	dalMemHooks_t	memHooks	= { dal_alloc_node, dal_alloc_data,
									dal_free_node, dal_free_data };
	//Initialize the DAL library and pass the previously created structure to it
	dal_init(&memHooks);

	//Create an empty object - then we can add other objects and values to it
	dal_t*		obj			= dal_create_obj();
	dal_add_cstr(obj, "string", "Hello from DAL");
	dal_add_double(obj, "double", 0.123456789);
	dal_t*		child		= dal_add_obj(obj, "child_object");
	dal_add_bool(child, "is_child", true);


	char		buf[4096]	= {0};
	uint32_t	len			= 0;
	dal2json_serialize(obj, buf, sizeof(buf), &len, false);

	printf(buf);
}