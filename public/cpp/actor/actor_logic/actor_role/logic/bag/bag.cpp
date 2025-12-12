/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_role.h"
#include "autoitem.h"
#include "bi_item.h"
#include "nlog.h"
#include "bag.h"

namespace ngl
{
	bag::bag()
		: m_autoitem(std::make_unique<autoitem>())
	{}

	void bag::initdata()
	{
		data_modified<pbdb::db_bag>& lbag = get();
		data_modified_return_getconst(lpdbbag, lbag);
		auto lpmap = lpdbbag->mitems();
		
		for (auto itor = lpmap.begin(); itor != lpmap.end(); ++itor)
		{
			int32_t tid = itor->second.mtid();
			tab_item* tab = ngl::allcsv::tab<tab_item>(tid);
			if (tab == nullptr)
			{
				continue;
			}
			if (tab->m_isstack)
			{
				m_stackitems.insert(std::make_pair(tid, &itor->second));
			}
			else
			{
				m_nostackitems.insert(std::make_pair(tid, &itor->second));
			}
		}
	}

	pbdb::item* bag::add(pbdb::item& aitem)
	{
		data_modified<pbdb::db_bag>& ldb_bag = get();
		data_modified_return_get(lpdbbag, ldb_bag, nullptr);
		int32_t lindexid = lpdbbag->mmaxid();
		aitem.set_mid(++lindexid);
		
		auto lpair = lpdbbag->mutable_mitems()->insert({ aitem.mid(), aitem });
		if (lpair.second == false)
		{
			log_error()->print("add_item roleid=[{}] tid=[{}] mutable_mitems()->insert({}) == false", get_actor()->id_guid(), aitem.mtid(), aitem.mid());
			return nullptr;
		}
		lpdbbag->set_mmaxid(lindexid);

		print_bi(aitem.mid(), aitem.mtid(), aitem.mcount());
		return &lpair.first->second;
	}

	void bag::print_bi(int32_t aitemid, int32_t atid, int32_t acount, bool aisadd /*= true*/)
	{
		bi_item ltempbi;
		ltempbi.m_serverid = nconfig::m_nodeid;
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
		tab_item* tab = ngl::allcsv::tab<tab_item>(atid);
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
					m_stackitems.insert({ item.mtid(), lpitem });
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
				m_nostackitems.insert({ item.mid(), lpitem });
				m_autoitem->add(item.mid());
			}
		}
		return true;
	}

	bool bag::add_item(const std::map<int32_t, int32_t>& amap)
	{
		for (const auto& [_tid, _count] : amap)
		{
			add_item(_tid, _count);
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

	// 只能删除可堆叠物品
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
			data_modified_return_get(lpdbbag, get(), false);
			lpdbbag->mutable_mitems()->erase(itor->second->mid());
			m_stackitems.erase(itor);
			return true;
		}
		itor->second->set_mcount(lcount);
		print_bi(litemid, atid, acount, false);
		return true;
	}

	// 删除不可堆叠物品
	bool bag::dec_item(int32_t aid)
	{
		auto itor = m_nostackitems.find(aid);
		if (itor == m_nostackitems.end())
		{
			return false;
		}
		int32_t tid = itor->second->mtid();
		tab_item* tab = ngl::allcsv::tab<tab_item>(itor->second->mtid());
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
		if (m_autoitem->m_addstackitems.empty()
			&& m_autoitem->m_addnostackitems.empty()
			&& m_autoitem->m_delstackitems.empty()
			&& m_autoitem->m_delnostackitems.empty()
			)
		{
			return;
		}
		pbnet::PROBUFF_NET_BAG_UPDATE pro;
		auto ladditems = pro.mutable_madditems();
		for (const auto& [_id, _count] : m_autoitem->m_addstackitems)
		{
			auto ladditem = ladditems->Add();
			ladditem->set_mid(_id);
			ladditem->set_mcount(_count);
		}
		auto laddnostackitems = pro.mutable_maddnostackitems();
		for (int32_t itemid : m_autoitem->m_addnostackitems)
		{
			laddnostackitems->Add(itemid);
		}
		auto ldelitems = pro.mutable_mdelitems();
		for (const auto& [_id, _count] : m_autoitem->m_delstackitems)
		{
			auto ldelitem = ldelitems->Add();
			ldelitem->set_mid(_id);
			ldelitem->set_mcount(_count);
		}
		auto ldelnostackitems = pro.mutable_mdelnostackitems();
		for (int32_t itemid : m_autoitem->m_delnostackitems)
		{
			ldelnostackitems->Add(itemid);
		}
		nactor()->send_client(get_actor()->id_guid(), pro);
	}
}// namespace ngl