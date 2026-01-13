#include "server_main.h"
#include "dumper.h"
#include "type.h"

#include <format>
#include <iostream>

Dumper lDumper;

#define NUMARGS(...)  std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value


#define abcccc(...) (1 __VA_OPT__(, )__VA_ARGS__)

int main(int argc, char** argv)
{
#if !defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL
	// Logic using the traditional preprocessor
#else
	// Logic using cross-platform compatible preprocessor
#endif

	std::cout << abcccc(3, 4, 5) << std::endl;
	std::cout << abcccc(3, 22) << std::endl;
	std::cout << abcccc(3) << std::endl;
	std::cout << abcccc() << std::endl;

	std::cout << NUMARGS(1) << std::endl;
	std::cout << NUMARGS(1, 2) << std::endl;
	std::cout << NUMARGS(1, 2, 3) << std::endl;

	Dumper::m_excname = std::format("node_{}", argv[1]);
	return ngl_main(argc, argv);
}