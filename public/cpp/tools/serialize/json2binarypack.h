#pragma once

#include "tools.h"
#include "pack.h"


namespace ngl
{
	template <typename T>
	static bool jsontobinarypack(const char* ajson, std::shared_ptr<pack>& apack, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		T ltemp;
		if (tools::jsontopro(ajson, ltemp))
		{
			apack = actor_base::net_pack(ltemp, aactorid, arequestactorid);
			return true;
		}
		return false;
	}
}