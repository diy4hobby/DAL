#ifndef DAL_NUM_TO_STRING_H_
#define DAL_NUM_TO_STRING_H_
#include "../../dal_common/dal_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

	void		dal_dtoa(double fp, char* dest, uint32_t* len);

	void		dal_u32toa(uint32_t x, char* p, uint32_t* len);
	void		dal_i32toa(int32_t x, char* p, uint32_t* len);
	void		dal_u64toa(uint64_t x, char* p, uint32_t* len);
	void		dal_i64toa(int64_t x, char* p, uint32_t* len);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif

#endif /* DAL_NUM_TO_STRING_H_ */
