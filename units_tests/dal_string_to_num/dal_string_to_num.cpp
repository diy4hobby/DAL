#include "dal_string_to_num.h"
#include "dal_ser_deser/dal_string_to_num/dal_string_to_num.h"

using namespace std;

void convert_and_print(const char* str)
{
	char*		strVal		= const_cast<char*>(str);
	uint32_t	len			= strlen(str);
	dalStrtonumResult_t		strtonum;
	strtonum				= dal_strntonum(&strVal, &len);

	cout << "String:	" << str << endl;
	cout << "As double		valid: " << ((strtonum.dbl_valid == 0) ? "false" : "true") << "		value: " << strtonum.dbl_value << endl;
	cout << "As integer		valid: " << ((strtonum.int_valid == 0) ? "false" : "true") << "		value: " << strtonum.int_value << endl;
	cout << endl;
}


int main()
{
	cout.precision(20);

	convert_and_print("123.456789");
	convert_and_print("1.0E+2");
	convert_and_print("1.0E-20");
	convert_and_print("-1.0E-20");
	convert_and_print("10.56E+10");
	convert_and_print("12345.0E-2000");
	convert_and_print("0.12345678910111213141516171819");
	convert_and_print("0x0123456A");
	convert_and_print("10.56E+38");
	convert_and_print("12345678910111213141516171819202122232425");
	convert_and_print("-12345678910111213141516171819202122232425");
	convert_and_print("1234567891011121314151617181920.30405060708090100110120130140");
	convert_and_print("1234567891011121314");
	convert_and_print("12345678910111213145.30405060708090100110120130140");
	convert_and_print("123456789101112131456.304050");
	convert_and_print("52.609588623046875");

	return 0;
}
