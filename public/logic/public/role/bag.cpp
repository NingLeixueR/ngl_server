#include "bag.h"
#include "actor_role.h"


namespace ngl
{
	pbdb::item* bag::add(pbdb::item& aitem)
	{
		int lid = get_constbag().m_maxid();
		pbdb::db_bag& lbag = get_bag();
		aitem.set_m_id(++lid);
		
		auto lpair = lbag.mutable_m_items()->insert({ aitem.m_id(), aitem });
		if (lpair.second == false)
		{
			LogLocalError("add_item roleid=[%] tid=[%] mutable_m_items()->insert(%) == false", actor()->id_guid(), aitem.m_tid(), aitem.m_id());
			return nullptr;
		}
		lbag.set_m_maxid(lid);
		return &lpair.first->second;
	}

	
}