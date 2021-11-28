#include "dal_utils.h"
#include "string.h"

#define	NULL			((void *)0)
#define TRUE			1U
#define FALSE			0U

dalMemHooks_t			memHooks		= {NULL};



dalResult_e	dal_init(dalMemHooks_t* mem)
{
	//Устанавливаем переданные пользователем методы выделения
	//и освобождения памяти
	memHooks		= *mem;

	return DAL_OK;
};


dal_t*	_dal_find_obj(dal_t* obj, const char* key)
{
	if (obj == NULL)	return NULL;
	if (strcmp(obj->key, key) != 0)
	{	obj		= obj->child;
		while (obj != NULL)
		{	if (strcmp(obj->key, key) == 0)		break;
			obj	= obj->next;
		}
	}
	return obj;
}


dal_t*	dal_create_obj()
{
	dal_t*	newObj		= (dal_t*)memHooks.alloc_node();
	if (newObj == NULL)		return NULL;
	memset(newObj, 0, sizeof(dal_t));
	newObj->type			= DT_OBJECT;
	return newObj;
};

dal_t*	dal_create_array(const char* key)
{
	dal_t*	newObj		= (dal_t*)memHooks.alloc_node();
	if (newObj == NULL)		return NULL;
	memset(newObj, 0, sizeof(dal_t));
	if ((key != NULL) && (*key != 0x00))
	{
		newObj->key_len		= strlen(key) + 1;	//+1 takes into account the 0 at the end of the line
		newObj->key_len		= (newObj->key_len > sizeof(newObj->key)) ? sizeof(newObj->key) : newObj->key_len;
		memcpy(newObj->key, key, newObj->key_len-1);
	}
	newObj->type			= DT_ARRAY;
	return newObj;
};


dal_t*	_dal_copy_recurse(dal_t* item, dal_t* prevLevel)
{
	if (item == NULL)		return prevLevel;

	switch (item->type)
	{
		case DT_UNKN:		break;
		case DT_BOOL:		if (prevLevel != NULL)	dal_add_bool(prevLevel, item->key, item->asBOOL);		break;
		case DT_UINT:		if (prevLevel != NULL)	dal_add_uint(prevLevel, item->key, item->asUINT);		break;
		case DT_INT:		if (prevLevel != NULL)	dal_add_int(prevLevel, item->key, item->asINT);			break;
		case DT_DOUBLE:		if (prevLevel != NULL)	dal_add_double(prevLevel, item->key, item->asDOUBLE);	break;
		case DT_STRING:		if (prevLevel == NULL)	break;
							dal_add_str(prevLevel, item->key, item->asSTRING, item->len);
							break;
		case DT_BLOB:		if (prevLevel == NULL)	break;
							void*		addr	= NULL;
							dal_add_blob(prevLevel, item->key, &addr, item->len);
							if (addr != NULL)	memcpy(addr, item->asBLOB, item->len);
							break;
		case DT_BLOB_REF:	if (prevLevel != NULL)	dal_add_blobref(prevLevel, item->key, item->asBLOB, item->len);	break;
		case DT_ARRAY:		;dal_t*	array		= dal_create_array(item->key);
							if (prevLevel != NULL)	dal_add_item(prevLevel, array);
							else					prevLevel	= array;
							dal_t*	arrItem		= item->child;
							while (arrItem != NULL)
							{	_dal_copy_recurse(arrItem, array);
								arrItem			= arrItem->next;
							}
							return array;
							break;
		case DT_OBJECT:		;dal_t*	obj			= dal_create_obj();
							if (prevLevel != NULL)
							{	dal_change_key(obj, item->key);
								dal_add_item(prevLevel, obj);
							}else prevLevel		= obj;
							dal_t*	child		= item->child;
							while (child != NULL)
							{	_dal_copy_recurse(child, obj);
								child			= child->next;
							}
							return obj;
							break;
	}
	return prevLevel;
}
dal_t*	dal_copy(dal_t* obj)
{
	if (obj == NULL)	return NULL;
	return _dal_copy_recurse(obj, NULL);
};


void	dal_change_key(dal_t* obj, const char* newKey)
{
	if (obj == NULL)	return;
	if (newKey == NULL)	return;
	obj->key_len			= strlen(newKey);
	if (obj->key_len >= sizeof(obj->key))	obj->key_len	= sizeof(obj->key) - 1;
	memcpy(obj->key, newKey, obj->key_len);
	obj->key[obj->key_len]	= 0x00;
	return;
};


