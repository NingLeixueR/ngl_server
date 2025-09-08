#pragma once

#include "ttab_mergearea.h"
#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_servers :
		public manage_csv<tab_servers>
	{
		ttab_servers(const ttab_servers&) = delete;
		ttab_servers& operator=(const ttab_servers&) = delete;

		std::map<i16_area, std::map<i32_serverid, const tab_servers*>> m_areaserver;
		std::map<i16_area, std::set<i16_area>> m_coressserver;// key:应该是小于0的跨服 value:跨服对应的区服

		ttab_servers()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_servers] reload" << std::endl;
			m_areaserver.clear();
			for (const auto& item : tablecsv())
			{
				m_areaserver[item.second.m_area][item.first] = &item.second;

				if (item.second.m_area < 0)
				{
					std::set<i16_area> lsetarea;
					for (i32_serverid actorserverid : item.second.m_actorserver)
					{
						const tab_servers* ltab = tab(actorserverid);
						if (ltab == nullptr)
						{
							continue;
						}
						lsetarea.insert(ltab->m_area);
					}

					for (i16_area area : lsetarea)
					{
						i16_area mergearea = ttab_mergearea::instance().mergeid(area);
						if (mergearea == nguid::none_area())
						{
							continue;
						}
						std::set<i16_area>* mergeareaset = ttab_mergearea::instance().mergelist(mergearea);
						if (mergeareaset == nullptr)
						{
							continue;
						}
						m_coressserver[item.second.m_area].insert(mergeareaset->begin(), mergeareaset->end());
					}
				}
			}
		}

	public:
		using type_tab = tab_servers;

		static ttab_servers& instance()
		{
			static ttab_servers ltemp;
			return ltemp;
		}

		const std::map<int, tab_servers>& tablecsv()
		{
			ttab_servers* ttab = allcsv::get<ttab_servers>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_servers* tab(int32_t aid)
		{
			auto itor = tablecsv().find(aid);
			if (itor == tablecsv().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		const tab_servers* tab()
		{
			return tab(nconfig::m_nodeid);
		}

		const tab_servers* tab(const std::string& aname, int area, int32_t atcount)
		{
			i16_area larea = ttab_mergearea::instance().mergeid(area);
			if (larea != nguid::none_area())
			{
				area = larea;
			}
			for (const std::pair<const int, tab_servers>& item : tablecsv())
			{
				std::cout << std::format(
					"m_area:[{}] m_name:[{}] m_tcount:[{}]"
					, item.second.m_area
					, item.second.m_name
					, item.second.m_tcount
				) << std::endl;
				if (item.second.m_area == area && item.second.m_name == aname && item.second.m_tcount == atcount)
				{
					return &item.second;
				}
			}
			return nullptr;
		}

		const net_works* nworks(ENET_PROTOCOL atype, const tab_servers* atab)
		{
			for (auto& item : atab->m_net)
			{
				if (item.m_type == atype)
				{
					return &item;
				}
			}
			return nullptr;
		}

		const net_works* nworks(ENET_PROTOCOL atype)
		{
			return nworks(atype, tab());
		}

		const net_works* get_nworks(ENET_PROTOCOL atype)
		{
			//nconfig::m_nodeid
			const tab_servers* ltab = tab();
			if (ltab == nullptr)
			{
				return nullptr;
			}
			return nworks(atype, ltab);
		}

		const net_works* get_nworks(const std::string& aname, int area, int32_t atcount, ENET_PROTOCOL atype)
		{
			const tab_servers* ltab = tab(aname, area, atcount);
			if (ltab == nullptr)
			{
				return nullptr;
			}
			return nworks(atype, ltab);
		}

		bool isefficient(ENET_PROTOCOL atype)
		{
			return atype == ENET_TCP || atype == ENET_WS;
		}
	private:
		const net_works* connect(i32_serverid alocalserver, i32_serverid aotherserver)
		{
			tools::no_core_dump(alocalserver != aotherserver);
			const tab_servers* ltab1 = tab(alocalserver);
			const tab_servers* ltab2 = tab(aotherserver);
			if (alocalserver > aotherserver)
			{
				std::swap(ltab1, ltab2);
			}
			for (const net_works& item1 : ltab1->m_net)
			{
				if (isefficient(item1.m_type) == false)
				{
					continue;
				}
				for (const net_works& item2 : ltab2->m_net)
				{
					if (item1.m_type == item2.m_type)
					{
						// 返回other的结构
						return alocalserver > aotherserver ? &item1 : &item2;
					}
				}
			}
			return nullptr;
		}
	public:
		net_works const* connect(i32_serverid aserverid)
		{
			return connect(nconfig::m_nodeid, aserverid);
		}

		NODE_TYPE node_type(i32_serverid aserverid)
		{
			const tab_servers* ltab = tab(aserverid);
			if (ltab == nullptr)
			{
				tools::no_core_dump();
				return FAIL;
			}
			return ltab->m_type;
		}

		NODE_TYPE node_type()
		{
			return node_type(nconfig::m_nodeid);
		}

		const tab_servers* node_tnumber(NODE_TYPE atype, int32_t anumber)
		{
			for (const std::pair<const int, tab_servers>& pair : ttab_servers::tablecsv())
			{
				if (pair.second.m_type == atype && pair.second.m_tcount == anumber)
				{
					return &pair.second;
				}
			}
			return nullptr;
		}

		// 便利所有服务器
		void foreach_server(const std::function<void(const tab_servers*)>& afun)
		{
			ttab_mergearea::instance().foreach([&afun](i16_area aarea, std::set<i16_area>& aset)
				{
					
					auto lpmap = tools::findmap(ttab_servers::instance().m_areaserver, aarea);
					if (lpmap == nullptr)
					{
						return;
					}
					for (const auto& item : *lpmap)
					{
						afun(item.second);
					}
				});
		}

		// 获取所有区服(负数区服是跨服区服需要转化为跨服内所有区服)
		const std::set<i16_area>* get_area(i16_area aarea)
		{
			if (aarea > 0)
			{
				return ttab_mergearea::instance().mergelist(aarea);
			}
			else
			{
				return tools::findmap(m_coressserver, aarea);				
			}
		}

		// 服务器类型	atype
		// 区服			aarea（负数代表跨服,需要提供跨服内所有atype服务器）
		bool foreach_server(NODE_TYPE atype, i16_area aarea, const std::function<void(const tab_servers*)>& afun)
		{
			const std::set<i16_area>* larea = get_area(aarea);
			if (larea == nullptr)
			{
				return false;
			}
			for (i16_area area : *larea)
			{
				auto lmap = tools::findmap(m_areaserver, area);
				if (lmap == nullptr)
				{
					continue;
				}
				for (auto [_serverid, _servertab] : *lmap)
				{
					if (atype == _servertab->m_type)
					{
						afun(_servertab);
					}
				}
			}
			return true;
		}

		bool get_server(NODE_TYPE atype, i16_area aarea, std::set<i32_serverid>& aset)
		{
			return foreach_server(atype, aarea, [&aset](const tab_servers* iserver)
				{
					aset.insert(iserver->m_id);
				});
		}

		const tab_servers* find_first(NODE_TYPE atype, i16_area aarea, const std::function<bool(const tab_servers*)>& afun)
		{
			std::set<i32_serverid> lset;
			get_server(atype, aarea, lset);
			for (i32_serverid serverid : lset)
			{
				const tab_servers* ltab = tab(serverid);
				if (ltab != nullptr && afun(tab(serverid)))
				{
					return ltab;
				}
			}
			return nullptr;
		}

		// 获取服务器所在区服(包括被合服的服务器)
		const std::set<i16_area>* get_arealist(i32_serverid aserverid)
		{
			const tab_servers* ltab = tab(aserverid);
			if (ltab == nullptr)
			{
				return nullptr;
			}
			return get_area(ltab->m_area);
		}

		// 获取服务器所在的区服(不包括被合服的服务器)
		const void get_arealist_nonrepet(i32_serverid aserverid, std::set<i16_area>& aareaset)
		{
			const std::set<i16_area>* larealist = get_arealist(aserverid);
			if (larealist == nullptr)
			{
				return;
			}
			for (i16_area larea : *larealist)
			{
				i16_area lmergeid = ttab_mergearea::instance().mergeid(larea);
				if (lmergeid == nguid::none_area() || lmergeid == larea)
				{
					aareaset.insert(larea);
					continue;
				}
				aareaset.insert(lmergeid);
			}			
		}
	};
}//namespace ngl

#define tab_self_area  ngl::ttab_servers::instance().tab()->m_area
#define tab_self_cros_area  ngl::ttab_servers::instance().tab()->m_crossarea
