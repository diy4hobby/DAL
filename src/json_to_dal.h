/*
 * complexData_utils.h
 *
 *  Created on: 25 03 2020 г.
 *      Author: a.sheviakov
 */

#ifndef JSON_TO_DAL_H_
#define JSON_TO_DAL_H_
#include "stdint.h"
#include "dal_core/dal_struct.h"



//Десерилизация данных из JSON в DAL
dal_t*			json2dal_deserialize(void* buf, uint32_t len);




#endif /* JSON_TO_DAL_H_ */
