#include "digitslut.h"
#include "int_to_str.h"



#define BEGIN2(n) \
    do { \
        int t = (n); \
        if(t < 10) *p++ = '0' + t; \
        else { \
            t *= 2; \
            *p++ = gDigitsLut[t]; \
            *p++ = gDigitsLut[t + 1]; \
        } \
    } while(0)

#define MIDDLE2(n) \
    do { \
        int t = (n) * 2; \
        *p++ = gDigitsLut[t]; \
        *p++ = gDigitsLut[t + 1]; \
    } while(0)

#define BEGIN4(n) \
    do { \
        int t4 = (n); \
        if(t4 < 100) BEGIN2(t4); \
        else { BEGIN2(t4 / 100); MIDDLE2(t4 % 100); } \
    } while(0)

#define MIDDLE4(n) \
    do { \
        int t4 = (n); \
        MIDDLE2(t4 / 100); MIDDLE2(t4 % 100); \
    } while(0)

#define BEGIN8(n) \
    do { \
        uint32_t t8 = (n); \
        if(t8 < 10000) BEGIN4(t8); \
        else { BEGIN4(t8 / 10000); MIDDLE4(t8 % 10000); } \
    } while(0)

#define MIDDLE8(n) \
    do { \
        uint32_t t8 = (n); \
        MIDDLE4(t8 / 10000); MIDDLE4(t8 % 10000); \
    } while(0)

#define MIDDLE16(n) \
    do { \
        uint64_t t16 = (n); \
        MIDDLE8((uint32_t)(t16 / 100000000)); MIDDLE8(t16 % 100000000); \
    } while(0)



void u32toa(uint32_t x, char* p, uint32_t* len) {

    char* beg	= p;
	if(x < 100000000) BEGIN8(x);
    else { BEGIN2(x / 100000000); MIDDLE8(x % 100000000); }
    *p = 0;
    if (len)	*len	= p - beg;
}
void i32toa(int32_t x, char* p, uint32_t* len) {

    uint32_t t;
    if(x >= 0) t = x;
    else *p++ = '-', t = -x;
    u32toa(t, p, len);
}
void u64toa(uint64_t x, char* p, uint32_t* len) {

	char* beg	= p;
	if(x < 100000000) BEGIN8((uint32_t)x);
    else if(x < 10000000000000000) { BEGIN8((uint32_t)(x / 100000000)); MIDDLE8(x % 100000000); }
    else { BEGIN4((int)(x / 10000000000000000)); MIDDLE16(x % 10000000000000000); }
    *p = 0;
    if (len)	*len	= p - beg;
}
void i64toa(int64_t x, char* p, uint32_t* len) {

    uint64_t t;
    if(x >= 0) t = x;
    else *p++ = '-', t = -x;
    u64toa(t, p, len);
}