uint8_t	dal_has_key(dal_t* obj)
{
	if (obj->key[0] == 0x00)				return FALSE;
	else									return TRUE;
};


uint8_t	dal_has_item(dal_t* obj, const char* key)
{
	if (obj == NULL)	return FALSE;
	obj		= obj->child;
	while (obj != NULL)
	{	if (strcmp(obj->key, key) == 0)		return TRUE;
		obj	= obj->next;
	}
	return FALSE;
};


dalResult_e	dal_add_item(dal_t* obj, dal_t* item)
{
	if ((obj == NULL) || (item == NULL))	return DAL_OK;
	//Проверяем, является ли объект назначения кортежем - он
	//должен быть либо объектом либо массивом, в другие типы
	//вставлять дочерние нельзя
	if ((obj->type & TUPLE_TYPE) == 0)	return DAL_DST_NOT_TUPLE;
	//Убеждаемся, что элемент изъят из другого объекта
	dal_eject_item(item);
	//Если у объекта нет дочерних, делаем добавляемый объект дочерним,
	//для добавляемого родительским делаем тот, в который добавляем
	if (obj->child == NULL)
	{	obj->child		= item;
		item->parent	= obj;
		return DAL_OK;
	}
	//У объекта есть дочерние - нужно пройти по всем ним пока не
	//дойдем до того, у кого следующего нет и делаем следующим
	//добавляемый элемент
	item->parent	= obj;
	obj				= obj->child;
	while (obj->next != NULL)	obj	= obj->next;
	obj->next		= item;
	item->prev		= obj;
	return DAL_OK;
};

dalResult_e	dal_eject_item(dal_t* item)
{
	if (item == NULL)	return DAL_OK;
	//Если у извлекаемого есть родитель, то нужно проверить,
	//не является ли данный элемент первым в списке дочерних, если да
	//до надо убрать его из списка дочерних и назначить первым следующий
	//элемент
	if (item->parent != NULL)
	{
		if (item->parent->child == item)
		{	item->parent->child		= item->next;
			item->parent			= NULL;
		}
	}
	//Теперь связываем объекты слева и справа от извлекаемого элемента
	if (item->prev != NULL)		item->prev->next	= item->next;
	if (item->next != NULL)		item->next->prev	= item->prev;
	item->prev			= NULL;
	item->next			= NULL;
	return DAL_OK;
};

//Изымание элемента по ключу - как dal_eject_item
dal_t*	dal_remove_item(dal_t* obj, char* key)
{
	if (key == NULL)	return NULL;
	dal_t*	item	= _dal_find_obj(obj, key);
	dalResult_e		result	= dal_eject_item(item);
	if (result != DAL_OK)	return NULL;
	else					return item;
};


dalResult_e	_dal_delete_recurse(dal_t* item)
{
	switch (item->type)
	{
		case DT_UNKN:
		case DT_BOOL:
		case DT_UINT:
		case DT_INT:
		case DT_DOUBLE:		memHooks.free_node(item);
							break;
		case DT_STRING:		memHooks.free_data(item->mem_ptr);
							memHooks.free_node(item);
							break;
		case DT_BLOB:		memHooks.free_data(item->mem_ptr);
							memHooks.free_node(item);
							break;
		case DT_BLOB_REF:	memHooks.free_node(item);			break;
		case DT_ARRAY:
		case DT_OBJECT:		;dal_t*	child		= item->child;
							while (child != NULL)
							{	dal_t*	next	= child->next;
								_dal_delete_recurse(child);
								child			= next;
							}
							memHooks.free_node(item);
							break;
	}
	return DAL_OK;
}

dalResult_e	dal_delete(dal_t* item)
{
	if (item == NULL)	return DAL_OK;
	//Убеждаемся, что элемент удален из родительского объекта
	dal_eject_item(item);
	//Вызываем удаление элемента с рекурсией
	_dal_delete_recurse(item);
	return DAL_OK;
};



dal_t*	dal_add_obj(dal_t* obj, const char* key)
{
	dal_t*	newObj		= dal_create_obj();
	if (newObj == NULL)		return NULL;
	if ((key != NULL) && (*key != 0x00))
	{
		newObj->key_len	= strlen(key) + 1;	//+1 учитывает 0 в конце строки
		newObj->key_len	= (newObj->key_len > sizeof(newObj->key)) ? sizeof(newObj->key) : newObj->key_len;
		memcpy(newObj->key, key, newObj->key_len-1);
	}
	dal_add_item(obj, newObj);
	return newObj;
};

