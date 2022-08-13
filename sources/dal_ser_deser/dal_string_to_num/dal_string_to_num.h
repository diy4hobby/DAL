#ifndef DAL_STRING_TO_NUM_H_
#define DAL_STRING_TO_NUM_H_
#include "../../dal_common/dal_base_types.h"

typedef struct {
	double			dbl_value;
	bool_t			dbl_valid;
	int64_t			int_value;
	bool_t			int_valid;
}dalStrtonumResult_t;

#ifdef __cplusplus
extern "C" {
#endif

dalStrtonumResult_t		dal_strntonum(char** str, uint32_t* size);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif

#endif /* DAL_STRING_TO_NUM_H_ */
