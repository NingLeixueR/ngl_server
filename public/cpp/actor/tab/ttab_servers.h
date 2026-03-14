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
#pragma once

#include "actor/tab/ttab_mergearea.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/tab/xml/xml.h"
#include "tools/type.h"

#include <array>
#include <atomic>
#include <bit>
#include <functional>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace ngl
{
	class nnodeid
	{
		int32_t m_nodeid;
	public:
		explicit nnodeid(int32_t anodeid) :
			m_nodeid(anodeid)
		{}

		explicit nnodeid(int16_t atid, int16_t atcount) :
			m_nodeid(nodeid(atid, atcount))
		{}

		static int32_t nodeid(int16_t atid, int16_t atcount)
		{
			const auto ltid = static_cast<uint32_t>(static_cast<uint16_t>(atid));
			const auto ltcount = static_cast<uint32_t>(static_cast<uint16_t>(atcount)) << 16;
			return std::bit_cast<int32_t>(ltid | ltcount);
		}

		int32_t nodeid() const
		{
			return m_nodeid;
		}

		static int16_t tcount(int32_t anodeid)
		{
			const auto lnodeid = std::bit_cast<uint32_t>(anodeid);
			return static_cast<int16_t>(static_cast<uint16_t>(lnodeid >> 16));
		}

		static int16_t tid(int32_t anodeid)
		{
			const auto lnodeid = std::bit_cast<uint32_t>(anodeid);
			return static_cast<int16_t>(static_cast<uint16_t>(lnodeid & 0xFFFFu));
		}

		int16_t tcount() const
		{
			return tcount(m_nodeid);
		}

		int16_t tid() const
		{
			return tid(m_nodeid);
		}
	};


	struct ttab_servers :
		public csv<tab_servers>
	{
		ttab_servers(const ttab_servers&) = delete;
		ttab_servers& operator=(const ttab_servers&) = delete;

		std::map<i16_area, std::map<i32_serverid, tab_servers*>> m_areaserver;
		std::map<i16_area, std::set<i16_area>> m_coressserver;// key:应该是小于0的跨服 value:跨服对应的区服
		std::map<i16_area, std::set<i16_area>> m_singleareas;

		std::map<i16_area, std::set<i16_area>> m_singleareas;
		std::map<i16_area, std::map<std::string, tab_servers*, std::less<>>> m_areanameindex;
		std::map<i16_area, std::map<NODE_TYPE, tab_servers*>> m_areatypeindex;
		std::map<i16_area, std::map<NODE_TYPE, std::vector<const tab_servers*>>> m_areatypegroup;
		std::map<i32_serverid, std::array<const net_works*, static_cast<std::size_t>(ENET_COUNT)>> m_networkindex;

		static bool normalize_area(int area, i16_area& aout)
		{
			if (area < static_cast<int>(std::numeric_limits<i16_area>::min()) ||
				area > static_cast<int>(std::numeric_limits<i16_area>::max()))
			{
				return false;
			}
			aout = static_cast<i16_area>(area);
			return true;
		}

		void reload()final
		{
			std::cout << "[ttab_servers] reload" << std::endl;
			m_areaserver.clear();
			m_coressserver.clear();
			m_singleareas.clear();
			m_areanameindex.clear();
			m_areatypeindex.clear();
			m_areatypegroup.clear();
			m_networkindex.clear();
			foreach([&](tab_servers& atab)
				{
					m_areaserver[atab.m_area][atab.m_id] = &atab;
					m_singleareas[atab.m_area].insert(atab.m_area);
					m_areanameindex[atab.m_area].try_emplace(atab.m_name, &atab);
					m_areatypeindex[atab.m_area].try_emplace(atab.m_type, &atab);
					m_areatypegroup[atab.m_area][atab.m_type].push_back(&atab);

					auto& lnetwork = m_networkindex[atab.m_id];
					lnetwork.fill(nullptr);
					for (const net_works& item : atab.m_net)
					{
						if (item.m_type < ENET_TCP || item.m_type >= ENET_COUNT)
						{
							continue;
						}
						lnetwork[static_cast<std::size_t>(item.m_type)] = &item;
					}

					if (atab.m_area < 0)
					{
						std::set<i16_area> lsetarea;
						for (i32_serverid actorserverid : atab.m_actorserver)
						{
							const tab_servers* ltab = csv<tab_servers>::tab(actorserverid);
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
							m_coressserver[atab.m_area].insert(mergeareaset->begin(), mergeareaset->end());
						}
					}
				}
			);
		}
	public:
		using type_tab = tab_servers;

		ttab_servers() = default;

		static ttab_servers& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_servers>();
			}
			return *ncsv::get<ttab_servers>();
		}

		// # std::map<int, tab_servers>& tabs()
		// # tab_servers* tab(int aid)

		const tab_servers* const_tab()
		{
			return csv<tab_servers>::tab(nconfig.tid());
		}

		const tab_servers* const_tab(const std::string& aname, int area)
		{
			i16_area lquery_area = 0;
			if (!normalize_area(area, lquery_area))
			{
				return nullptr;
			}
			i16_area larea = ttab_mergearea::instance().mergeid(lquery_area);
			if (larea != nguid::none_area())
			{
				lquery_area = larea;
			}
			auto* lmap = tools::findmap(m_areanameindex, lquery_area);
			if (lmap == nullptr)
			{
				return nullptr;
			}
			auto* lserver = tools::findmap(*lmap, aname);
			return lserver == nullptr ? nullptr : *lserver;
		}

		const tab_servers* const_tab(NODE_TYPE atype, int area)
		{
			i16_area lquery_area = 0;
			if (!normalize_area(area, lquery_area))
			{
				return nullptr;
			}
			i16_area larea = ttab_mergearea::instance().mergeid(lquery_area);
			if (larea != nguid::none_area())
			{
				lquery_area = larea;
			}
			auto* lmap = tools::findmap(m_areatypeindex, lquery_area);
			if (lmap == nullptr)
			{
				return nullptr;
			}
			auto* lserver = tools::findmap(*lmap, atype);
			return lserver == nullptr ? nullptr : *lserver;
		}

	private:
		const net_works* nworks(ENET_PROTOCOL atype, const tab_servers* atab)
		{
			if (atab == nullptr || atype < ENET_TCP || atype >= ENET_COUNT)
			{
				return nullptr;
			}
			if (auto* lnetwork = tools::findmap(m_networkindex, atab->m_id); lnetwork != nullptr)
			{
				return (*lnetwork)[static_cast<std::size_t>(atype)];
			}
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
			return nworks(atype, const_tab());
		}

	public:
		bool get_nworks(const tab_servers* atab, ENET_PROTOCOL atype, int32_t atcount, net_works& anetwork)
		{
			if (atcount <= 0)
			{
				return false;
			}
			const net_works* lpnet = nworks(atype, atab);
			if (lpnet == nullptr)
			{
				return false;
			}
			anetwork = *lpnet;
			const int32_t lport = static_cast<int32_t>(anetwork.m_port) + (atcount - 1);
			if (lport < 0 || lport > static_cast<int32_t>(std::numeric_limits<i16_port>::max()))
			{
				return false;
			}
			anetwork.m_port = static_cast<i16_port>(lport);
			return true;
		}

		bool get_nworks(ENET_PROTOCOL atype, int32_t atcount, net_works& anetwork)
		{
			const tab_servers* ltab = const_tab();
			if (ltab == nullptr)
			{
				return false;
			}
			return get_nworks(ltab, atype, atcount, anetwork);
		}

		bool get_nworks(ENET_PROTOCOL atype, net_works& anetwork)
		{
			return get_nworks(atype, nconfig.tcount(), anetwork);
		}

		bool get_nworks(const std::string& aname, int area, int32_t atcount, ENET_PROTOCOL atype, net_works& anetwork)
		{
			const tab_servers* ltab = const_tab(aname, area);
			if (ltab == nullptr)
			{
				return false;
			}
			return get_nworks(ltab, atype, atcount, anetwork);
		}

		bool get_nworks(NODE_TYPE anodetype, i16_area area, ENET_PROTOCOL atype, int32_t atcount, net_works& anetwork)
		{
			const tab_servers* ltab = const_tab(anodetype, area);
			if (ltab == nullptr)
			{
				return false;
			}
			return get_nworks(ltab, atype, atcount, anetwork);
		}

		bool isefficient(ENET_PROTOCOL atype)
		{
			return atype == ENET_TCP || atype == ENET_WS;
		}
	private:
		bool connect(i32_serverid alocalserver, i32_serverid aotherserver, net_works& anetwork)
		{
			if (alocalserver == aotherserver)
			{
				tools::no_core_dump();
				return false;
			}

			const tab_servers* ltab1 = csv<tab_servers>::tab(nnodeid::tid(alocalserver));
			const tab_servers* ltab2 = csv<tab_servers>::tab(nnodeid::tid(aotherserver));
			if (ltab1 == nullptr || ltab2 == nullptr)
			{
				tools::no_core_dump();
				return false;
			}
			if (!get_nworks(ltab2->m_type, ltab2->m_area, ENET_TCP, nnodeid::tcount(aotherserver), anetwork))
			{
				tools::no_core_dump();
				return false;
			}
			return true;
		}
	public:
		bool connect(i32_serverid aserverid, net_works& anetwork)
		{
			return connect(nconfig.nodeid(), aserverid, anetwork);
		}

		NODE_TYPE node_type(int32_t atid)
		{
			const tab_servers* ltab = csv<tab_servers>::tab(atid);
			if (ltab == nullptr)
			{
				tools::no_core_dump();
				return FAIL;
			}
			return ltab->m_type;
		}

		NODE_TYPE nodetype()
		{
			return node_type(nconfig.tid());
		}

		// 便利所有服务器
		void foreach_server(const std::function<void(tab_servers*)>& afun)
		{
			ttab_mergearea::instance().for_each([&afun](i16_area aarea, [[maybe_unused]] std::set<i16_area>& aset)
				{
					auto lpmap = tools::findmap(ttab_servers::instance().m_areaserver, aarea);
					if (lpmap == nullptr)
					{
						return;
					}
					for (std::pair<const i32_serverid, tab_servers*>& item : *lpmap)
					{
						afun(item.second);
					}
				}
			);
		}

		// 获取所有区服(负数区服是跨服区服需要转化为跨服内所有区服)
		const std::set<i16_area>* get_area(i16_area aarea)
		{
			if (aarea > 0)
			{
				i16_area lquery_area = aarea;
				if (const i16_area merged_area = ttab_mergearea::instance().mergeid(aarea); merged_area != nguid::none_area())
				{
					lquery_area = merged_area;
				}
				if (const std::set<i16_area>* merged = ttab_mergearea::instance().mergelist(lquery_area); merged != nullptr)
				{
					return merged;
				}
				return tools::findmap(m_singleareas, lquery_area);
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
				auto lmap = tools::findmap(m_areatypegroup, area);
				if (lmap == nullptr)
				{
					continue;
				}
				auto lservers = tools::findmap(*lmap, atype);
				if (lservers == nullptr)
				{
					continue;
				}
				for (const tab_servers* lservertab : *lservers)
				{
					afun(lservertab);
				}
			}
			return true;
		}

		// 获取的是服务器的tid  没有结合tcount
		bool serverid(NODE_TYPE atype, i16_area aarea, std::set<i32_serverid>& aset)
		{
			return foreach_server(atype, aarea, [&aset](const tab_servers* iserver)
				{
					aset.insert(iserver->m_id);
				}
			);
		}

		const tab_servers* find_first(NODE_TYPE atype, i16_area aarea, const std::function<bool(const tab_servers*)>& afun)
		{
			std::set<i32_serverid> lset;
			serverid(atype, aarea, lset);
			for (i32_serverid serverid : lset)
			{
				const tab_servers* ltab = csv<tab_servers>::tab(serverid);
				if (ltab != nullptr && afun(ltab))
				{
					return ltab;
				}
			}
			return nullptr;
		}

		// 获取服务器所在区服(包括被合服的服务器)
		const std::set<i16_area>* get_arealist(i32_serverid aserverid)
		{
			const tab_servers* ltab = csv<tab_servers>::tab(aserverid);
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

#define tab_self_area  ngl::ttab_servers::instance().const_tab()->m_area
#define tab_self_cros_area  ngl::ttab_servers::instance().const_tab()->m_crossarea
