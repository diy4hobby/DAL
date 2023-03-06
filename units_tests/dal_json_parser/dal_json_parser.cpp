#include "dal_json_parser.h"
#include "dal.h"
#include <fstream>
#include <vector>

using namespace std;

#define		TEST_DATA_PATH		"../../../test_data/"
#define		TEST_FILE_NAME		"complex.json"

dal_t* dal_alloc_node()
{
	return static_cast<dal_t*>(malloc(sizeof(dal_t)));
}

void* dal_alloc_data(uint32_t size)
{
	return malloc(size);
};

void	dal_free_node(dal_t* data)
{
	free(data);
};

void	dal_free_data(void* data)
{
	free(data);
};


int main()
{
	ifstream	inStream(TEST_DATA_PATH TEST_FILE_NAME);
	if (inStream.is_open() == false)
	{
		cout << "Test file not opened" << endl;
		return -1;
	}
	inStream.seekg(0, ios::end);
	uint32_t	length	= inStream.tellg();
	inStream.seekg(0, ios::beg);
	vector<char>	jsonStr(length);
	inStream.read(jsonStr.data(), length);
	inStream.close();

	dalMemHooks_t	memHooks	= { dal_alloc_node, dal_alloc_data, dal_free_node, dal_free_data };
	dal_init(&memHooks);

	dal_t*	obj		= dal_create();
	obj->from_json(reinterpret_cast<uint8_t*>(jsonStr.data()), jsonStr.size());

	return 0;
}
