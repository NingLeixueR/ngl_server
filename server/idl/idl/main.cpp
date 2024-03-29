#include "idl.h"
#include "idlcpp_protocol.h"
#include <vector>


int main(int argc, char* argv[])
{
	vector<const char*> lv = { "csvtable"};
	for (const char* item : lv)
		idl::getInstance().analysis(string(item) + ".idl");
	//
	idlcppprotocol::getInstance()._h();
	idlcppprotocol::getInstance()._cpp();
	idlcppprotocol::getInstance()._cs();
	//Struct::init_sql();
	//Struct::init_tab();
	return 0;
}
