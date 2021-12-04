#include "dal_core/dal_utils.h"
#include "mpack_to_dal.h"
#include "mpack/mpack-reader.h"


dalResult_e		_mpack2dal_deserialize_recurse(uint8_t level, void* ctx, dal_t** obj, char* itemKey)
{
	//Проверяем вложенность рекурсии, ее надо ограничить, иначе неправильные данные могут зациклить
	//вызовы
	if (level > 32)		return DAL_FORMAT_ERR;

	uint32_t		count				= 0;
	dal_t*			newObj				= NULL;
	uint32_t		strLen				= 0;
	const char*		srcAddr				= NULL;
	char*			dstAddr				= NULL;
	char			key[DAL_KEY_SIZE]	= {0, 0};
	dalResult_e		result				= DAL_OK;
	mpack_reader_t*	reader				= ctx;
	mpack_tag_t		tag					= mpack_read_tag(reader);
	if (mpack_reader_error(reader) != mpack_ok)		return DAL_FORMAT_ERR;

	switch (mpack_tag_type(&tag))
	{
		case mpack_type_map:	count	= mpack_tag_map_count(&tag);
								newObj	= dal_create_obj();
								dal_change_key(newObj, itemKey);
	            				if (*obj == NULL)	*obj	= newObj;
	            				else				dal_add_item(*obj, newObj);
	            				while (count-- > 0)
	            				{	//Проверяем, если следующий элемент - строка, то это название
	            					//элемента и его надо передать в парсер для этого элемента
	            					tag		= mpack_read_tag(reader);
	            					if (mpack_tag_type(&tag) == mpack_type_str)
	            					{	strLen		= mpack_tag_str_length(&tag);
	            						mpack_read_utf8_cstr(reader, key, sizeof(key), strLen);
	            		            	mpack_done_str(reader);
	            					}//Иначе выводим ошибку - мап должен содержать первым элементом строку
	            						else return DAL_FORMAT_ERR;
	            					//Обрабатываем второй элемент мапа
	            					result	= _mpack2dal_deserialize_recurse(level+1, reader, &newObj, key);
	            					if (result != DAL_OK)							return result;
	            					if (mpack_reader_error(reader) != mpack_ok)		return DAL_FORMAT_ERR;
	            				}
	            				mpack_done_map(reader);
	            				break;
		case mpack_type_array:	count	= mpack_tag_array_count(&tag);
								newObj	= dal_add_arr(*obj, itemKey);
								while (count-- > 0)
								{	//Обрабатываем элементы массива
	            					result	= _mpack2dal_deserialize_recurse(level+1, reader, &newObj, NULL);
	            					if (result != DAL_OK)							return result;
	            					if (mpack_reader_error(reader) != mpack_ok)		return DAL_FORMAT_ERR;
								}
								mpack_done_array(reader);
								break;
		case mpack_type_bool:	dal_add_bool(*obj, itemKey, mpack_tag_bool_value(&tag));				break;
		case mpack_type_int:	dal_add_int(*obj, itemKey, mpack_tag_int_value(&tag));					break;
		case mpack_type_uint:	dal_add_uint(*obj, itemKey, mpack_tag_uint_value(&tag));				break;
		case mpack_type_float:	dal_add_double(*obj, itemKey, mpack_tag_float_value(&tag));				break;
		case mpack_type_double:	dal_add_double(*obj, itemKey, mpack_tag_double_value(&tag));			break;
		case mpack_type_str:	strLen		= mpack_tag_str_length(&tag);
								srcAddr		= mpack_read_utf8_inplace(reader, strLen);
								dal_add_str(*obj, itemKey, (char*)srcAddr, strLen);
								mpack_done_str(reader);
								break;
		case mpack_type_bin:	count		= mpack_tag_bin_length(&tag);
								srcAddr		= mpack_read_bytes_inplace(reader, count);
								dstAddr		= NULL;
								dal_add_blob(*obj, itemKey, (void**)&dstAddr, count);
								if (dstAddr != NULL)	memcpy(dstAddr, srcAddr, count);
								mpack_done_bin(reader);
								break;
		default:	break;
	}
	return DAL_OK;
}





dal_t*	mpack2dal_deserialize(void* buf, uint32_t len)
{
	mpack_reader_t	reader;
	mpack_reader_init_data(&reader, buf, len);
	dal_t*			obj			= NULL;
	dalResult_e		result		= _mpack2dal_deserialize_recurse(0, &reader, &obj, NULL);
	mpack_reader_destroy(&reader);
	if (result == DAL_OK)	return obj;
	dal_delete(obj);
	return NULL;
};


