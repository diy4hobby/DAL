#include "dal_num_to_string.h"
#include "dal_num_powers.h"

#define		DAL_FRAC_MASK		0x000FFFFFFFFFFFFFU
#define		DAL_EXP_MASK		0x7FF0000000000000U
#define		DAL_HIDDEN_BIT		0x0010000000000000U
#define		DAL_SIGN_MASK		0x8000000000000000U
#define		DAL_EXP_BIAS		(1023 + 52)

#define		dal_abs(n)			((n) < 0 ? -(n) : (n))
#define		dal_min(a, b)		((a) < (b) ? (a) : (b))

static uint64_t tens[] =
{
	10000000000000000000U, 1000000000000000000U, 100000000000000000U,
	10000000000000000U, 1000000000000000U, 100000000000000U,
	10000000000000U, 1000000000000U, 100000000000U,
	10000000000U, 1000000000U, 100000000U,
	10000000U, 1000000U, 100000U,
	10000U, 1000U, 100U,
	10U, 1U
};

static uint64_t	_dal_double_get_bits(double d)
{
	union
	{
		double		dbl;
		uint64_t	i;
	}dbl_bits	= { d };
	return dbl_bits.i;
}

static dalFloat_t _dal_construct_dalFloat(double d)
{
	uint64_t	bits	= _dal_double_get_bits(d);
	dalFloat_t	fp;
	fp.frac				= bits & DAL_FRAC_MASK;
	fp.exp				= (bits & DAL_EXP_MASK) >> 52;

	if (fp.exp)
	{	fp.frac			+= DAL_HIDDEN_BIT;
		fp.exp			-= DAL_EXP_BIAS;
	}else
	{	fp.exp			= -DAL_EXP_BIAS + 1;
	}
	return fp;
}

static void _dal_normalize_dalFloat(dalFloat_t* fp)
{
	while ((fp->frac & DAL_HIDDEN_BIT) == 0)
	{	fp->frac		<<= 1;
		fp->exp--;
	}

	uint32_t	shift	= 64 - 52 - 1;
	fp->frac			<<= shift;
	fp->exp				-= shift;
}

static void _dal_gen_norm_boundaries(dalFloat_t* fp, dalFloat_t* lower, dalFloat_t* upper)
{
	upper->frac			= (fp->frac << 1) + 1;
	upper->exp			= fp->exp - 1;

	while ((upper->frac & (DAL_HIDDEN_BIT << 1)) == 0)
	{	upper->frac <<= 1;
		upper->exp--;
	}

	uint32_t	u_shift	= 64 - 52 - 2;
	upper->frac			<<= u_shift;
	upper->exp			-= u_shift;

	uint32_t	l_shift	= (fp->frac == DAL_HIDDEN_BIT) ? 2 : 1;
	lower->frac			= (fp->frac << l_shift) - 1;
	lower->exp			= fp->exp - l_shift;
	lower->frac			<<= (int64_t)lower->exp - (int64_t)upper->exp;
	lower->exp			= upper->exp;
}

static dalFloat_t _dal_mult_dalFloat(dalFloat_t* a, dalFloat_t* b)
{
	const uint64_t	lomask		= 0x00000000FFFFFFFF;

	uint64_t		ah_bl		= (a->frac >> 32)	* (b->frac & lomask);
	uint64_t		al_bh		= (a->frac & lomask) * (b->frac >> 32);
	uint64_t		al_bl		= (a->frac & lomask) * (b->frac & lomask);
	uint64_t		ah_bh		= (a->frac >> 32)	* (b->frac >> 32);

	uint64_t		tmp			= (ah_bl & lomask) + (al_bh & lomask) + (al_bl >> 32);
	/* round up */
	tmp							+= 1U << 31;

	dalFloat_t		fp			= {	ah_bh + (ah_bl >> 32) + (al_bh >> 32) + (tmp >> 32),
									a->exp + b->exp + 64 };
	return fp;
}

static void _dal_round_digits(char* digits, uint32_t ndigits, uint64_t delta, uint64_t rem, uint64_t kappa, uint64_t frac)
{
	while (rem < frac && delta - rem >= kappa &&
		   (rem + kappa < frac || frac - rem > rem + kappa - frac))
	{
		digits[ndigits - 1]--;
		rem						+= kappa;
	}
}

static int32_t _dal_generate_digits(dalFloat_t* fp, dalFloat_t* upper, dalFloat_t* lower, char* digits, int32_t* K)
{
	uint64_t	wfrac			= upper->frac - fp->frac;
	uint64_t	delta			= upper->frac - lower->frac;

	dalFloat_t	one;
	one.frac					= 1ULL << -upper->exp;
	one.exp						= upper->exp;

	uint64_t	part1			= upper->frac >> -one.exp;
	uint64_t	part2			= upper->frac & (one.frac - 1);

	uint32_t	idx = 0,	kappa = 10;
	uint64_t*	divp;
	/* 1000000000 */
	for(divp = tens + 10; kappa > 0; divp++)
	{
		uint64_t	div			= *divp;
		uint64_t	digit		= part1 / div;

		if (digit || idx)
		{
			digits[idx++]		= (char)digit + '0';
		}

		part1					-= digit * div;
		kappa--;

		uint64_t	tmp			= (part1 <<-one.exp) + part2;
		if (tmp <= delta)
		{
			*K					+= kappa;
			_dal_round_digits(digits, idx, delta, tmp, div << -one.exp, wfrac);
			return idx;
		}
	}

	/* 10 */
	uint64_t*	unit			= tens + 18;

	while(TRUE)
	{
		part2					*= 10;
		delta					*= 10;
		kappa--;
		uint64_t	digit		= part2 >> -one.exp;

		if (digit || idx)
		{
			digits[idx++]		= (char)digit + '0';
		}

		part2 &= one.frac - 1;

		if (part2 < delta)
		{
			*K					+= kappa;
			_dal_round_digits(digits, idx, delta, part2, one.frac, wfrac * *unit);
			return idx;
		}

		unit--;
	}
}

