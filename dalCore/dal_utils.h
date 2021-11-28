/*
 * Data Abstraction Level (DAL)
 *
 *  Created on: 25 03 2020 г.
 *      Author: a.sheviakov
 */

#ifndef DATA_ABSTRACTION_LAYER_UTILS_H_
#define DATA_ABSTRACTION_LAYER_UTILS_H_
#include "stdint.h"
#include "dal_struct.h"


//DAL initialization - memory allocation and release methods must be passed. The methods must be reentrant.
dalResult_e		dal_init(dalMemHooks_t* mem);

//Creating an empty object
dal_t*			dal_create_obj();

//Create an empty array - after that you can add elements to it, which can be heterogeneous (numbers, objects, other arrays, etc.)
dal_t*			dal_create_array(const char* key);

//Create copy - object (or array) with the same structure and data will be created
dal_t*			dal_copy(dal_t* obj);

//Methods of working with keys
void			dal_change_key(dal_t* obj, const char* newKey);
uint8_t			dal_has_key(dal_t* obj);
uint8_t			dal_has_item(dal_t* obj, const char* key);

//Add item to object - the item is added to the list of child elements of the passed object, i.e. one level down in the hierarchy
dalResult_e		dal_add_item(dal_t* obj, dal_t* item);

//Removing the passed item from the childrens of the object which is the parent in relation to the item. The removed item is not destroyed
dalResult_e		dal_eject_item(dal_t* item);

//Removing an item by key - as dal_eject_item
dal_t*			dal_remove_item(dal_t* obj, char* key);

//Removing an object, it will be removed from its parent and all memory allocated for that object (including children) will be freed
dalResult_e		dal_delete(dal_t* item);

//Methods of creating different objects/arrays/values
dal_t*			dal_add_obj(dal_t* obj, const char* key);
dal_t*			dal_add_arr(dal_t* dst, const char* key);
dal_t*			dal_add_bool(dal_t* dst, const char* key, uint8_t value);
dal_t*			dal_add_uint(dal_t* dst, const char* key, uint64_t value);
dal_t*			dal_add_int(dal_t* dst, const char* key, int64_t value);
dal_t*			dal_add_double(dal_t* dst, const char* key, double value);
dal_t*			dal_add_cstr(dal_t* dst, const char* key, const char* value);
dal_t*			dal_add_str(dal_t* dst, const char* key, char* value, uint32_t len);
dal_t*			dal_add_blob(dal_t* dst, const char* key, void** value, uint32_t len);
dal_t*			dal_add_blobref(dal_t* dst, const char* key, void* value, uint32_t len);

//Methods of getting various objects/arrays/values - they are not taken from the passed object, the pointer to the sought element is returned
dal_t*			dal_get_obj(dal_t* obj, const char* key);
dal_t*			dal_get_arr(dal_t* obj, const char* key);
uint8_t			dal_get_bool(dal_t* obj, const char* key, uint8_t def);
uint64_t		dal_get_uint(dal_t* obj, const char* key, uint64_t def);
int64_t			dal_get_int(dal_t* obj, const char* key, int64_t def);
double			dal_get_double(dal_t* obj, const char* key, double def);
char*			dal_get_str(dal_t* obj, const char* key);
void*			dal_get_blob(dal_t* obj, const char* key);
uint32_t		dal_get_blob_size(dal_t* obj, const char* key);
uint32_t		dal_get_size(dal_t* obj);

//Methods of checking types of passed objects
uint8_t			dal_is_obj(dal_t* obj);
uint8_t			dal_is_array(dal_t* obj);
uint8_t			dal_is_empty(dal_t* obj);
uint8_t			dal_is_number(dal_t* obj);
uint8_t			dal_is_str(dal_t* obj);

//Iterate over child items of an array or object
#define			dal_for_each(item, arr) for(item = (arr != NULL) ? (arr)->child : NULL; item != NULL; item = item->next)

//Data serialization method - goes through the whole data tree and calls methods passed in the structure for serialization of data elements
dalResult_e		dal_serialize(void* ctx, writerHooks_t* wrHooks, dal_t* obj);






#endif /* DATA_ABSTRACTION_LAYER_UTILS_H_ */
