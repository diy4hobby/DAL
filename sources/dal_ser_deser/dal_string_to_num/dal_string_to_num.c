#include "dal_string_to_num.h"
#include "dal_string_to_num_common.h"


#define	DAL_FLOAT_SMALLEST_POWER	-325
#define	DAL_FLOAT_LARGEST_POWER		308
#define	DAL_MAX_POSITIVE_INT		9223372036854775807
#define	DAL_MAX_NEGATIVE_INT		-9223372036854775807


typedef struct
{
	uint64_t	low;
	uint64_t	high;
}_dalValue128_t;

typedef union
{
	double		dbl;
	uint64_t	uint;
}_dalBitDouble_u;



static uint32_t	_dal_clz64(uint64_t x)
{
	uint8_t		count		= 0;
	if (x & (uint64_t)0xffffffff00000000)	x >>= 32, count |= 32;
	if (x & (uint64_t)0xffff0000)			x >>= 16, count |= 16;
	if (x & (uint64_t)0xff00)				x >>=  8, count |=  8;
	if (x & (uint64_t)0xf0)					x >>=  4, count |=  4;
	if (x & (uint64_t)0xc)					x >>=  2, count |=  2;
	if (x & (uint64_t)0x2)					x >>=  1, count |=  1;
	return 63 - count;
}

static inline bool_t	_dal_is_integer(char c)
{
	return (c >= '0' && c <= '9');
}

static inline double	_dal_construct_double_nan()
{
	_dalBitDouble_u	result;
	result.uint			= 0x7ff8000000000000;
	return result.dbl;
}

static inline double	_dal_construct_double_pinf()
{
	_dalBitDouble_u	result;
	result.uint			= 0x7ff0000000000000;
	return result.dbl;
}

static inline double	_dal_construct_double_ninf()
{
	_dalBitDouble_u	result;
	result.uint			= 0xfff0000000000000;
	return result.dbl;
}

static double _dal_str_to_num_hex(const char *s, char **e, uint16_t neg, int64_t* asInt)
{
	const char	*q, *p		= s;						// s = xxx[.]xxx[p+dd]
	static char	lookup[]	=
	".!\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9!!!!!!!"
	"\xa\xb\xc\xd\xe\xf!!!!!!!!!p!!!!!!!!!!!!!!!!"
	"\xa\xb\xc\xd\xe\xf!!!!!!!!!p";

	uint64_t	m			= 0;
	int32_t		i=0, d=0, bexp=0, pt=-1, sign=1;

	for(; *p=='0'; p++);
	int32_t		lz			= (int32_t)(p - s);

	while ((uint32_t)(*p - '.') <= (uint32_t)('p' - '.'))
	{
		switch (d = lookup[*p - '.'])
		{
			case '.':
				if (pt != -1)				goto HEX_CVT;
				if (++p, (pt = i) == 0)		// .(0*)xxx
					for(; *p=='0'; p++, lz++)	bexp -= 4;
				break;

			case 'p':						// add bexp
				if (*(q = p+1) == '-')	q++, sign = -1;
				else if	(*q == '+')		q++;
				if (!_dal_is_integer(*q))	goto HEX_CVT;
				for( p=q+1, d=*q-'0'; _dal_is_integer(*p); p++)
					if (d < 100000000)	d = 10 * d + *p - '0';
				bexp		+= sign * d;
				// fall thru
				goto HEX_CVT;

			case '!':	goto HEX_CVT;	// bad input

			default: ++i, ++p;				// hex digits
				if (i < 16)		m = m*16 + d;
				else if (d)		m |= 1;		// sticky bit
		}
	}

	if (asInt != NULL)			*asInt	= m;

HEX_CVT:
	if (e) *e	= (char *) (i + lz ? p : s-1);
	if (!i)			return neg ? -0. : 0.;
	if (pt == -1)	pt = i;
	if (i > 15)		i = 15;
	lz				= _dal_clz64(m) - 1;
	bexp			+= (pt-i)*4 + 1021 + 63 - lz;

	if (!((uint32_t)(bexp - -53) <= (uint32_t)(2045 - -53)))
	{
		if (asInt != NULL)		*asInt		= 0;
		if (neg)				return _dal_construct_double_ninf();
		else					return _dal_construct_double_pinf();
	}

	d				= 0;
	if (bexp < 0)	d		=-bexp, bexp=0;		// pos d = denormal
	m				<<= lz;
	if (lz < 10 + d)
	{								// m > 53 - d bits
		uint64_t	halfway	= 0x200ULL<<d;
		if (m & (halfway * 3 - 1))	m += halfway;
	}

	_dalBitDouble_u	val;
	val.uint		= ((uint64_t)(neg | bexp) << 52) + (m >> (10 + d));
	return val.dbl;
}

