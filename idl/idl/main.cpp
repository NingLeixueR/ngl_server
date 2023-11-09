#include "idl.h"
#include "idlcpp_protocol.h"
#include <vector>


int main(int argc, char* argv[])
{
	vector<const char*> lv = { /*"game_gm", "game_enum","game_shared","game_logic", "game_db", */"csvtable"/*, "game_worldgame"*/};
	for (const char* item : lv)
		idl::getInstance().analysis(string(item) + ".idl");
	//
	idlcppprotocol::getInstance()._h();
	idlcppprotocol::getInstance()._cpp();

	//Struct::init_sql();
	//Struct::init_tab();
	return 0;
}
