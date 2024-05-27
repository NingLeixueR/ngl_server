#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_servers : public manage_csv<tab_servers>
	{
		static std::map<int16_t, std::vector<tab_servers*>> m_areaofserver;

		ttab_servers()
		{}

		virtual void reload()
		{
			m_areaofserver.clear();
			for (std::pair<const int, tab_servers>& pair : tablecsv)
			{
				m_areaofserver[pair.second.m_area].push_back(&pair.second);
			}
		}

		static tab_servers* tab(i32_serverid aserverid)
		{
			ttab_servers* ttab = allcsv::get<ttab_servers>();
			if (ttab == nullptr)
				return nullptr;
			auto itor = ttab->tablecsv.find(aserverid);
			return &itor->second;
		}

		static tab_servers* tab()
		{
			return tab(nconfig::m_nodeid);
		}

		static tab_servers* tab(const std::string& aname,int area, int32_t atcount)
		{
			ttab_servers* ttab = allcsv::get<ttab_servers>();
			assert(ttab != nullptr);
			for (std::pair<const int, tab_servers>& item : ttab->tablecsv)
			{
				if (item.second.m_area == area && item.second.m_name == aname && item.second.m_tcount == atcount)
					return &item.second;
			}
			return nullptr;
		}

		static net_works const* nworks(ENET_PROTOCOL atype, tab_servers* atab)
		{
			for (auto& item : atab->m_net)
			{
				if (item.m_type == atype)
					return &item;
			}
			return nullptr;
		}

		static net_works const* nworks(ENET_PROTOCOL atype)
		{
			return nworks(atype, tab());
		}

		static net_works const* get_nworks(ENET_PROTOCOL atype)
		{
			//nconfig::m_nodeid
			tab_servers* ltab =  tab();
			if (ltab == nullptr)
				return nullptr;
			return nworks(atype, ltab);
		}

		static net_works const* get_nworks(const std::string& aname, int area, int32_t atcount, ENET_PROTOCOL atype)
		{
			tab_servers* ltab = tab(aname, area, atcount);
			if (ltab == nullptr)
				return nullptr;
			return nworks(atype, ltab);
		}

		static bool isefficient(ENET_PROTOCOL atype)
		{
			return atype == ENET_TCP || atype == ENET_WS;
		}
	private:
		static net_works const* connect(i32_serverid alocalserver, i32_serverid aotherserver)
		{
			assert(alocalserver != aotherserver);
			tab_servers* ltab1 = tab(alocalserver);
			tab_servers* ltab2 = tab(aotherserver);
			if (alocalserver > aotherserver)
			{
				std::swap(ltab1, ltab2);
			}
			for (net_works& item1 : ltab1->m_net)
			{
				if (isefficient(item1.m_type) == false)
					continue;
				for (net_works& item2 : ltab2->m_net)
				{
					if (item1.m_type == item2.m_type)
					{
						// 返回other的结构
						if (alocalserver > aotherserver)
						{
							return &item1;
						}
						else
						{
							return &item2;
						}
					}
				}
			}
			return nullptr;
		}
	public:
		static net_works const* connect(i32_serverid aserverid)
		{
			return connect(nconfig::m_nodeid, aserverid);
		}

		static NODE_TYPE node_type(i32_serverid aserverid)
		{
			tab_servers* ltab = tab(aserverid);
			assert(ltab != nullptr);
			return ltab->m_type;
		}

		static NODE_TYPE node_type()
		{
			return node_type(nconfig::m_nodeid);
		}

		static tab_servers* node_tnumber(NODE_TYPE atype, int32_t anumber)
		{
			ttab_servers* ttab = allcsv::get<ttab_servers>();
			assert(ttab != nullptr);
			for (std::pair<const int, tab_servers>& pair : ttab->tablecsv)
			{
				if (pair.second.m_type == atype && pair.second.m_tcount == anumber)
					return &pair.second;
			}
			return nullptr;
		}

		// 便利所有服务器
		static void foreach_server(const std::function<void(const tab_servers*)>& afun)
		{
			for (const auto& [_area, _vec] : m_areaofserver)
			{
				for (const tab_servers* iserver : _vec)
				{
					afun(iserver);
				}
			}
		}

		// 获取所有区服(负数区服是跨服区服需要转化为跨服内所有区服)
		static bool get_area(i16_area aarea, std::set<i16_area>& asetarea)
		{
			if (aarea < 0)
			{
				auto itor = m_areaofserver.find(aarea);
				if (itor == m_areaofserver.end())
					return false;
				if (itor->second.empty())
					return false;
				const tab_servers* ltab = *itor->second.begin();
				if (ltab == nullptr)
					return false;
				for (i32_serverid itemid : ltab->m_actorserver)
				{
					const tab_servers* tabactor = tab(itemid);
					if (ltab == nullptr)
						continue;
					asetarea.insert(tabactor->m_area);
				}
			}
			else
			{
				asetarea.insert(aarea);
			}
			return true;
		}

		// 服务器类型	atype
		// 区服			aarea（负数代表跨服,需要提供跨服内所有atype服务器）
		static bool foreach_server(NODE_TYPE atype, i16_area aarea, const std::function<void(const tab_servers*)>& afun)
		{
			std::set<i16_area> larea;
			if (get_area(aarea, larea) == false)
			{
				return false;
			}			

			for (const auto& [_area, _vec] : m_areaofserver)
			{
				if (larea.find(_area) == larea.end())
					continue;
				for (const tab_servers* iserver : _vec)
				{
					if (atype == FAIL)
					{
						afun(iserver);
					}
					else if (iserver->m_type == atype)
					{
						afun(iserver);
					}
				}
			}
			return true;
		}

		static bool get_server(NODE_TYPE atype, i16_area aarea, std::vector<i32_serverid>& avec)
		{
			return foreach_server(atype, aarea, [&avec](const tab_servers* iserver)
				{
					avec.push_back(iserver->m_id);
				});
		}

		static tab_servers* find_first(NODE_TYPE atype, const std::function<bool(tab_servers*)>& afun)
		{
			std::vector<tab_servers*>* litem = tools::findmap(m_areaofserver, tab()->m_area);
			if (litem == nullptr)
				return nullptr;
			for (tab_servers* iserver : *litem)
			{
				if (iserver->m_type == atype && afun(iserver))
				{
					return iserver;
				}
			}
			return nullptr;
		}
	};
}//namespace ngl

#define tab_self_area  ttab_servers::tab()->m_area