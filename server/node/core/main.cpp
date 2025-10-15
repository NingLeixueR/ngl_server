#include "server_main.h"
#include "type.h"
#include <map>

int main(int argc, char** argv)
{
	int16_t lnumb = 1;
	std::map<int32_t, bool> lmap = { {1,true},{2,true},{3,true} };
	auto itor = lmap.find(lnumb);

	return ngl_main(argc, argv);
}