dal_t*	dal_add_arr(dal_t* dst, const char* key)
{
	dal_t*	newObj			= dal_add_obj(dst, key);
	if (newObj == NULL)		return NULL;
	newObj->type			= DT_ARRAY;
	return newObj;
};

dal_t*	dal_add_bool(dal_t* dst, const char* key, uint8_t value)
{
	if (dst == NULL)		return NULL;
	dal_t*	newObj			= dal_add_obj(dst, key);
	if (newObj == NULL)		return NULL;
	newObj->type			= DT_BOOL;
	newObj->asBOOL			= value;
	newObj->len				= sizeof(uint8_t);
	return newObj;
};

dal_t*	dal_add_uint(dal_t* dst, const char* key, uint64_t value)
{
	if (dst == NULL)		return NULL;
	dal_t*	newObj			= dal_add_obj(dst, key);
	if (newObj == NULL)		return NULL;
	newObj->type			= DT_UINT;
	newObj->asUINT			= value;
	newObj->asDOUBLE		= (double)value;
	newObj->len				= sizeof(uint64_t);
	return newObj;
};

dal_t*	dal_add_int(dal_t* dst, const char* key, int64_t value)
{
	if (dst == NULL)		return NULL;
	dal_t*	newObj			= dal_add_obj(dst, key);
	if (newObj == NULL)		return NULL;
	newObj->type			= DT_INT;
	newObj->asINT			= value;
	newObj->asDOUBLE		= (double)value;
	newObj->len				= sizeof(int64_t);
	return newObj;
};

dal_t*	dal_add_double(dal_t* dst, const char* key, double value)
{
	if (dst == NULL)		return NULL;
	dal_t*	newObj			= dal_add_obj(dst, key);
	if (newObj == NULL)		return NULL;
	newObj->type			= DT_DOUBLE;
	newObj->asDOUBLE		= value;
	newObj->asINT			= (int64_t)value;
	newObj->len				= sizeof(float);
	return newObj;
};

dal_t*	dal_add_cstr(dal_t* dst, const char* key, const char* value)
{
	if (dst == NULL)		return NULL;
	dal_t*	newObj			= dal_add_obj(dst, key);
	if (newObj == NULL)		return NULL;
	newObj->type			= DT_STRING;
	uint32_t	valLen		= (value != NULL) ? (strlen(value) + 1) : 1;	//+1 учитывает 0 в конце строки
	newObj->mem_ptr			= memHooks.alloc_data(valLen + 8);
	newObj->asSTRING		= (char*)((size_t)(newObj->mem_ptr + 7) & ~(size_t)0x07);
	if (newObj->asSTRING == NULL)
	{	dal_delete(newObj);
		return NULL;
	}
	if (value != NULL)
	{		memcpy(newObj->asSTRING, value, valLen-1);
			newObj->asSTRING[valLen-1]	= 0x00;
	}else	newObj->asSTRING[0]			= 0x00;
	newObj->len				= valLen-1;
	return newObj;
};

dal_t*	dal_add_str(dal_t* dst, const char* key, char* value, uint32_t len)
{
	if (dst == NULL)		return NULL;
	if (value == NULL)		return NULL;
	dal_t*	newObj			= dal_add_obj(dst, key);
	if (newObj == NULL)		return NULL;
	newObj->type			= DT_STRING;
	len						+= 1;	//+1 учитывает 0 в конце строки
	newObj->mem_ptr			= memHooks.alloc_data(len + 8);
	newObj->asSTRING		= (char*)((size_t)(newObj->mem_ptr + 7) & ~(size_t)0x07);
	if (newObj->asSTRING == NULL)
	{	dal_delete(newObj);
		return NULL;
	}
	memcpy(newObj->asSTRING, value, len-1);
	newObj->asSTRING[len-1]	= 0x00;
	newObj->len				= len-1;
	return newObj;
};

