#pragma once

#include "tools.h"
#include "type.h"

#include <string>

namespace ngl
{
	enum eprotocol {};

	class protocoltools
	{
		protocoltools() = delete;
		protocoltools(const protocoltools&) = delete;
		protocoltools& operator=(const protocoltools&) = delete;
	public:
		static bool names(
			char* abuff, 
			int abuffsize, 
			const char* aname, 
			i32_protocolnum aprotocolnumber, 
			EPROTOCOL_TYPE atype
		);

		static void push(
			i32_protocolnum aprotocolnum, 
			const std::string& aprotocolname, 
			EPROTOCOL_TYPE atype
		);

		static const char* name(i32_protocolnum aprotocolnum, EPROTOCOL_TYPE atype);
	};
}// namespace ngl