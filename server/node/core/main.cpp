// File overview: Implements the entry point for the node target.

#include "server_main.h"
#include "dumper.h"
#include "tools/tools.h"
#include "tools/type.h"

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
	if (argc >= 4)
	{
		Dumper::m_excname = std::format("node_{}", argv[1]);
		lDumper.setDumperHandler(ngl::tools::send_mail("core dump"));
		return ngl_main(argc, argv);
	}
	std::cout << "Argument error: EXE name areaid tab_servers::tcount" << std::endl;
	return static_cast<int>(startup_error::invalid_args);
}
