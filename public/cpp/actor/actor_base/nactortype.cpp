#include "nactortype.h"
#include "enum2name.h"

namespace ngl
{
	bool enum_actor::is_signle(ENUM_ACTOR aenum)
	{
		return aenum >= ACTOR_SIGNLE_START;
	}

	void nactortype::register_enumname(ENUM_ACTOR aenum, const char* aname)
	{
		em<ENUM_ACTOR>::set(aenum, aname);
	}

	bool nactortype::name2enum(const char* aname, ENUM_ACTOR& avalue)
	{
		auto lpair = em<ENUM_ACTOR>::get_enum(aname);
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