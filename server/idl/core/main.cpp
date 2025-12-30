#include "idlcpp_protocol.h"
#include "idl.h"

#include <vector>

int main(int argc, char* argv[])
{
	if (argv[1] == std::string("idl"))
	{
		vector<const char*> lv = { "../compile_befor/idl/csvtable" };
		for (const char* item : lv)
		{
			idl::instance().analysis(string(item) + ".idl");
		}
		idlcppprotocol::instance()._h();
		idlcppprotocol::instance()._cpp();
		idlcppprotocol::instance()._cs();

		idlcppprotocol::instance()._ttab();

		idl::instance().data().clear();
		idl::instance().analysis("../compile_befor/idl/xmltable.idl");

	}
	else if (argv[1] == std::string("protocol"))
	{
		idl::instance(true);
		idl::instance().analysis("../../public/cpp/actor/protocol/nprotocol.h");
		idlcppprotocol::instance()._nprotocol_auto();
	}
	else if (argv[1] == std::string("actor"))
	{
		idl::instance(true);
		idl::instance().analysis("../../public/cpp/actor/actor_base/nactortype.h");
		idlcppprotocol::instance()._auto_actor_enum();
	}
	return 0;
}