dal_t*	dal_add_blob(dal_t* dst, const char* key, void** value, uint32_t len)
{
	*value					= NULL;
	if (dst == NULL)		return NULL;
	dal_t*	newObj			= dal_add_obj(dst, key);
	if (newObj == NULL)		return NULL;
	newObj->type			= DT_BLOB;
	newObj->mem_ptr			= memHooks.alloc_data(len + 8);
	newObj->asBLOB			= (uint8_t*)((size_t)(newObj->mem_ptr + 7) & ~(size_t)0x07);
	if (newObj->asBLOB == NULL)
	{	dal_delete(newObj);
		return NULL;
	}
	*value					= newObj->asBLOB;
	newObj->len				= len;
	return newObj;
};

dal_t*	dal_add_blobref(dal_t* dst, const char* key, void* value, uint32_t len)
{
	if (dst == NULL)		return NULL;
	dal_t*	newObj			= dal_add_obj(dst, key);
	if (newObj == NULL)		return NULL;
	newObj->type			= DT_BLOB_REF;
	newObj->asBLOB			= value;
	newObj->len				= len;
	return newObj;
};




dal_t*	dal_get_obj(dal_t* obj, const char* key)
{
	if (obj == NULL)								return NULL;
	if (obj->type != DT_ARRAY)		obj	= _dal_find_obj(obj, key);
	if ((obj != NULL) && (obj->type & TUPLE_TYPE))	return obj;
	else											return NULL;
};

dal_t*	dal_get_arr(dal_t* obj, const char* key)
{
	if (obj == NULL)								return NULL;
	if (obj->type != DT_ARRAY)		obj	= _dal_find_obj(obj, key);
	if ((obj != NULL) && (obj->type == DT_ARRAY))	return obj;
	else											return NULL;
};

uint8_t	dal_get_bool(dal_t* obj, const char* key, uint8_t def)
{
	if (obj == NULL)								return def;
	if (obj->type != DT_ARRAY)		obj	= _dal_find_obj(obj, key);
	if ((obj != NULL) && (obj->type & NUMBER_TYPE))	return obj->asBOOL;
	else											return def;
};

uint64_t	dal_get_uint(dal_t* obj, const char* key, uint64_t def)
{
	if (obj == NULL)								return def;
	if (obj->type != DT_ARRAY)		obj	= _dal_find_obj(obj, key);
	if ((obj != NULL) && (obj->type & NUMBER_TYPE))	return obj->asUINT;
	else											return def;
};

int64_t		dal_get_int(dal_t* obj, const char* key, int64_t def)
{
	if (obj == NULL)								return def;
	if (obj->type != DT_ARRAY)		obj	= _dal_find_obj(obj, key);
	if ((obj != NULL) && (obj->type & NUMBER_TYPE))	return obj->asINT;
	else											return def;
};

double	dal_get_double(dal_t* obj, const char* key, double def)
{
	if (obj == NULL)								return def;
	if (obj->type != DT_ARRAY)		obj	= _dal_find_obj(obj, key);
	if ((obj != NULL) && (obj->type & NUMBER_TYPE))	return obj->asDOUBLE;
	else											return def;
};

char*	dal_get_str(dal_t* obj, const char* key)
{
	if (obj == NULL)								return NULL;
	if (obj->type != DT_ARRAY)		obj	= _dal_find_obj(obj, key);
	if ((obj != NULL) && (obj->type == DT_STRING))	return obj->asSTRING;
	else											return NULL;
};

void*	dal_get_blob(dal_t* obj, const char* key)
{
	if (obj == NULL)								return NULL;
	if (obj->type != DT_ARRAY)		obj	= _dal_find_obj(obj, key);
	if ((obj != NULL) && (obj->type & BLOB_TYPE))	return obj->asBLOB;
	else											return NULL;
};

uint32_t	dal_get_blob_size(dal_t* obj, const char* key)
{
	if (obj == NULL)								return 0;
	if (obj->type != DT_ARRAY)		obj	= _dal_find_obj(obj, key);
	if ((obj != NULL) && (obj->type & BLOB_TYPE))	return obj->len;
	else											return 0;
};


