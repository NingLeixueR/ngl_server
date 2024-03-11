#pragma once

#include "define.h"
#include "type.h"

#include <string>

namespace ngl
{
	enum eprotocol {};

	class protocoltools
	{
	public:
		static char*		hex_str(char* abuff, int anumber);
		static bool			names(char* abuff, int abuffsize, const char* aname, i32_protocolnum aprotocolnumber, EPROTOCOL_TYPE atype);
		static void			push(i32_protocolnum aprotocolnum, const std::string& aprotocolname, EPROTOCOL_TYPE atype);
		static const char*	name(i32_protocolnum aprotocolnum, EPROTOCOL_TYPE atype);
	};
}// namespace ngl