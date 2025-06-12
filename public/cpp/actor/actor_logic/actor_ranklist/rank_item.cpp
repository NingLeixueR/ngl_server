#include "actor_ranklist.h"
#include "rank_item.h"


namespace ngl
{
	const pbdb::db_brief* rank_item::get()const
	{
		return tdb_brief::nsp_cli<actor_ranklist>::instance(actor_ranklist::actorid()).getconst(m_actorid);
	}
}//namespace ngl