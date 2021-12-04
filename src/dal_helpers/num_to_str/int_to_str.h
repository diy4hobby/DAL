#ifndef INT_TO_STR_H_
#define INT_TO_STR_H_
#include "stdint.h"


void		u32toa(uint32_t x, char* p, uint32_t* len);
void		i32toa(int32_t x, char* p, uint32_t* len);
void		u64toa(uint64_t x, char* p, uint32_t* len);
void		i64toa(int64_t x, char* p, uint32_t* len);





#endif /* INT_TO_STR_H_ */
