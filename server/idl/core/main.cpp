#include "idl.h"
#include "idlcpp_protocol.h"
#include <vector>


int main(int argc, char* argv[])
{
	if (argv[1] == std::string("idl"))
	{
		vector<const char*> lv = { "../idl/csvtable" };
		for (const char* item : lv)
		{
			idl::getInstance().analysis(string(item) + ".idl");
		}
		idlcppprotocol::getInstance()._h();
		idlcppprotocol::getInstance()._cpp();
		idlcppprotocol::getInstance()._cs();

		idlcppprotocol::getInstance()._ttab();
	}
	else if (argv[1] == std::string("protocol"))
	{
		idl::getInstance(true);
		idl::getInstance().analysis("../../public/cpp/actor/nprotocol.h");
		idlcppprotocol::getInstance()._nprotocol_auto();
	}
	else if (argv[1] == std::string("actor"))
	{
		idl::getInstance(true);
		idl::getInstance().analysis("../../public/cpp/actor/actor_base/nactortype.h");
		idlcppprotocol::getInstance()._auto_actor_enum();
	}


	return 0;
}
