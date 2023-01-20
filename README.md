# DAL
**Data Abstraction Layer (DAL)** - library of functions that allows developers of embedded systems to use data representation in the form of objects. The main design goal is to abstract data structure and simplify data handling, fast and easy serialization and deserialization for network transfer and saving/loading.

## Nuances of working with dynamic memory
Since working with memory is very important for embedded system developers, I would like to specify right away: none of the library methods allocate or free dynamic memory without using user-provided methods. Methods of allocating and releasing dynamic memory are provided by the user during library initialization.
### Methods of allocating and releasing dynamic memory
**Allocate memory for a node** (element, containing data) made separately from memory allocation for data. This allows to speed up the process of memory allocation, because the size of the node is always known for in advance. This approach allows use the dynamic memory in form of "blocks"-pool for creating a node, the support of which is in some systems. If the methods of allocation of the "block" memory is ont provided by architecture, then to allocate memory for the node, use "regular" methods for allocating memory with a fixed size - size of a node.

**Allocating memory for data** is used when creating nodes, containing strings or binary data. "Regular" memory allocation/free methods should be used (malloc/free).

Memory alignment does not matter for internal library methods.


## Usage example
```cpp
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
	topObj->add_value("string_value", "Hello from DAL");
	topObj->add_value("int_value", 123456789);
	topObj->add_value("dbl_value", 10.987654321);

	//Adding a child object - making the structure more complex
	dal_t*		chldObj			= topObj->create_child();
	int		u8Arr[5]		= {50, 40, 30, 20, 10};
	chldObj->add_array("uint8_array", u8Arr, 5);

	//Serialize the object in JSON to check the resulting structure
	uint8_t		buf[4096]	= {0};
	uint32_t	len			= topObj->to_json(buf, sizeof(buf));

	dal_delete(topObj);

	cout << buf << endl;
	return 0;
}
```

## Serialization and deserialization of objects
Serialization is supported to:
- [Message Pack](https://msgpack.org/);
- [JSON](https://www.json.org/json-en.html).

Deseritization is supported from:
- [Message Pack](https://msgpack.org/);
- [JSON](https://www.json.org/json-en.html);
- WebAPI.

## Versions
***
1.0.0
- First version, commit for the community.
- Writed short description and base example.
***
1.1.0
- Added a method for creating a copy of a node (obj->duplicate()). The method returns a copy of the node (taking into account the nesting of the structure).
***
1.2.0
- Added a method for creating a child node-object with its name.
***
1.2.1
- Fixed array deserialization method.
***
1.2.2
- Fixed the bug described in "add_node & detach bugs #1".
***
2.0.0
- Node attach method renamed from add_node to attach to better express the nature of the operation - node ownership (the attached node becomes a child and therefore should not be deleted by itself);
- The methods of adding nodes and getting their values have been significantly changed. ***The new version is not compatible with the previous API.***
***


