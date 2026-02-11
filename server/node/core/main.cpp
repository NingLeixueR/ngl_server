#include "server_main.h"
#include "dumper.h"
#include "type.h"

#include <iostream>
#include <format>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

Dumper lDumper;

int main(int argc, char** argv) 
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif
	if (argc >= 3)
	{
		Dumper::m_excname = std::format("node_{}", argv[1]);
		lDumper.setDumperHandler(dump_logic("core dump"));
		return ngl_main(argc, argv);
	}
	std::cout << "参数错误:EXE name areaid tab_servers::tcount" << std::endl;
	return 0;
}