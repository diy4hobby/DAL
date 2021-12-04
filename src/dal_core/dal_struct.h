/*
 * dal_struct.h
 *
 *  Created on: 25 03 2020 г.
 *      Author: a.sheviakov
 */

#ifndef DATA_ABSTRACTION_LAYER_STRUCT_H_
#define DATA_ABSTRACTION_LAYER_STRUCT_H_

#include "stdint.h"


//Типы данных, которые может содержать поле
typedef enum
{
	DT_UNKN				= 0x0000,
	DT_BOOL				= 0x0001,
	DT_UINT				= 0x0002,
	DT_INT				= 0x0004,
	DT_DOUBLE			= 0x0010,
	DT_STRING			= 0x0040,
	DT_BLOB				= 0x0080,
	DT_BLOB_REF			= 0x0100,	//Указатель на статическую область памяти
	DT_ARRAY			= 0x1000,
	DT_OBJECT			= 0x2000,
}dataType_e;
//Комбинирование перечислений для получения констант, разрешающих запись и чтение
//данных из дружественных типов
#define	TUPLE_TYPE		(DT_ARRAY | DT_OBJECT)
#define	NUMBER_TYPE		(DT_BOOL | DT_UINT | DT_INT | DT_DOUBLE)
#define	BLOB_TYPE		(DT_BLOB | DT_BLOB_REF)


//Коды результатов выполнения методов библиотеки
typedef enum
{
	DAL_OK				= 0x00,
	DAL_MEM_ERR			= 0x01,
	DAL_DST_NOT_TUPLE	= 0x02,
	DAL_FORMAT_ERR		= 0x03,
}dalResult_e;


#define	DAL_KEY_SIZE			64
//Структура, хранящая комплексные данные, указатели на следующие по иерархии данные
//и/или соседние данные (равные в иерархии)
typedef struct dal_t
{
	struct dal_t*				parent;
	struct dal_t*				prev;
	struct dal_t*				next;
	struct dal_t*				child;
	char						key[DAL_KEY_SIZE];
	uint8_t						key_len;
	dataType_e					type;
	uint32_t					len;
	union{
		uint8_t					asBOOL;
		uint64_t				asUINT;
		int64_t					asINT;
		char*					asSTRING;
		uint8_t*				asBLOB;
	};
	double						asDOUBLE;
	void*						mem_ptr;	//Указатель на распределенную память (может быть не выровненным)
											//используется для asSTRING и asBLOB, что бы там иметь выровненные
											//по 8 байт указатели
}dal_t;

//Прототипы функций выделения и освобождения памяти - платформозависимые методы, должны
//предоставляться пользователем
typedef struct
{
	void*				(*alloc_node)();
	void*				(*alloc_data)(uint32_t size);
	void				(*free_node)(void* data);
	void				(*free_data)(void* data);
}dalMemHooks_t;

//Прототипы функций серилизации данных - предоставляются пользователем в зависимости
//от протокола, в который нужно серилизовать данные
typedef struct
{
	uint32_t			(*wr_bool)(void* ctx, char* key, uint8_t value);
	uint32_t			(*wr_uint)(void* ctx, char* key, uint64_t value);
	uint32_t			(*wr_int)(void* ctx, char* key, int64_t value);
	uint32_t			(*wr_dbl)(void* ctx, char* key, double value);
	uint32_t			(*wr_str)(void* ctx, char* key, char* value, uint32_t len);
	uint32_t			(*wr_blob)(void* ctx, char* key, void* value, uint32_t len);
	uint32_t			(*wr_obj_beg)(void* ctx, char* key, uint32_t items);
	uint32_t			(*wr_obj_end)(void* ctx);
	uint32_t			(*wr_arr_beg)(void* ctx, char* key, uint32_t items);
	uint32_t			(*wr_arr_end)(void* ctx);
}writerHooks_t;



#endif /* DATA_ABSTRACTION_LAYER_STRUCT_H_ */