static inline _dalValue128_t _dal_multiply_128(uint64_t x, uint64_t y)
{
	const	uint64_t	x0	= (uint32_t)x,	x1	= x >> 32;
	const	uint64_t	y0	= (uint32_t)y,	y1	= y >> 32;
	const	uint64_t	p11	= x1 * y1,		p01	= x0 * y1;
	const	uint64_t	p10	= x1 * y0,		p00	= x0 * y0;
	_dalValue128_t		result;

	//64-bit product + two 32-bit values
	const	uint64_t	middle	= p10 + (p00 >> 32) + (uint32_t)p01;
	// 64-bit product + two 32-bit values
	result.high				= p11 + (middle >> 32) + (p01 >> 32);
	result.low				= (middle << 32) | (uint32_t)p00;
	return result;
}

double _dal_compute_float_64(int64_t power, uint64_t i, bool_t negative, bool_t *success)
{
	//Clinger WD. How to read floating point numbers accurately.
	//ACM SIGPLAN Notices. 1990
	if (-22 <= power && power <= 22 && i <= 9007199254740991)
	{
		double	d		= (double)i;
		if (power < 0)	d	= d / _dal_powoften_dbl[-power];
		else			d	= d * _dal_powoften_dbl[power];
		if (negative)	d	= -d;
		*success			= TRUE;
		return d;
	}

	if(i == 0)	return negative ? -0.0 : 0.0;

	uint64_t	factor_mantissa	= mantissa_64[power - DAL_FLOAT_SMALLEST_POWER];

	int64_t		exponent		= (((152170 + 65536) * power) >> 16) + 1024 + 63;
	uint32_t	lz				= _dal_clz64(i);
	i							<<= lz;

	_dalValue128_t	product		= _dal_multiply_128(i, factor_mantissa);
	uint64_t lower				= product.low;
	uint64_t upper				= product.high;

	if (((upper & 0x1FF) == 0x1FF) && (lower + i < lower))
	{
		uint64_t	factor_mantissa_low = mantissa_128[power - DAL_FLOAT_SMALLEST_POWER];
		product							= _dal_multiply_128(i, factor_mantissa_low);
		uint64_t	product_low			= product.low;
		uint64_t	product_middle2		= product.high;
		uint64_t	product_middle1		= lower;
		uint64_t	product_high		= upper;
		uint64_t	product_middle		= product_middle1 + product_middle2;
		if (product_middle < product_middle1)
		{
			product_high++; // overflow carry
		}

		if ((product_middle + 1 == 0) && ((product_high & 0x1FF) == 0x1FF) &&
				 (product_low + i < product_low))
		{
			*success = FALSE;
			return 0;
		}
		upper							= product_high;
		lower							= product_middle;
	}

	uint64_t		upperbit			= upper >> 63;
	uint64_t		mantissa			= upper >> (upperbit + 9);
	lz		+= (1 ^ (uint32_t)upperbit);

	if ((lower == 0) && ((upper & 0x1FF) == 0) && ((mantissa & 3) == 1))
	{
		*success	= FALSE;
		return 0;
	}

	mantissa		+= mantissa & 1;
	mantissa		>>= 1;
	if (mantissa >= (1ULL << 53))
	{	mantissa	= (1ULL << 52);
		lz--;	//undo previous addition
	}

	mantissa					&= ~(1ULL << 52);
	uint64_t	real_exponent	= exponent - lz;
	if ((real_exponent < 1) || (real_exponent > 2046))
	{
		*success	= FALSE;
		return 0;
	}

	mantissa		|= real_exponent << 52;
	mantissa		|= (((uint64_t)negative) << 63);
	_dalBitDouble_u d;
	d.uint			= mantissa;
	*success		= TRUE;
	return d.dbl;
}




