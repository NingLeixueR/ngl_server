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
// File overview: Declares interfaces for tab.

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
#include <unordered_map>
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
		struct node_type_hash
		{
			std::size_t operator()(NODE_TYPE avalue) const noexcept
			{
				return std::hash<int>{}(static_cast<int>(avalue));
			}
		};

		using name_index = std::unordered_map<std::string, tab_servers*>;
		using type_index = std::unordered_map<NODE_TYPE, tab_servers*, node_type_hash>;
		using type_group_index = std::unordered_map<NODE_TYPE, std::vector<const tab_servers*>, node_type_hash>;
		using network_index = std::unordered_map<i32_serverid, std::array<const net_works*, static_cast<std::size_t>(ENET_COUNT)>>;

		ttab_servers(const ttab_servers&) = delete;
		ttab_servers& operator=(const ttab_servers&) = delete;

		std::map<i16_area, std::map<i32_serverid, tab_servers*>> m_areaserver;
		std::map<i16_area, std::set<i16_area>> m_coressserver;// Key: this 0 cross-server value:cross-servercorresponding area
		std::map<i16_area, std::set<i16_area>> m_singleareas;

		std::map<i16_area, name_index> m_areanameindex;
		std::map<i16_area, type_index> m_areatypeindex;
		std::map<i16_area, type_group_index> m_areatypegroup;
		network_index m_networkindex;

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

		static bool resolve_lookup_area(int area, i16_area& aout)
		{
			if (!normalize_area(area, aout))
			{
				return false;
			}
			if (const i16_area merged_area = ttab_mergearea::instance().mergeid(aout); merged_area != nguid::none_area())
			{
				aout = merged_area;
			}
			return true;
		}

		void load_row(tab_servers& aserver, ttab_mergearea& amerge)
		{
			auto& larea_srv = m_areaserver[aserver.m_area];
			auto& lsingle = m_singleareas[aserver.m_area];
			auto& lname_idx = m_areanameindex[aserver.m_area];
			auto& ltype_idx = m_areatypeindex[aserver.m_area];
			auto& ltype_grp = m_areatypegroup[aserver.m_area];

			larea_srv[aserver.m_id] = &aserver;
			lsingle.insert(aserver.m_area);
			lname_idx.try_emplace(aserver.m_name, &aserver);
			ltype_idx.try_emplace(aserver.m_type, &aserver);
			ltype_grp[aserver.m_type].push_back(&aserver);

			auto& lnet_slots = m_networkindex.try_emplace(aserver.m_id).first->second;
			lnet_slots.fill(nullptr);
			for (const net_works& lnet : aserver.m_net)
			{
				if (lnet.m_type < ENET_TCP || lnet.m_type >= ENET_COUNT)
				{
					continue;
				}
				lnet_slots[static_cast<std::size_t>(lnet.m_type)] = &lnet;
			}

			if (aserver.m_area >= 0)
			{
				return;
			}

			std::set<i16_area> lrel_areas;
			for (i32_serverid lactor_sid : aserver.m_actorserver)
			{
				const tab_servers* lrel_srv = csv<tab_servers>::tab(lactor_sid);
				if (lrel_srv == nullptr)
				{
					continue;
				}
				lrel_areas.insert(lrel_srv->m_area);
			}

			auto& lcross_areas = m_coressserver[aserver.m_area];
			for (i16_area larea : lrel_areas)
			{
				const i16_area lmerge_area = amerge.mergeid(larea);
				if (lmerge_area == nguid::none_area())
				{
					continue;
				}
				std::set<i16_area>* larea_set = amerge.mergelist(lmerge_area);
				if (larea_set == nullptr)
				{
					continue;
				}
				lcross_areas.insert(larea_set->begin(), larea_set->end());
			}
		}

		void reload()final
		{
			m_areaserver.clear();
			m_coressserver.clear();
			m_singleareas.clear();
			m_areanameindex.clear();
			m_areatypeindex.clear();
			m_areatypegroup.clear();
			m_networkindex.clear();
			m_networkindex.reserve(m_csv.size());
			auto& lmerge = ttab_mergearea::instance();
			foreach([this, &lmerge](tab_servers& aserver)
				{
					load_row(aserver, lmerge);
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
			i16_area lookup_area = 0;
			if (!resolve_lookup_area(area, lookup_area))
			{
				return nullptr;
			}
			const auto* area_name_index = tools::findmap(m_areanameindex, lookup_area);
			if (area_name_index == nullptr)
			{
				return nullptr;
			}
			const auto* server_entry = tools::findmap(*area_name_index, aname);
			return server_entry == nullptr ? nullptr : *server_entry;
		}

		const tab_servers* const_tab(NODE_TYPE atype, int area)
		{
			i16_area lookup_area = 0;
			if (!resolve_lookup_area(area, lookup_area))
			{
				return nullptr;
			}
			const auto* area_type_index = tools::findmap(m_areatypeindex, lookup_area);
			if (area_type_index == nullptr)
			{
				return nullptr;
			}
			const auto* server_entry = tools::findmap(*area_type_index, atype);
			return server_entry == nullptr ? nullptr : *server_entry;
		}

	private:
		const net_works* nworks(ENET_PROTOCOL atype, const tab_servers* atab)
		{
			if (atab == nullptr || atype < ENET_TCP || atype >= ENET_COUNT)
			{
				return nullptr;
			}
			if (auto* network_slots = tools::findmap(m_networkindex, atab->m_id); network_slots != nullptr)
			{
				return (*network_slots)[static_cast<std::size_t>(atype)];
			}
			for (const net_works& network : atab->m_net)
			{
				if (network.m_type == atype)
				{
					return &network;
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

		bool get_nworks(i32_serverid aserverid, int area, ENET_PROTOCOL atype, net_works& anetwork)
		{
			i16_area lookup_area = 0;
			if (!normalize_area(area, lookup_area))
			{
				return false;
			}
			const tab_servers* server_tab = csv<tab_servers>::tab(nnodeid::tid(aserverid));
			if (server_tab == nullptr)
			{
				return false;
			}
			return get_nworks(server_tab->m_type, lookup_area, atype, nnodeid::tcount(aserverid), anetwork);
		}

		bool get_nworks(i32_serverid aserverid, ENET_PROTOCOL atype, net_works& anetwork)
		{
			return get_nworks(aserverid, nconfig.area(), atype, anetwork);
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
				log_error()->print("ttab_servers::connect same server [{}]", alocalserver);
				return false;
			}

			const bool lhas_local = csv<tab_servers>::tab(nnodeid::tid(alocalserver)) != nullptr;
			const tab_servers* lother_tab = csv<tab_servers>::tab(nnodeid::tid(aotherserver));
			if (!lhas_local || lother_tab == nullptr)
			{
				log_error()->print(
					"ttab_servers::connect missing server local:[{}] remote:[{}]",
					alocalserver,
					aotherserver);
				return false;
			}
			if (!get_nworks(lother_tab->m_type, lother_tab->m_area, ENET_TCP, nnodeid::tcount(aotherserver), anetwork))
			{
				log_error()->print("ttab_servers::connect missing tcp remote:[{}]", aotherserver);
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
				log_error()->print("ttab_servers::node_type missing tid [{}]", atid);
				return FAIL;
			}
			return ltab->m_type;
		}

		NODE_TYPE nodetype()
		{
			return node_type(nconfig.tid());
		}

		// Allserver
		void foreach_server(const std::function<void(tab_servers*)>& afun)
		{
			ttab_mergearea::instance().for_each([&afun, this](i16_area aarea, [[maybe_unused]] std::set<i16_area>& aset)
				{
					auto* area_servers = tools::findmap(m_areaserver, aarea);
					if (area_servers == nullptr)
					{
						return;
					}
					for (const std::pair<const i32_serverid, tab_servers*>& entry : *area_servers)
					{
						afun(entry.second);
					}
				}
			);
		}

		// Getallarea( area cross-serverareaneed to cross-server allarea)
		const std::set<i16_area>* get_area(i16_area aarea)
		{
			if (aarea <= 0)
			{
				return tools::findmap(m_coressserver, aarea);
			}

			auto& merge_table = ttab_mergearea::instance();
			i16_area lookup_area = aarea;
			if (const i16_area merged_area = merge_table.mergeid(aarea); merged_area != nguid::none_area())
			{
				lookup_area = merged_area;
			}
			if (const std::set<i16_area>* merged_areas = merge_table.mergelist(lookup_area); merged_areas != nullptr)
			{
				return merged_areas;
			}
			return tools::findmap(m_singleareas, lookup_area);
		}

		// Servertype atype
		// Area aarea( tablecross-server,need to cross-server allatypeserver)
		template <typename TFun>
		bool foreach_server(NODE_TYPE atype, i16_area aarea, const TFun& afun)
		{
			const std::set<i16_area>* area_list = get_area(aarea);
			if (area_list == nullptr)
			{
				return false;
			}
			for (i16_area area : *area_list)
			{
				auto* area_type_group = tools::findmap(m_areatypegroup, area);
				if (area_type_group == nullptr)
				{
					continue;
				}
				auto* server_tabs = tools::findmap(*area_type_group, atype);
				if (server_tabs == nullptr)
				{
					continue;
				}
				for (const tab_servers* server_tab : *server_tabs)
				{
					afun(server_tab);
				}
			}
			return true;
		}

		// Get server tid tcount
		bool serverid(NODE_TYPE atype, i16_area aarea, std::set<i32_serverid>& aset)
		{
			return foreach_server(atype, aarea, [&aset](const tab_servers* server_tab)
				{
					aset.insert(server_tab->m_id);
				}
			);
		}

		const tab_servers* find_first(NODE_TYPE atype, i16_area aarea, const std::function<bool(const tab_servers*)>& afun)
		{
			const tab_servers* best_match = nullptr;
			foreach_server(atype, aarea, [&afun, &best_match](const tab_servers* server_tab)
			{
				if (server_tab != nullptr && afun(server_tab) && (best_match == nullptr || server_tab->m_id < best_match->m_id))
				{
					best_match = server_tab;
				}
			});
			return best_match;
		}

		// Getserver area(pack server)
		const std::set<i16_area>* get_arealist(i32_serverid aserverid)
		{
			const tab_servers* ltab = csv<tab_servers>::tab(aserverid);
			if (ltab == nullptr)
			{
				return nullptr;
			}
			return get_area(ltab->m_area);
		}

		// Getserver area( pack server)
		void get_arealist_nonrepet(i32_serverid aserverid, std::set<i16_area>& aareaset)
		{
			const std::set<i16_area>* area_list = get_arealist(aserverid);
			if (area_list == nullptr)
			{
				return;
			}
			auto& merge_table = ttab_mergearea::instance();
			for (i16_area area : *area_list)
			{
				const i16_area merged_area = merge_table.mergeid(area);
				if (merged_area == nguid::none_area() || merged_area == area)
				{
					aareaset.insert(area);
					continue;
				}
				aareaset.insert(merged_area);
			}
		}
	};
}//namespace ngl

#define tab_self_area  ngl::ttab_servers::instance().const_tab()->m_area
#define tab_self_cros_area  ngl::ttab_servers::instance().const_tab()->m_crossarea
