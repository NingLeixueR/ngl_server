#include "nactortype.h"
#include "enum2name.h"

namespace ngl
{
	bool enum_actor::is_signle(ENUM_ACTOR aenum)
	{
		return aenum >= ACTOR_SIGNLE_START;
	}

	bool nactortype::name2enum(const std::string& aenumname, ENUM_ACTOR& avalue)
	{
		auto lpair = em<ENUM_ACTOR>::get_enum(aenumname.c_str());
		if (lpair.second == false)
			return false;
		avalue = lpair.first;
		return true;
	}

	const char* nactortype::enum2name(ENUM_ACTOR aenum)
	{
		auto lpair = em<ENUM_ACTOR>::get_name(aenum);
		if (lpair.second == false)
			return "nullptr";
		return lpair.first;
	}	
}//namespace ngl