/*
 * Data Abstraction Layer (DAL)
 *
 *  Created on: 08 06 2022 г.
 *      Author: a.sheviakov
 */

#ifndef DATA_ABSTRACTION_LAYER_STRUCT_H_
#define DATA_ABSTRACTION_LAYER_STRUCT_H_

#include "dal_common/dal_base_types.h"
#include "dal_result_codes.h"
#include "dal_string_struct.h"
#include "dal_blob_struct.h"
#include "dal_ser_deser/dal_serializer.h"


//===============================================================================================================================
//Data types that a node can contain
typedef enum
{
	DT_UNKN				= 0x0000,
	DT_BOOL				= 0x0001,
	DT_UINT				= 0x0002,
	DT_INT				= 0x0004,
	DT_DOUBLE			= 0x0010,
	DT_STRING			= 0x0040,
	DT_BLOB				= 0x0080,
	DT_BLOB_REF			= 0x0100,	//Pointer to a static memory area
	DT_ARRAY			= 0x1000,
	DT_OBJECT			= 0x2000,
}dalNodeType_e;

//Combining enums to obtain constants that allow writing and reading data from friend types
#define	TUPLE_TYPE				(DT_ARRAY | DT_OBJECT)
#define	NUMBER_TYPE				(DT_BOOL | DT_UINT | DT_INT | DT_DOUBLE)
#define	BLOB_TYPE				(DT_BLOB | DT_BLOB_REF)
#define	ALLOCATED_MEM_TYPE		(DT_STRING | DT_BLOB)
//===============================================================================================================================


