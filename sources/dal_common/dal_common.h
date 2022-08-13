#ifndef DAL_COMMON_H_
#define DAL_COMMON_H_
#include "dal_base_types.h"

#ifdef __cplusplus
extern "C" {
#endif

	bool_t		dal_string_compare(char* str1, const char* str2, uint32_t len2);
	uint32_t	dal_string_length(const char* str, uint32_t maxLen);
	void		dal_string_copy(char* dst, const char* src, uint32_t maxLen);

	bool_t		dal_bytedata_compare(uint8_t* data1, uint8_t* data2, uint32_t len1, uint32_t len2);
	void		dal_bytedata_copy(void* data1, void* data2, uint32_t len);

	void*		dal_pointer_align_8(void* ptr);

	uint32_t	dal_hash_sdbm(const char* str, uint32_t maxLen);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif

#endif /* DAL_COMMON_H_ */
