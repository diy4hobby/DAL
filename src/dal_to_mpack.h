/*
 * complexData_utils.h
 *
 *  Created on: 25 03 2020 г.
 *      Author: a.sheviakov
 */

#ifndef DAL_TO_MPACK_H_
#define DAL_TO_MPACK_H_
#include "stdint.h"
#include "dal_core/dal_struct.h"



//—ерилизаци¤ данных из DAL в MessagePack в переданный буффер
//если серилизаци¤ завершена удачно, в len будет записана длина серилизованных данных
dalResult_e		dal2mpack_serialize(dal_t* obj, void* buf, uint32_t bufSize, uint32_t* len);




#endif /* DAL_TO_MPACK_H_ */
