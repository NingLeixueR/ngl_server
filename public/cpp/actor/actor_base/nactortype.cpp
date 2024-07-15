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
		ENUM_ACTOR lactortype = em<ENUM_ACTOR>::get_enum(aname);
		if (lactortype == em<ENUM_ACTOR>::enum_null())
			return false;
		avalue = lactortype;
		return true;
	}

	const char* nactortype::enum2name(ENUM_ACTOR aenum)
	{
		const char* lactorname = em<ENUM_ACTOR>::get_name(aenum);
		if (lactorname == nullptr)
			return "nullptr";
		return lactorname;
	}	
}//namespace ngl