static uint32_t _dal_grisu2(double d, char* digits, int32_t* K)
{
	dalFloat_t	w				= _dal_construct_dalFloat(d);

	dalFloat_t	lower, upper;
	_dal_gen_norm_boundaries(&w, &lower, &upper);
	_dal_normalize_dalFloat(&w);

	int32_t		k;
	dalFloat_t cp				= _dal_find_pow10(upper.exp, &k);

	w							= _dal_mult_dalFloat(&w, &cp);
	upper						= _dal_mult_dalFloat(&upper, &cp);
	lower						= _dal_mult_dalFloat(&lower, &cp);
	lower.frac++;
	upper.frac--;
	*K							= -k;
	return _dal_generate_digits(&w, &upper, &lower, digits, K);
}

static uint32_t _dal_emit_digits(char* digits, int32_t ndigits, char* dest, int32_t K, bool_t neg)
{
	uint64_t	exp				= dal_abs(K + ndigits - 1);

	//write plain integer
	if (K >= 0 && (exp < (ndigits + 7)))
	{
		uint32_t	result		= ndigits + K;
		while (ndigits > 0)
		{	*dest++				= *digits++;
			ndigits--;
		}
		while (K > 0)
		{	*dest++				= '0';
			K--;
		}
		return result;
	}

	//write decimal w/o scientific notation
	if (K < 0 && (K > -7 || exp < 4))
	{
		int32_t	offset			= ndigits - dal_abs(K);
		//fp < 1.0 -> write leading zero
		if (offset <= 0)
		{
			offset				= -offset;
			*dest++				= '0';
			*dest++				= '.';
			uint32_t	result	= ndigits + 2 + offset;
			while (offset > 0)
			{	*dest++			= '0';
				offset--;
			}
			while (ndigits > 0)
			{	*dest++				= *digits++;
				ndigits--;
			}
			return result;
		}else	//fp > 1.0
		{
			uint32_t	result	= ndigits + 1;
			while (offset > 0)
			{	*dest++			= *digits++;
				offset--;
				ndigits--;
			}
			*dest++				= '.';
			while (ndigits > 0)
			{	*dest++			= *digits++;
				ndigits--;
			}
			return result;
		}
	}//if (K < 0 && (K > -7 || exp < 4))

	//write decimal w/ scientific notation
	int32_t	temp				= (neg == TRUE) ? 17 : 18;
	ndigits						= dal_min(ndigits, temp);
	char*	beg					= dest;
	*dest++						= digits[0];

	if(ndigits > 1)
	{
		*dest++					= '.';
		uint32_t	temp		= ndigits - 1;
		while (temp > 0)
		{	*dest++				= *digits++;
			temp--;
		}
	}

	*dest++						= 'e';
	char	sign				= ((K + ndigits - 1) < 0) ? '-' : '+';
	*dest++						= sign;

	uint64_t		cent		= 0;

	if(exp > 99)
	{	cent					= exp / 100;
		*dest++					= (uint8_t)cent + '0';
		exp						-= cent * 100;
	}
	if(exp > 9)
	{
		uint64_t	dec			= exp / 10;
		*dest++					= (uint8_t)dec + '0';
		exp						-= dec * 10;
	}else
		if(cent)
		{
			*dest++				= '0';
		}

	*dest++						= exp % 10 + '0';

	return (uint8_t)(dest - beg);
}

static int _dal_filter_spec_values(double fp, char* dest)
{
	if(fp == 0.0)
	{	*dest					= '0';
		return 1;
	}

	uint64_t	bits			= _dal_double_get_bits(fp);

	bool_t		nan				= (bits & DAL_EXP_MASK) == DAL_EXP_MASK;

	if(!nan)
	{	return 0;
	}

	if(bits & DAL_FRAC_MASK)
	{
		*dest++ = 'n'; *dest++ = 'a'; *dest++ = 'n';
	}else
	{
		*dest++ = 'i'; *dest++ = 'n'; *dest++ = 'f';
	}
	return 3;
}

void	dal_dtoa(double d, char* dest, uint32_t* len)
{
	char		digits[18];

	uint32_t	str_len			= 0;
	bool_t		neg				= FALSE;

	if(_dal_double_get_bits(d) & DAL_SIGN_MASK)
	{	*dest++					= '-';
		str_len++;
		neg						= TRUE;
	}

	uint32_t	spec			= _dal_filter_spec_values(d, dest);

	if(spec)
	{
		if (len != NULL)		*len	= str_len + spec;
		return;
	}

	int32_t		K				= 0;
	uint32_t	ndigits			= _dal_grisu2(d, digits, &K);

	str_len						+= _dal_emit_digits(digits, ndigits, dest, K, neg);
	if (len != NULL)			*len	= str_len;
	return;
}
