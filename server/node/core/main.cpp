#include "server_main.h"
#include "dumper.h"
#include "type.h"

#include <format>
#include <iostream>

Dumper lDumper;

#define NUMARGS(...)  std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

int main(int argc, char** argv)
{
	std::cout << NUMARGS(1) << std::endl;
	std::cout << NUMARGS(1, 2) << std::endl;
	std::cout << NUMARGS(1, 2, 3) << std::endl;

	Dumper::m_excname = std::format("node_{}", argv[1]);
	return ngl_main(argc, argv);
}