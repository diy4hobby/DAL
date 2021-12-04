/*
 * complexData_utils.h
 *
 *  Created on: 25 03 2020 г.
 *      Author: a.sheviakov
 */

#ifndef MPACK_TO_DAL_H_
#define MPACK_TO_DAL_H_
#include "stdint.h"
#include "dal_core/dal_struct.h"



//Десерилизация данных из MessagePack в DAL
dal_t*			mpack2dal_deserialize(void* buf, uint32_t len);




#endif /* MPACK_TO_DAL_H_ */
