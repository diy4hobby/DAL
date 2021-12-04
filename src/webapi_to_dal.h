/*
 * webapi_to_dal.h
 *
 *  Created on: 25 03 2020 г.
 *      Author: a.sheviakov
 */

#ifndef WEBAPI_TO_DAL_H_
#define WEBAPI_TO_DAL_H_
#include "stdint.h"
#include "string.h"
#include "dal_core/dal_struct.h"



//Десерилизация данных из webApi в DAL
dalResult_e			webapi2dal_deserialize(char* args, dal_t** obj);




#endif /* WEBAPI_TO_DAL_H_ */
