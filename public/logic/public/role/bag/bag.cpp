#include "bag.h"
#include "actor_role.h"


namespace ngl
{
	bag::bag()
		: bag_db_modular() 
	{}

	pbdb::db_bag& bag::get_bag()
	{
		return db()->get();
	}

	const pbdb::db_bag& bag::get_constbag()
	{
		return db()->getconst();
	}

	void bag::initdata()
	{
		pbdb::db_bag& lbag = db()->get(false);
		auto lpmap = lbag.mutable_m_items();

		for (auto itor = lpmap->begin(); itor != lpmap->end(); ++itor)
		{
			int32_t tid = itor->second.m_tid();
			tab_item* tab = ngl::allcsv::tab<tab_item>(tid);
			if (tab == nullptr)
				continue;
			if (tab->m_isstack)
				m_stackitems.insert(std::make_pair(tid, &itor->second));
			else
				m_nostackitems.insert(std::make_pair(tid, &itor->second));
		}
	}

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

	bool bag::add_item(int32_t atid, int32_t acount)
	{
		std::vector<pbdb::item> lvec;
		if (item_create::create(actor(), atid, acount, lvec) == false)
			return false;
		int lid = get_constbag().m_maxid();
		tab_item* tab = ngl::allcsv::tab<tab_item>(atid);
		if (tab == nullptr)
			return false;
		for (auto& item : lvec)
		{
			if (tab->m_isstack)
			{
				auto itor = m_stackitems.find(item.m_tid());
				if (itor == m_stackitems.end())
				{
					pbdb::item* lpitem = add(item);
					if (lpitem == nullptr)
					{
						continue;
					}
					m_stackitems.insert({ item.m_tid(), lpitem });
				}
				else
				{
					pbdb::item* lpitem = itor->second;
					lpitem->set_m_count(lpitem->m_count() + item.m_count());
				}
			}
			else
			{
				pbdb::item* lpitem = add(item);
				if (lpitem == nullptr)
				{
					continue;
				}
				m_nostackitems.insert({ item.m_id(), lpitem });
			}
		}
		get_bag().set_m_maxid(lid);
		LogBI("%|%|%|%", actorbase()->id_guid(), g_remakes(actor()), atid, acount);
		return true;
	}

	bool bag::add_item(const std::map<int32_t, int32_t>& amap)
	{
		for (const auto& [_tid, _count] : amap)
			add_item(_tid, _count);
		return true;
	}

	bool bag::add_item(std::vector<pbdb::item>& avec)
	{
		for (auto& litem : avec)
			add(litem);
		return true;
	}

	// 只能删除可堆叠物品
	bool bag::dec_item(int32_t atid, int32_t acount)
	{
		auto itor = m_stackitems.find(atid);
		if (itor == m_stackitems.end())
			return false;
		int lcount = itor->second->m_count();
		lcount -= acount;
		if (acount < 0)
			return false;
		if (acount == 0)
		{
			get_bag().mutable_m_items()->erase(itor->second->m_id());
			m_stackitems.erase(itor);
			return true;
		}
		itor->second->set_m_count(lcount);
		return true;
	}

	// 删除不可堆叠物品
	bool bag::dec_item(int32_t aid)
	{
		auto itor = m_nostackitems.find(aid);
		if (itor == m_nostackitems.end())
			return false;
		tab_item* tab = ngl::allcsv::tab<tab_item>(itor->second->m_tid());
		if (tab == nullptr)
			return false;
		if (tab->m_isstack)
			return false;
		m_nostackitems.erase(itor);
		return true;
	}

	bool bag::checkbytid(int32_t atid, int32_t acount)
	{
		auto itor = m_stackitems.find(atid);
		if (itor == m_stackitems.end())
			return false;
		return acount >= itor->second->m_count();
	}

	bool bag::checkbyid(int32_t aid)
	{
		auto itor = m_nostackitems.find(aid);
		return itor != m_nostackitems.end();
	}
}