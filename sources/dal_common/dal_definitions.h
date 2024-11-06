/*
 * Data Abstraction Layer (DAL)
 *
 *  Created on: 29 07 2022 г.
 *      Author: a.sheviakov
 */

#ifndef DAL_DEFINITIONS_H_
#define DAL_DEFINITIONS_H_

#define	ENABLE_MEMCPY

#ifdef _MSC_VER

	#include <stdint.h>
	typedef uint8_t				bool_t;
	#define TRUE				0x01
	#define FALSE				0x00

#else

	#ifndef NULL
		#define NULL				0
	#endif

	#ifndef bool_t
		typedef __UINT8_TYPE__		bool_t;
	#endif

	#ifndef TRUE
		#define TRUE				0x01
	#endif

	#ifndef FALSE
		#define FALSE				0x00
	#endif

	#ifndef uint8_t
		typedef __UINT8_TYPE__		uint8_t;
	#endif

	#ifndef uint16_t
		typedef __UINT16_TYPE__		uint16_t;
	#endif

	#ifndef uint32_t
		typedef __UINT32_TYPE__		uint32_t;
	#endif

	#ifndef uint64_t
		typedef __UINT64_TYPE__		uint64_t;
	#endif

	#ifndef int8_t
		typedef __INT8_TYPE__		int8_t;
	#endif

	#ifndef int16_t
		typedef __INT16_TYPE__		int16_t;
	#endif

	#ifndef int32_t
		typedef __INT32_TYPE__		int32_t;
	#endif

	#ifndef int64_t
		typedef __INT64_TYPE__		int64_t;
	#endif

#endif

#endif /* DAL_DEFINITIONS_H_ */
