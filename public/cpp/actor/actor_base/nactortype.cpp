#include "nactortype.h"
#include "enum2name.h"

namespace ngl
{
	bool enum_actor::is_signle(ENUM_ACTOR aenum)
	{
		return aenum >= ACTOR_SIGNLE_START;
	}

	nactortype::nactortype()
	{
		em<ENUM_ACTOR>::set(em_pram(ACTOR_ROLE));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_ROBOT));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_GUILD));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_SIGNLE_START));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_ADDRESS_SERVER));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_ADDRESS_CLIENT));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_LOGIN));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_GATEWAY));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_CREATE));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_LOG));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_MANAGE_ROLE));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_MANAGE_ROBOT));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_MANAGE_GUILD));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_MANAGE_PLAYS));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_RELOADCSV));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_RELOADCSV_DISTRIBUTE));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_NOTICE));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_GM));
		em<ENUM_ACTOR>::set(em_pram(ACTOR_MAIL));

#define _DBNAME(A,B) ((ENUM_ACTOR)((int)A + (int)B))

		em<ENUM_ACTOR>::set(em_pram(_DBNAME(ACTOR_DB, pbdb::ENUM_DB_ACCOUNT)));
		em<ENUM_ACTOR>::set(em_pram(_DBNAME(ACTOR_DB, pbdb::ENUM_DB_ROLE)));
		em<ENUM_ACTOR>::set(em_pram(_DBNAME(ACTOR_DB, pbdb::ENUM_DB_BAG)));
		em<ENUM_ACTOR>::set(em_pram(_DBNAME(ACTOR_DB, pbdb::ENUM_DB_KEYVALUE)));
		em<ENUM_ACTOR>::set(em_pram(_DBNAME(ACTOR_DB, pbdb::ENUM_DB_MAIL)));
		em<ENUM_ACTOR>::set(em_pram(_DBNAME(ACTOR_DB, pbdb::ENUM_DB_GUILD)));
		em<ENUM_ACTOR>::set(em_pram(_DBNAME(ACTOR_DB, pbdb::ENUM_DB_NOTICE)));
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
}