uint32_t	_dal_get_size_recurse(dal_t* item)
{
	switch (item->type)
	{
		case DT_UNKN:		return 0;
		case DT_BOOL:		return (2 + sizeof(uint8_t) + item->key_len);
		case DT_UINT:		return (2 + sizeof(uint64_t) + item->key_len);
		case DT_INT:		return (2 + sizeof(int64_t) + item->key_len);
		case DT_DOUBLE:		return (2 + sizeof(double) + item->key_len);
		case DT_STRING:		return (2 + item->len + item->key_len);
		case DT_BLOB:		return (2 + item->len + item->key_len);
		case DT_BLOB_REF:	return (2 + item->len + item->key_len);
		case DT_ARRAY:
		case DT_OBJECT:		;dal_t*	child		= item->child;
							uint32_t	size	= 2;
							while (child != NULL)
							{	dal_t*	next	= child->next;
								size			+= _dal_get_size_recurse(child);
								child			= next;
							}
							return (size + item->key_len);
	}
	return 0;
}

uint32_t	dal_get_size(dal_t* obj)
{
	return _dal_get_size_recurse(obj);
};





uint8_t	dal_is_obj(dal_t* obj)
{
	if (obj == NULL)			return FALSE;
	if (obj->type == DT_OBJECT)	return TRUE;
	else						return FALSE;
};

uint8_t	dal_is_array(dal_t* obj)
{
	if (obj == NULL)			return FALSE;
	if (obj->type == DT_ARRAY)	return TRUE;
	else						return FALSE;
};

uint8_t	dal_is_empty(dal_t* obj)
{
	if (obj == NULL)			return TRUE;
	if (obj->child == NULL)		return TRUE;
	else						return FALSE;
};

uint8_t	dal_is_number(dal_t* obj)
{
	if (obj == NULL)			return FALSE;
	if (obj->type & NUMBER_TYPE)return TRUE;
	else						return FALSE;
};

uint8_t	dal_is_str(dal_t* obj)
{
	if (obj == NULL)			return FALSE;
	if (obj->asSTRING == NULL)	return FALSE;
	if (obj->type == DT_STRING)	return TRUE;
	else						return FALSE;
};






dalResult_e	_dal_serialize_recurse(void* ctx, writerHooks_t* wrHooks, dal_t* obj)
{
	dal_t*		next	= NULL;
	dal_t*		child	= obj->child;
	uint32_t	count	= 0;
	char*		key		= NULL;

	while (obj != NULL)
	{
		next		= obj->next;
		//Проверяем, разрешена ли запись ключа - ключ нельзя записывать
		//для верхнего элемента и элементов массива
		if (obj->parent == NULL)						key	= NULL;
		else	if (obj->parent->type == DT_ARRAY)		key	= NULL;
				else									key	= obj->key;

		switch (obj->type)
		{
			case DT_UNKN:		break;
			case DT_BOOL:		wrHooks->wr_bool(ctx, key, obj->asBOOL);		break;
			case DT_UINT:		wrHooks->wr_uint(ctx, key, obj->asUINT);		break;
			case DT_INT:		wrHooks->wr_int(ctx, key, obj->asINT);			break;
			case DT_DOUBLE:		wrHooks->wr_dbl(ctx, key, obj->asDOUBLE);		break;
			case DT_STRING:		wrHooks->wr_str(ctx, key, obj->asSTRING, obj->len);
								break;
			case DT_BLOB:		wrHooks->wr_blob(ctx, key, obj->asBLOB, obj->len);
								break;
			case DT_BLOB_REF:	wrHooks->wr_blob(ctx, key, obj->asBLOB, obj->len);
								break;
			case DT_ARRAY:		//Нужно посчитать количество дочерних элементов
								child	= obj->child;
								count	= 0;
								while (child != NULL)
								{	count++;
									child	= child->next;
								}
								wrHooks->wr_arr_beg(ctx, key, count);
								_dal_serialize_recurse(ctx, wrHooks, obj->child);
								wrHooks->wr_arr_end(ctx);
								break;
			case DT_OBJECT:		//Нужно посчитать количество дочерних элементов
								child	= obj->child;
								count	= 0;
								while (child != NULL)
								{	count++;
									child	= child->next;
								}
								wrHooks->wr_obj_beg(ctx, key, count);
								_dal_serialize_recurse(ctx, wrHooks, obj->child);
								wrHooks->wr_obj_end(ctx);
								break;
		}
		//Переходим к следующему элементу
		obj		= next;
	}

	return DAL_OK;
};


dalResult_e	dal_serialize(void* ctx, writerHooks_t* wrHooks, dal_t* obj)
{
	if ((ctx == NULL) || (wrHooks == NULL) || (obj == NULL))		return DAL_MEM_ERR;
	return _dal_serialize_recurse(ctx, wrHooks, obj);
};




