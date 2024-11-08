# DAL
**Data Abstraction Layer (DAL)** - library of functions that allows developers of embedded systems to use data representation in the form of objects. The main design goal is to abstract data structure and simplify data handling, fast and easy serialization (https://msgpack.org/, JSON) and deserialization (https://msgpack.org/, JSON, WebAPI) for network transfer and saving/loading.

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
	topObj->add_val_str("string_value", "Hello from DAL");
	topObj->add_val_uint("int_value", 123456789);
	topObj->add_val_dbl("dbl_value", 10.987654321);

	//Adding a child object - making the structure more complex
	dal_t*		chldObj			= topObj->create_child("child_obj");
	int			testArr[5]		= {50, 40, 30, 20, 10};
	chldObj->add_arr_int("int_array", testArr, 5);
	memset(testArr, 0, 5 * sizeof(int));
	uint32_t	arrLen			= 5;
	chldObj->get_arr_int("int_array", testArr, arrLen);

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
2.0.1
- Some corrections.
***
2.0.2
- Fixed a bug when getting a double value - the node value was erroneously cast to the float type.
- In the "_copy_trivial" method, handling of invalid types has been added, previously this could lead to an error.
***
2.0.3
- Fixed a bug with mixing up the "add_val_ref" methods and "add_val_blob".
***
2.1.0
- Added methods for getting the contents of arrays. Methods are not safe - before calling methods, the developer must make sure that he has allocated enough memory to copy the values ​​of the array. The number of elements in an array can be obtained using the "get_array_size" method.
***
2.1.1
- Minor improvements in getting array values.
***
2.1.2
- Fixed an error parsing a JSON string that occurs if there is still data after the closing bracket of the top object.
***
2.2.0
- Added conversion of floating point numbers to integer values when calling methods to get values. Previously, if the value of a node was a floating-point number, calling the method to get an integer value returned "false", because could not convert the number to the right format.
***
2.2.1
- Fixed a bug in MessagePack deserialization method - the type was defined incorrectly, which caused valid MessagePack to be parsed with a format error.
***
2.2.2
- Fixed a bug in methods for getting values ​​by index.
***
2.2.3
- Fixed a bug when getting a value of "blob" type.
***
2.3.0
- The work with arrays has been significantly changed, now they are not linear memory areas, but lists of nodes, this simplifies working with arrays, although it slightly reduces the speed of their processing.
***
2.3.1
- Bug fixes for serialization/deserialization
***
2.3.2
- Some changes in the API that can be easily fixed in the user software. Significantly compatibility is not broken.
***
2.3.3
- Fixed a bug in getting numeric values ​​related to number representation -2147483648.
***
2.3.4
- Fixed a bug when calling the get object size method related to incorrect processing of arrays after changing their structure.
***
2.3.5
- Fixed a bug when attaching a node.
***
2.3.6
- Minor usability improvements.
***
2.3.7
- Fixes to remove the warning about comparing signed and unsigned numbers.
***