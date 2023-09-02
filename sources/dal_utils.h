/*
 * Data Abstraction Layer (DAL)
 *
 *  Created on: 08 06 2022 г.
 *      Author: a.sheviakov
 */

#ifndef DATA_ABSTRACTION_LAYER_UTILS_H_
#define DATA_ABSTRACTION_LAYER_UTILS_H_

#include "cstdint"
#include "dal_struct.h"


//DAL initialization - memory allocation and release methods must be passed. The methods must be reentrant.
dalResult_e		dal_init(dalMemHooks_t* mem);

//Creating an empty object
dal_t*			dal_create(dalNodeType_e type = DT_OBJECT);

//Delete object and all childs
void			dal_delete(dal_t* node);


#endif /* DATA_ABSTRACTION_LAYER_UTILS_H_ */
