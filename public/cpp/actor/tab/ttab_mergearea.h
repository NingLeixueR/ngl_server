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

#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/nguid.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/tab/xml/xml.h"
#include "tools/log/nlog.h"
#include "tools/tools.h"
#include "tools/type.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <vector>

namespace ngl
{
	struct ttab_mergearea :
		public csv<tab_mergearea>
	{
		ttab_mergearea(const ttab_mergearea&) = delete;
		ttab_mergearea& operator=(const ttab_mergearea&) = delete;

		// key: 区服id value: 合并到哪个区服(最终合并的区服)
		static constexpr std::size_t m_merge_slot_count = 1u << 16;
		static constexpr std::size_t merge_slot(i16_area aarea) noexcept
		{
			return static_cast<std::size_t>(static_cast<std::uint16_t>(aarea));
		}
		std::array<i16_area, m_merge_slot_count> m_merge1;
		// key: 合服区服id value: 哪些区服在此区服
		std::map<i16_area, std::set<i16_area>> m_merge2;

		void reload()final
		{
			std::cout << "[ttab_mergearea] reload" << std::endl;
			m_merge1.fill(nguid::none_area());
			m_merge2.clear();

			std::vector<i16_area> ldirect(m_merge_slot_count, nguid::none_area());
			std::vector<i16_area> lresolved(m_merge_slot_count, nguid::none_area());
			std::vector<std::uint32_t> lvisit_stamp(m_merge_slot_count, 0);
			std::vector<i16_area> lpath;
			lpath.reserve(m_csv.size());
			std::uint32_t lcurrent_stamp = 0;

			foreach([&ldirect](tab_mergearea& atab)
				{
					ldirect[merge_slot(static_cast<i16_area>(atab.m_id))] = static_cast<i16_area>(atab.m_mergeid);
				}
			);

			auto next_visit_stamp = [&lvisit_stamp, &lcurrent_stamp]()
			{
				++lcurrent_stamp;
				if (lcurrent_stamp == 0)
				{
					std::fill(lvisit_stamp.begin(), lvisit_stamp.end(), 0);
					lcurrent_stamp = 1;
				}
				return lcurrent_stamp;
			};

			auto resolve_mergeid = [&](i16_area aarea)
			{
				const std::uint32_t lstamp = next_visit_stamp();
				lpath.clear();
				i16_area lcurrent = aarea;
				i16_area lroot = nguid::none_area();
				while (true)
				{
					const std::size_t lslot = merge_slot(lcurrent);
					if (const i16_area lcached = lresolved[lslot]; lcached != nguid::none_area())
					{
						lroot = lcached;
						break;
					}
					if (lvisit_stamp[lslot] == lstamp)
					{
						lroot = *std::min_element(lpath.begin(), lpath.end());
						log_error()->print("ttab_mergearea::reload cycle detected area:[{}] root:[{}]", aarea, lroot);
						break;
					}
					lvisit_stamp[lslot] = lstamp;
					lpath.push_back(lcurrent);

					const i16_area lnext = ldirect[lslot];
					if (lnext == nguid::none_area() || lnext == lcurrent)
					{
						lroot = lcurrent;
						break;
					}
					lcurrent = lnext;
				}

				for (i16_area area : lpath)
				{
					lresolved[merge_slot(area)] = lroot;
				}
				return lroot;
			};

			foreach([&](tab_mergearea& atab)
				{
					const i16_area area = static_cast<i16_area>(atab.m_id);
					const i16_area lroot = resolve_mergeid(area);
					m_merge1[merge_slot(area)] = lroot;
					m_merge2[lroot].insert(lroot);
					m_merge2[lroot].insert(area);
				}
			);
		}
	public:
		using type_tab = tab_mergearea;

		ttab_mergearea()
		{
			m_merge1.fill(nguid::none_area());
		}

		static ttab_mergearea& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_mergearea>();
			}
			return *ncsv::get<ttab_mergearea>();
		}

		// # std::map<int, tab_mergearea>& tabs()
		// # tab_mergearea* tab(int aid)

		// 哪些区服在此区服
		std::set<i16_area>* mergelist(i16_area aarea)
		{
			const auto it = m_merge2.find(aarea);
			if (it == m_merge2.end())
			{
				return nullptr;
			}
			return &it->second;
		}

		i16_area mergeid(i16_area aarea)
		{
			return m_merge1[merge_slot(aarea)];
		}

		void for_each(const std::function<void(i16_area, std::set<i16_area>&)>& afun)
		{
			for (std::pair<const i16_area, std::set<i16_area>>& item : m_merge2)
			{
				afun(item.first, item.second);
			}
		}
	};
}//namespace ngl
