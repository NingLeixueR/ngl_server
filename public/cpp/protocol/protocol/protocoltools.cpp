#include "protocoltools.h"

#include "enum2name.h"
#include "nlog.h"

namespace ngl
{
	char* protocoltools::hex_str(char* abuff, int anumber)
	{
		//itoa(anumber, &abuff[2], 16);
		//abuff[0] = '0';
		//abuff[1] = 'x';
		std::ostringstream ss;
		ss << std::hex << anumber;
		std::string str = ss.str();
		memcpy(abuff, str.c_str(), str.size());
		return abuff;
	}

	bool protocoltools::names(char* abuff, int abuffsize, const char* aname, i32_protocolnum aprotocolnumber, EPROTOCOL_TYPE atype)
	{
		return snprintf(abuff, 1024, "%d:%s:%d", atype, aname, aprotocolnumber) > 0;
	}

	void protocoltools::push(i32_protocolnum aprotocolnum, const std::string& aprotocolname, EPROTOCOL_TYPE atype)
	{
		char lbuff[1024] = { 0x0 };
		if (names(lbuff, 1024, aprotocolname.c_str(), aprotocolnum, atype))
		{
			LogLocalError("protocol_push [%] ", lbuff)
			em<eprotocol>::set((eprotocol)aprotocolnum, lbuff, atype);
		}
		else
			LogLocalError("protocol_push fail [%] ", lbuff)
	}

	const char* protocoltools::name(i32_protocolnum aprotocolnum, EPROTOCOL_TYPE atype)
	{
		std::pair<const char*, bool> lpair = em<eprotocol>::get_name((eprotocol)(aprotocolnum), atype);
		if (lpair.second == false)
			return "[not find protocol name]";
		return lpair.first;
	}
}
