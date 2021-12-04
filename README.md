# DAL
**Data Abstraction Layer (DAL)** - library of functions that allows developers of embedded systems to use data representation in the form of objects. The main design goal is to abstract data structure and simplify data handling, fast and easy serialization and deserialization for network transfer and saving/loading.
The libraries written by Nicholas Fraser (MessagePack) and Vincent Hanquez (JSON) are used for serialization and deserialization.

## Nuances of working with dynamic memory
Since working with memory is very important for embedded system developers, I would like to specify right away: none of the library methods allocate or free dynamic memory without using user-provided methods. Methods of allocating and releasing dynamic memory are provided by the user during library initialization.
### Methods of allocating and releasing dynamic memory
**Allocate memory for a node** (element, containing data) made separately from memory allocation for data. This allows to speed up the process of memory allocation, because the size of the node is always known for in advance. This approach allows use the dynamic memory in form of "blocks"-pool for creating a node, the support of which is in some systems. If the methods of allocation of the "block" memory is ont provided by architecture, then to allocate memory for the node, use "regular" methods for allocating memory with a fixed size - size of a node.

**Allocating memory for data** is used when creating nodes, containing strings or binary data. "Regular" memory allocation/free methods should be used (malloc/free).

Memory alignment does not matter for internal library methods.


## Library methods
The methods are divided into the following groups:
- library initialization (dal_init);
- create/delete, copy, add/remove object (dal_create_obj, dal_create_array, dal_copy, dal_add_item, dal_eject_item, dal_remove_item, dal_delete);
- key change, check availability of object with specified key (dal_change_key, dal_has_key, dal_has_item);
- add/extract data (dal_add_obj, dal_add_arr, dal_add_bool, dal_add_uint, dal_add_int, dal_add_double, dal_add_cstr, dal_add_str, dal_add_blob, dal_add_blobref, dal_get_obj, dal_get_arr, dal_get_bool, dal_get_uint, dal_get_int, dal_get_double, dal_get_str, dal_get_blob, dal_get_blob_size, dal_get_size);
- verification of availability and type of object (dal_is_obj, dal_is_array, dal_is_empty, dal_is_number, dal_is_str);
- iterating over the child elements of an object (dal_for_each);
- object serialization (dal_serialize - internal method).

## Serialization and deserialization of objects
Serialization is supported to:
- [Message Pack](https://msgpack.org/);
- [JSON](https://www.json.org/json-en.html).

Deseritization is supported from:
- [Message Pack](https://msgpack.org/);
- [JSON](https://www.json.org/json-en.html);
- WebAPI.

## Versions
- 1.0.0 - first version, commit for the community. Writed short description and base example.

