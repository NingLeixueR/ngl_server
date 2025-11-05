#include "server_main.h"
#include "dumper.h"
#include "type.h"

#include <format>
#include <map>

Dumper lDumper;

int main(int argc, char** argv)
{
	Dumper::m_excname = std::format("node_{}", argv[1]);
	return ngl_main(argc, argv);
}