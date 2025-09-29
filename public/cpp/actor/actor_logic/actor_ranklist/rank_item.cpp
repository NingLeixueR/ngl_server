#include "actor_ranklist.h"
#include "rank_item.h"


namespace ngl
{
	const pbdb::db_brief* rank_item::get()const
	{
		static tdb_brief::nsp_cread<actor_ranklist>* lpdb = nsp_instance<tdb_brief::nsp_cread<actor_ranklist>>::nclient(actor_ranklist::actorid());
		if (lpdb == nullptr)
		{
			tools::no_core_dump();
			return nullptr;
		}
		return lpdb->getconst(m_actorid);
	}
}//namespace ngl