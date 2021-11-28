/*
 * complexData_utils.h
 *
 *  Created on: 25 03 2020 г.
 *      Author: a.sheviakov
 */

#ifndef DAL_TO_JSON_H_
#define DAL_TO_JSON_H_
#include "stdint.h"
#include "dalCore/dal_struct.h"



//—ерилизаци¤ данных из DAL в JSON в переданный буффер
//если серилизаци¤ завершена удачно, в len будет записана длина серилизованных данных
//бинарные данные в текущей версии пропускаютс¤
dalResult_e		dal2json_serialize(dal_t* obj, void* buf, uint32_t bufSize, uint32_t* len, uint8_t minimize);




#endif /* DAL_TO_JSON_H_ */