dalStrtonumResult_t	dal_strntonum(char** str, uint32_t* size)
{
	dalStrtonumResult_t		result	= {	.dbl_value = 0.0, .dbl_valid = FALSE,
										.int_value = 0, .int_valid = FALSE};

	while ((**str == ' ') || (**str == '\t') || (**str == '\r') || (**str == '\n'))
	{	++*str;
		--*size;
	}

	if (*size > 2)
	{
		if ((((*str)[0] == 'n') || ((*str)[0] == 'N')) &&
			(((*str)[1] == 'a') || ((*str)[1] == 'A')) &&
			(((*str)[2] == 'n') || ((*str)[2] == 'N')))
		{
			result.dbl_value	= _dal_construct_double_nan();
			*str				+= 3;
			*size				-= 3;
			return result;
		}

		if ((((*str)[0] == 'i') || ((*str)[0] == 'I')) &&
			(((*str)[1] == 'n') || ((*str)[1] == 'N')) &&
			(((*str)[2] == 'f') || ((*str)[2] == 'F')))
		{
			result.dbl_valid	= TRUE;
			result.dbl_value	= _dal_construct_double_pinf();
			*str				+= 3;
			*size				-= 3;
			return result;
		}
	}

	if (*size > 3)
	{
		if (((*str)[0] == '-') &&
			(((*str)[1] == 'i') || ((*str)[1] == 'I')) &&
			(((*str)[2] == 'n') || ((*str)[2] == 'N')) &&
			(((*str)[3] == 'f') || ((*str)[3] == 'F')))
		{
			result.dbl_valid	= TRUE;
			result.dbl_value	= _dal_construct_double_ninf();
			*str				+= 4;
			*size				-= 4;
			return result;
		}
	}

	bool_t	found_minus		= (**str == '-');
	bool_t	negative		= FALSE;
	if (found_minus)
	{
		++*str;
		--*size;
		negative			= TRUE;
		if (!_dal_is_integer(**str))
		{	//A negative sign must be followed by an integer
			return result;
		}
		if (*size == 0)		return result;
	}

	//Handle hex
	if (**str == 'x')
	{	++str;
		--*size;
		int64_t	intVal				= 0;
		char*	end					= *str;
		double	dblVal				= _dal_str_to_num_hex(*str, &end, negative, &intVal);
		uint32_t	readed			= (uint32_t)(end - *str);
		*size						= (readed > *size) ? 0 : (*size - readed);
		*str						= end;
		result.dbl_valid			= TRUE;
		result.dbl_value			= dblVal;
		result.int_valid			= TRUE;
		result.int_value			= intVal;
		return result;
	}
	if (((*str)[0] == '0') && ((*str)[1] == 'x'))
	{	*str						+= 2;
		*size						-= 2;
		int64_t	intVal				= 0;
		char*	end					= *str;
		double	dblVal				= _dal_str_to_num_hex(*str, &end, negative, &intVal);
		uint32_t	readed			= (uint32_t)(end - *str);
		*size						= (readed > *size) ? 0 : (*size - readed);
		*str						= end;
		result.dbl_valid			= TRUE;
		result.dbl_value			= dblVal;
		result.int_valid			= TRUE;
		result.int_value			= intVal;
		return result;
	}

	uint64_t	mantissa;
	uint32_t	num_count		= 0;
	int64_t		exponent		= 0;

	if (**str == '0')
	{	//0 cannot be followed by an integer
		++*str;
		--*size;
		if (_dal_is_integer(**str))
		{
			return result;
		}
		mantissa		= 0;
	}else
	{
		if (!(_dal_is_integer(**str)))
		{	//must start with an integer
			return result;
		}

		unsigned char	digit	= **str - '0';
		mantissa				= digit;
		++*str;
		--*size;
		++num_count;

		while (_dal_is_integer(**str) && (*size > 0))
		{
			if (num_count < 20)
			{	digit			= **str - '0';
				mantissa		= (mantissa << 3) + (mantissa << 1) + digit;//10 * mantissa + digit;
				++num_count;
			}else	++exponent;
			++*str;
			--*size;
		}
	}

	if ((**str == '.') && (*size > 0))
	{
		++*str;
		--*size;
		if (*size == 0)			return result;

		if (_dal_is_integer(**str))
		{
			unsigned char	digit	= **str - '0';
			++*str;
			--*size;
			if (*size == 0)		return result;
			if (num_count < 20)
			{	mantissa			= (mantissa << 3) + (mantissa << 1) + digit;//mantissa * 10 + digit;
				--exponent;
				++num_count;
			}
		}else
		{
			return result;
		}

		while (_dal_is_integer(**str) && (*size > 0))
		{
			if (num_count < 20)
			{	unsigned char	digit	= **str - '0';
				mantissa				= (mantissa << 3) + (mantissa << 1) + digit;//mantissa * 10 + digit;
				--exponent;
				++num_count;
			}
			++*str;
			--*size;
		}
	}

	if ((('e' == **str) || ('E' == **str)) && (*size > 0))
	{
		++*str;
		--*size;
		if (*size == 0)		return result;

		bool_t	neg_exp		= FALSE;
		if ('-' == **str)
		{
			neg_exp			= TRUE;
			++*str;
			--*size;
		}else
			if ('+' == **str)
			{	++*str;
				--*size;
			}
		if (*size == 0)		return result;

		if (!_dal_is_integer(**str))
		{
			return result;
		}

		unsigned char	digit			= **str - '0';
		uint64_t		exp_number		= digit;
		++*str;
		--*size;

		if (_dal_is_integer(**str) && (*size > 0))
		{
			digit			= **str - '0';
			exp_number		= (exp_number << 3) + (exp_number << 1) + digit;//10 * exp_number + digit;
			++*str;
			--*size;
		}
		if (_dal_is_integer(**str) && (*size > 0))
		{
			digit			= **str - '0';
			exp_number		= (exp_number << 3) + (exp_number << 1) + digit;//10 * exp_number + digit;
			++*str;
			--*size;
		}

		while (_dal_is_integer(**str) && (*size > 0))
		{
			digit			= **str - '0';
			if (exp_number < 0x100000000)
			{
				exp_number	= (exp_number << 3) + (exp_number << 1) + digit;//10 * exp_number + digit;
			}
			++*str;
			--*size;
		}
		if (neg_exp)	exponent	-= exp_number;
		else			exponent	+= exp_number;
	}

	if (exponent < DAL_FLOAT_SMALLEST_POWER)
	{	result.dbl_value					= (negative) ? -0.0 : 0.0;	//Even though they're equivalent, I leave it for better understanding
		result.dbl_valid					= TRUE;
		return result;
	}
	if (exponent > DAL_FLOAT_LARGEST_POWER)
	{	if (negative)	result.dbl_value	= _dal_construct_double_ninf();
		else			result.dbl_value	= _dal_construct_double_pinf();
		result.dbl_valid					= TRUE;
		return result;
	}

	bool_t	success							= TRUE;
	result.dbl_value						= _dal_compute_float_64(exponent, mantissa, negative, &success);
	result.dbl_valid						= success;
	if (result.dbl_valid == FALSE)
	{
		result.dbl_value					= _dal_construct_double_nan();
	}

	if (exponent >= 0)
	{
		int32_t	int_expo					= num_count + (int32_t)exponent;
		if ((int_expo > 0) && (int_expo < 20))
		{
			if (exponent > 0) 	result.int_value	= mantissa * _dal_powoften_int[exponent];
			else				result.int_value	= mantissa / _dal_powoften_int[-exponent];
			if (negative)		result.int_value	*= -1;
			result.int_valid		= TRUE;
		}
	}

	return result;
}
