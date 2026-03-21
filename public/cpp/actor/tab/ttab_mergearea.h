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

		// Key: areaid value: andto area( and area)
		static constexpr std::size_t m_merge_slot_count = 1u << 16;
		static constexpr std::size_t merge_slot(i16_area aarea) noexcept
		{
			return static_cast<std::size_t>(static_cast<std::uint16_t>(aarea));
		}
		std::array<i16_area, m_merge_slot_count> m_merge1;
		// Key: areaid value: whicharea this area
		std::map<i16_area, std::set<i16_area>> m_merge2;

		void reload()final
		{
			m_merge1.fill(nguid::none_area());
			m_merge2.clear();

			std::vector<i16_area> direct_merge(m_merge_slot_count, nguid::none_area());
			std::vector<i16_area> resolved_merge(m_merge_slot_count, nguid::none_area());
			std::vector<std::uint32_t> visit_stamp(m_merge_slot_count, 0);
			std::vector<i16_area> visit_path;
			visit_path.reserve(m_csv.size());
			std::uint32_t current_stamp = 0;

			foreach([&direct_merge](tab_mergearea& merge_row)
				{
					direct_merge[merge_slot(static_cast<i16_area>(merge_row.m_id))] = static_cast<i16_area>(merge_row.m_mergeid);
				}
			);

			auto next_visit_stamp = [&visit_stamp, &current_stamp]()
			{
				++current_stamp;
				if (current_stamp == 0)
				{
					std::fill(visit_stamp.begin(), visit_stamp.end(), 0);
					current_stamp = 1;
				}
				return current_stamp;
			};

			auto resolve_mergeid = [&](i16_area aarea)
			{
				const std::uint32_t stamp = next_visit_stamp();
				visit_path.clear();
				i16_area current_area = aarea;
				i16_area root_area = nguid::none_area();
				while (true)
				{
					const std::size_t slot = merge_slot(current_area);
					if (const i16_area cached_root = resolved_merge[slot]; cached_root != nguid::none_area())
					{
						root_area = cached_root;
						break;
					}
					if (visit_stamp[slot] == stamp)
					{
						root_area = *std::min_element(visit_path.begin(), visit_path.end());
						log_error()->print("ttab_mergearea::reload cycle detected area:[{}] root:[{}]", aarea, root_area);
						break;
					}
					visit_stamp[slot] = stamp;
					visit_path.push_back(current_area);

					const i16_area next_area = direct_merge[slot];
					if (next_area == nguid::none_area() || next_area == current_area)
					{
						root_area = current_area;
						break;
					}
					current_area = next_area;
				}

				for (i16_area area : visit_path)
				{
					resolved_merge[merge_slot(area)] = root_area;
				}
				return root_area;
			};

			foreach([&](tab_mergearea& merge_row)
				{
					const i16_area area = static_cast<i16_area>(merge_row.m_id);
					const i16_area root_area = resolve_mergeid(area);
					m_merge1[merge_slot(area)] = root_area;
					auto& merged_areas = m_merge2[root_area];
					merged_areas.insert(root_area);
					merged_areas.insert(area);
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

		// Whicharea this area
		std::set<i16_area>* mergelist(i16_area aarea)
		{
			return tools::findmap(m_merge2, aarea);
		}

		i16_area mergeid(i16_area aarea)
		{
			return m_merge1[merge_slot(aarea)];
		}

		void for_each(const std::function<void(i16_area, std::set<i16_area>&)>& afun)
		{
			for (std::pair<const i16_area, std::set<i16_area>>& merged_entry : m_merge2)
			{
				afun(merged_entry.first, merged_entry.second);
			}
		}
	};
}//namespace ngl
