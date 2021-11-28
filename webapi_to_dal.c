#include "webapi_to_dal.h"
#include "../helpers/numbers.h"
#include "dal_platform_mem.h"
#include "dalCore/dal_utils.h"

#define TRUE		1U
#define FALSE		0U
#define NULL		((void *)0)
#define INRANGE(x,a,b)  ((unsigned)((x)-(a)) <= (unsigned)((b)-(a)))
#define ISSPACE(c)  	((c)==' ' || INRANGE((c), '\t', '\r'))



dalResult_e	webapi2dal_process_pair(dal_t* obj, char* key, uint32_t keyLen, char* value, uint32_t valueLen)
{
	int64_t		intVal		= 0;
	double		dblVal		= 0;
	uint32_t	parsed		= FALSE;
	//Пытаемся распарсить значение как число
	//Сначала проверяем длину значения - для числа она не должна превышать
	//20 символов - это uint64_t
	if (valueLen < 21)		parsed	= strToNum(value, &dblVal, &intVal);
	if (parsed == TRUE)		dal_add_double(obj, key, dblVal);
	else
	{	//Если не распарсилось, значит значение или строка или массив
		if (*value == '[')
		{	//Значение массив - надо распарсить каждый элемент
			dal_t*	array	= dal_add_arr(obj, key);
			if (array == NULL)	return DAL_MEM_ERR;
			char		itemBuf[256];
			while ((*value != ']') && (valueLen > 0))
			{
				value++;					//Смещаемся на символ разделения (для первого числа - это смещение на символ [)
				valueLen--;
				while (ISSPACE(*value) && (valueLen > 0))
				{	//Убираем пробелы перед элеметом массива
					value++;
					valueLen--;
				}
				//Копируем значение в буффер
				uint32_t	itemBufLen	= 0;
				while ((*value != ',') && (*value != ']') && (itemBufLen < sizeof(itemBuf)) && (valueLen > 0))
				{	itemBuf[itemBufLen]	= *value;
					value++;
					itemBufLen++;
					valueLen--;
				}
				if ((itemBufLen == sizeof(itemBuf)) || (valueLen == 0))
				{	dal_delete(array);
					return DAL_FORMAT_ERR;
				}
				itemBuf[itemBufLen]		= 0x00;
				//Добавляем элементв в массив
				if (strToNum(itemBuf, &dblVal, &intVal) == TRUE)	dal_add_double(array, "", dblVal);
				else												dal_add_str(array, "", itemBuf, itemBufLen);
				while (ISSPACE(*value) && (valueLen > 0))
				{	//Убираем пробелы после элемента массива
					value++;
					valueLen--;
				}
			}
		}else
		{	//Проверяем строковые значения на логические - что бы если передано true, TRUE, false, FALSE
			//оно воспринималось как логическое
			if ((strcmp(value, "true") == 0) || (strcmp(value, "TRUE") == 0))				dal_add_bool(obj, key, TRUE);
			else	if ((strcmp(value, "false") == 0) || (strcmp(value, "FALSE") == 0))		dal_add_bool(obj, key, FALSE);
					else																	dal_add_str(obj, key, value, valueLen);	//Заносим значение как строку
		}
	}

	return DAL_OK;
}



dalResult_e	webapi2dal_deserialize(char* args, dal_t** obj)
{
	dal_t*			topObj		= dal_create_obj();
	char			key[DAL_KEY_SIZE]	= {0};
	uint32_t		keyLen		= 0;
	uint32_t		bufSize		= 8 * 65536;
	char*			buf			= dal_alloc_data(bufSize);
	uint32_t		bufLen		= 0;
	uint32_t		result		= DAL_OK;

	while (*args != 0x00)
	{	//Формируем ключ - должна быть строка без спец символов
		keyLen		= 0;
		while ((*args != 0x00) && (*args != '&') && (*args != '?')
				 && (*args != ':') && (*args != '=')
				 && (keyLen < sizeof(key)))
		{
			key[keyLen]		= *args;
			keyLen++;
			args++;
		}
		if (keyLen < sizeof(key))	key[keyLen]	= 0x00;
		//Проверяем валидность ключа
		if ((key[0] == 0x00) || (keyLen == sizeof(key)))
		{	dal_free_data(buf);
			dal_delete(topObj);
			return DAL_FORMAT_ERR;
		}
		//Смещаемся на спецсимвол '='
		if (*args != 0x00)		args++;
		//Формируем значение
		bufLen		= 0;
		while ((*args != 0x00) && (*args != '&') && (*args != '?')
				 && (*args != ':') && (bufLen < bufSize))
		{
			buf[bufLen]		= *args;
			bufLen++;
			args++;
		}
		if (bufLen < bufSize)		buf[bufLen]	= 0x00;
		if (bufLen > 0)
		{	//Парсим полученную пару ключ:значение
			result		= webapi2dal_process_pair(topObj, key, keyLen, buf, bufLen);
			if (result != DAL_OK)
			{	dal_free_data(buf);
				dal_delete(topObj);
				return result;
			}
		}
		//Смещаемся на спецсимвол '&' '?' ':'
		if (*args != 0x00)		args++;
	}
	*obj	= topObj;
	dal_free_data(buf);
	return DAL_OK;
};