//===============================================================================================================================
#define	DAL_KEY_SIZE			64
#define	DAL_MAX_CSTR_LEN		1 * 1024 * 1024	//Maximum length of c-style string (terminated by 0x00) - used in strnlen for safety
//===============================================================================================================================
//A structure that stores polymorphic data, pointers to the next data in the hierarchy and/or adjacent data (equal in hierarchy)
//Decorated as a structure, not a class, so that a new instance can be created from allocated memory blocks
//(faster and more reliable), with the class instantiation is simple memory allocation would be wrong - problems with the virtual table will begin
//functions, etc. Subsequently, it is possible to use placement new
typedef struct dal_t
{
private:
	struct dal_t*				_parent;
	struct dal_t*				_prev;
	struct dal_t*				_next;
	struct dal_t*				_child;
	struct dal_t*				_last;
	char						_key[DAL_KEY_SIZE];
	uint32_t					_key_len;
	uint32_t					_key_hash;
	dalNodeType_e				_type;
	uint32_t					_size;
	union{
		bool					_as_bool;
		uint64_t				_as_uint;
		int64_t					_as_int;
		char*					_as_str;
		uint8_t*				_as_blob;
		double					_as_dbl;
	};
	void*						_mem_ptr;	//Pointer to allocated memory (may not be aligned)
											//used for as_str and as_blob to have aligned
											//8 bytes pointers
	dalResult_e					_copy_trivial(dal_t* src);//Copy key and value of source node
	dalResult_e					_add_item(dal_t* node);	//Adding an element to childrens at the end of a list

	friend			dal_t*		dal_create(dalNodeType_e type);
	friend			void		dal_delete(dal_t* node);
	friend			uint32_t	_dal_size_recurse(dal_t* node);
	friend			dalResult_e	_dal_serialize_recurse(dalSerializer_t* ser, dal_t* node);
	friend			bool		_dal_compare_recurse(dal_t* node1, dal_t* node2);
	friend			dal_t*		_dal_duplicate_recurse(dal_t* src, dal_t* parent);
public:
	dalNodeType_e	type()		{return this->_type;}
	dal_t*			parent()	{return this->_parent;}
	dal_t*			next()		{return this->_next;}
	dalStr_t		key();
	void			rename(dalStr_t* key);
	void			rename(const char* newName);
	uint32_t		size();
	bool			compare(dal_t* node);			//Compare with other dal structure
	dal_t*			duplicate();					//Create copy of this node
	//Working with child elements of a node
	dal_t*			add_child(dalNodeType_e type = DT_OBJECT);	//Create new child
	dal_t*			add_child(const char* key, dalNodeType_e type = DT_OBJECT);	//Create new child
	dal_t*			get_child(const char* key);		//Access child node
	dal_t*			get_child(uint32_t idx);		//Access child node
	bool			has_child(const char* key);		//Check if node has chld with specified name
	uint32_t		count_childs();
	dal_t*			attach(dalStr_t* key, dal_t* node);
	dal_t*			attach(char* key, dal_t* node);
	dal_t*			attach(dal_t* node);
	void			detach();						//Detach from parent node
	//Adding child values
	dal_t*			add_val_bool(dalStr_t* key, bool value);
	dal_t*			add_val_int(dalStr_t* key, int64_t value);
	dal_t*			add_val_uint(dalStr_t* key, uint64_t value);
	dal_t*			add_val_dbl(dalStr_t* key, double value);
	dal_t*			add_val_str(dalStr_t* key, char* value);
	dal_t*			add_val_str(dalStr_t* key, const char* value);
	dal_t*			add_val_str(dalStr_t* key, char* value, uint32_t len);
	dal_t*			add_val_ref(dalStr_t* key, void* value, uint32_t len);
	dal_t*			add_val_blob(dalStr_t* key, void*& value, uint32_t len);
	dal_t*			add_val_bool(const char* key, bool value);
	dal_t*			add_val_int(const char* key, int64_t value);
	dal_t*			add_val_uint(const char* key, uint64_t value);
	dal_t*			add_val_dbl(const char* key, double value);
	dal_t*			add_val_str(const char* key, char* value);
	dal_t*			add_val_str(const char* key, const char* value);
	dal_t*			add_val_str(const char* key, char* value, uint32_t len);
	dal_t*			add_val_ref(const char* key, void* value, uint32_t len);
	dal_t*			add_val_blob(const char* key, void*& value, uint32_t len);
	//Adding child arrays
	dal_t*			add_array(dalStr_t* key, uint32_t count);
	dal_t*			add_array(const char* key);
	dal_t*			add_arr_bool(dalStr_t* key, bool* arr, uint32_t count);
	dal_t*			add_arr_int(dalStr_t* key, int* arr, uint32_t count);
	dal_t*			add_arr_uint(dalStr_t* key, unsigned int* arr, uint32_t count);
	dal_t*			add_arr_int32(dalStr_t* key, int32_t* arr, uint32_t count);
	dal_t*			add_arr_uint32(dalStr_t* key, uint32_t* arr, uint32_t count);
	dal_t*			add_arr_int64(dalStr_t* key, int64_t* arr, uint32_t count);
	dal_t*			add_arr_uint64(dalStr_t* key, uint64_t* arr, uint32_t count);
	dal_t*			add_arr_flt(dalStr_t* key, float* arr, uint32_t count);
	dal_t*			add_arr_dbl(dalStr_t* key, double* arr, uint32_t count);
	dal_t*			add_array(const char* key, uint32_t count);
	dal_t*			add_arr_bool(const char* key, bool* arr, uint32_t count);
	dal_t*			add_arr_int(const char* key, int* arr, uint32_t count);
	dal_t*			add_arr_uint(const char* key, unsigned int* arr, uint32_t count);
	dal_t*			add_arr_int32(const char* key, int32_t* arr, uint32_t count);
	dal_t*			add_arr_uint32(const char* key, uint32_t* arr, uint32_t count);
	dal_t*			add_arr_int64(const char* key, int64_t* arr, uint32_t count);
	dal_t*			add_arr_uint64(const char* key, uint64_t* arr, uint32_t count);
	dal_t*			add_arr_flt(const char* key, float* arr, uint32_t count);
	dal_t*			add_arr_dbl(const char* key, double* arr, uint32_t count);
	//Get item of array
	uint32_t		get_array_size();
	dal_t*			get_array_item(uint32_t idx);
	//Set value of the node
	dal_t&			operator=(bool value);
	dal_t&			operator=(uint64_t value);
	dal_t&			operator=(int64_t value);
	dal_t&			operator=(double value);
	dal_t&			operator=(char* value);
	dal_t&			operator=(dalStr_t& value);
	dal_t&			operator=(dalBlob_t& value);
	dal_t&			operator=(dalBlobRef_t& value);
	//Getting value of the child node by key
	bool			get_val_bool(const char* key, bool& value);
	bool			get_val_int(const char* key, int& value);
	bool			get_val_uint(const char* key, unsigned int& value);
	bool			get_val_int32(const char* key, int32_t& value);
	bool			get_val_uint32(const char* key, uint32_t& value);
	bool			get_val_int64(const char* key, int64_t& value);
	bool			get_val_uint64(const char* key, uint64_t& value);
	bool			get_val_flt(const char* key, float& value);
	bool			get_val_dbl(const char* key, double& value);
	bool			get_val_str(const char* key, const char*& value);
	bool			get_val_str(const char* key, char*& value);
	bool			get_val_str(const char* key, dalStr_t& value);
	bool			get_val_blob(const char* key, dalBlob_t& value);
	//Getting value by index of child or of array item
	bool			get_val_bool(uint32_t idx, bool& value);
	bool			get_val_int(uint32_t idx, int& value);
	bool			get_val_uint(uint32_t idx, unsigned int& value);
	bool			get_val_int32(uint32_t idx, int32_t& value);
	bool			get_val_uint32(uint32_t idx, uint32_t& value);
	bool			get_val_int64(uint32_t idx, int64_t& value);
	bool			get_val_uint64(uint32_t idx, uint64_t& value);
	bool			get_val_flt(uint32_t idx, float& value);
	bool			get_val_dbl(uint32_t idx, double& value);
	bool			get_val_str(uint32_t idx, const char*& value);
	bool			get_val_str(uint32_t idx, char*& value);
	bool			get_val_str(uint32_t idx, dalStr_t& value);
	bool			get_val_blob(uint32_t idx, dalBlob_t& value);
	//Getting array values
	bool			get_arr_bool(const char* key, bool* value, uint32_t& len);
	bool			get_arr_int(const char* key, int* value, uint32_t& len);
	bool			get_arr_uint(const char* key, unsigned int* value, uint32_t& len);
	bool			get_arr_int32(const char* key, int32_t* value, uint32_t& len);
	bool			get_arr_uint32(const char* key, uint32_t* value, uint32_t& len);
	bool			get_arr_int64(const char* key, int64_t* value, uint32_t& len);
	bool			get_arr_uint64(const char* key, uint64_t* value, uint32_t& len);
	bool			get_arr_flt(const char* key, float* value, uint32_t& len);
	bool			get_arr_dbl(const char* key, double* value, uint32_t& len);
	//Serilization methods
	uint32_t		to_mpack(uint8_t* buf, uint32_t size);
	uint32_t		to_json(uint8_t* buf, uint32_t size, bool pretty = true);
	//Deserilization methods
	dalResult_e		from_mpack(uint8_t* buf, uint32_t size);
	dalResult_e		from_json(uint8_t* buf, uint32_t size, uint32_t tokens = 8192);
	dalResult_e		from_webapi(uint8_t* buf, uint32_t size);
}dal_t;
//===============================================================================================================================


//===============================================================================================================================
//The memory allocation and deallocation function prototypes are platform-specific methods and should
//provided by the user
typedef struct
{
	dal_t*				(*alloc_node)();
	void*				(*alloc_data)(uint32_t size);
	void				(*free_node)(dal_t* data);
	void				(*free_data)(void* data);
}dalMemHooks_t;
//===============================================================================================================================


#endif /* DATA_ABSTRACTION_LAYER_STRUCT_H_ */
