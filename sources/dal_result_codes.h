/*
 * Data Abstraction Layer (DAL)
 *
 *  Created on: 25 06 2022 г.
 *      Author: a.sheviakov
 */

#ifndef DAL_RESULT_CODES_H_
#define DAL_RESULT_CODES_H_

#include "cstdint"


//Result codes for library methods execution
typedef enum
{
	DAL_OK				= 0x00,
	DAL_MEM_ERR			= 0x01,
	DAL_DST_NOT_TUPLE	= 0x02,
	DAL_FORMAT_ERR		= 0x03,
}dalResult_e;


#endif /* DAL_RESULT_CODES_H_ */
