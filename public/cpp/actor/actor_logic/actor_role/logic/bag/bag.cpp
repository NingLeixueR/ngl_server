/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for bag.


#include "actor/actor_logic/actor_role/logic/bag/autoitem.h"
#include "actor/actor_logic/actor_role/logic/bag/bag.h"
#include "actor/actor_logic/actor_role/actor_role.h"
#include "tools/log/bi/bi_item.h"
#include "tools/log/nlog.h"

namespace ngl
{
	bag::bag()
		: m_autoitem(std::make_unique<autoitem>())
	{}

	void bag::initdata()
	{
		data_modified<pbdb::db_bag>& lbag = get();
		MODIFIED_RETUAN(lpdbbag, lbag, );
		auto* lpmap = lpdbbag->mutable_mitems();
		
		for (auto& lpair : *lpmap)
		{
			const int32_t ltid = lpair.second.mtid();
			const tab_item* ltab = ttab_item::instance().tab(ltid);
			if (ltab == nullptr)
			{
				continue;
			}
			if (ltab->m_isstack)
			{
				m_stackitems.emplace(ltid, &lpair.second);
			}
			else
			{
				m_nostackitems.emplace(ltid, &lpair.second);
			}
		}
	}

	pbdb::item* bag::add(pbdb::item& aitem)
	{
		data_modified<pbdb::db_bag>& ldb_bag = get();
		MODIFIED_RETUAN(lpdbbag, ldb_bag, nullptr);
		int32_t lindexid = lpdbbag->mmaxid();
		aitem.set_mid(++lindexid);
		
		auto [lit, lnew] = lpdbbag->mutable_mitems()->insert({ aitem.mid(), aitem });
		if (!lnew)
		{
			log_error()->print("add_item roleid=[{}] tid=[{}] mutable_mitems()->insert({}) == false", get_actor()->id_guid(), aitem.mtid(), aitem.mid());
			return nullptr;
		}
		lpdbbag->set_mmaxid(lindexid);

		print_bi(aitem.mid(), aitem.mtid(), aitem.mcount());
		return &lit->second;
	}

	void bag::print_bi(int32_t aitemid, int32_t atid, int32_t acount, bool aisadd /*= true*/)
	{
		bi_item ltempbi;
		ltempbi.m_serverid = nconfig.nodeid();
		ltempbi.m_itemid = aitemid;
		ltempbi.m_tid = atid;
		ltempbi.m_count = acount;
		ltempbi.m_roleid = get_actor()->id_guid();
		ltempbi.m_time = (int32_t)localtime::gettime();
		ltempbi.m_mask = local_get_remakes(nactor());
		ltempbi.m_adddec = aisadd ? 1 : 2;
		ltempbi.print();
	}

	bool bag::add_item(int32_t atid, int32_t acount)
	{
		std::vector<pbdb::item> lvec;
		if (item_create::create(nactor(), atid, acount, lvec) == false)
		{
			return false;
		}
		tab_item* tab = ttab_item::instance().tab(atid);
		if (tab == nullptr)
		{
			return false;
		}
		for (auto& item : lvec)
		{
			if (tab->m_isstack)
			{
				auto itor = m_stackitems.find(item.mtid());
				if (itor == m_stackitems.end())
				{
					pbdb::item* lpitem = add(item);
					if (lpitem == nullptr)
					{
						continue;
					}
					m_stackitems.emplace(item.mtid(), lpitem);
				}
				else
				{
					pbdb::item* lpitem = itor->second;
					lpitem->set_mcount(lpitem->mcount() + item.mcount());
					print_bi(lpitem->mid(), atid, item.mcount());
				}
				m_autoitem->add(atid, acount);
			}
			else
			{
				pbdb::item* lpitem = add(item);
				if (lpitem == nullptr)
				{
					continue;
				}
				m_nostackitems.emplace(item.mid(), lpitem);
				m_autoitem->add(item.mid());
			}
		}
		return true;
	}

	bool bag::add_item(const std::map<int32_t, int32_t>& amap)
	{
		for (const auto& [ltid, lcount] : amap)
		{
			add_item(ltid, lcount);
		}
		return true;
	}

	bool bag::add_item(std::vector<pbdb::item>& avec)
	{
		for (auto& litem : avec)
		{
			add(litem);
			m_autoitem->add(litem.mid());
		}
		return true;
	}

	// Delete item
	bool bag::dec_item(int32_t atid, int32_t acount)
	{
		auto itor = m_stackitems.find(atid);
		if (itor == m_stackitems.end())
		{
			return false;
		}
		int lcount = itor->second->mcount();
		lcount -= acount;
		if (acount < 0)
		{
			return false;
		}
		m_autoitem->del(atid, acount);
		int32_t litemid = itor->second->mid();
		if (acount == 0)
		{
			MODIFIED_RETUAN(lpdbbag, get(), false);
			lpdbbag->mutable_mitems()->erase(itor->second->mid());
			m_stackitems.erase(itor);
			return true;
		}
		itor->second->set_mcount(lcount);
		print_bi(litemid, atid, acount, false);
		return true;
	}

	// Delete item
	bool bag::dec_item(int32_t aid)
	{
		auto itor = m_nostackitems.find(aid);
		if (itor == m_nostackitems.end())
		{
			return false;
		}
		int32_t tid = itor->second->mtid();
		tab_item* tab = ttab_item::instance().tab(itor->second->mtid());
		if (tab == nullptr)
		{
			return false;
		}
		if (tab->m_isstack)
		{
			return false;
		}
		m_nostackitems.erase(itor);
		m_autoitem->del(aid);
		print_bi(aid, tid, 1, false);
		return true;
	}

	bool bag::checkbytid(int32_t atid, int32_t acount)
	{
		auto itor = m_stackitems.find(atid);
		if (itor == m_stackitems.end())
		{
			return false;
		}
		return acount >= itor->second->mcount();
	}

	bool bag::checkbyid(int32_t aid)
	{
		auto itor = m_nostackitems.find(aid);
		return itor != m_nostackitems.end();
	}

	void bag::sync_client()
	{
		if (m_autoitem->empty())
		{
			return;
		}
		pbnet::PROBUFF_NET_BAG_UPDATE pro;
		auto ladditems = pro.mutable_madditems();
		for (const auto& [lid, lcount] : m_autoitem->addstackitems())
		{
			auto ladditem = ladditems->Add();
			ladditem->set_mid(lid);
			ladditem->set_mcount(lcount);
		}
		auto laddnostackitems = pro.mutable_maddnostackitems();
		for (const int32_t litemid : m_autoitem->addnostackitems())
		{
			laddnostackitems->Add(litemid);
		}
		auto ldelitems = pro.mutable_mdelitems();
		for (const auto& [lid, lcount] : m_autoitem->delstackitems())
		{
			auto ldelitem = ldelitems->Add();
			ldelitem->set_mid(lid);
			ldelitem->set_mcount(lcount);
		}
		auto ldelnostackitems = pro.mutable_mdelnostackitems();
		for (const int32_t litemid : m_autoitem->delnostackitems())
		{
			ldelnostackitems->Add(litemid);
		}
		nactor()->send_client(get_actor()->id_guid(), pro);
		m_autoitem->clear();
	}
}// namespace ngl
