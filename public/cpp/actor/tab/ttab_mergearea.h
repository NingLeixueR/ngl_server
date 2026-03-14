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
#include <atomic>
#include <functional>
#include <map>
#include <set>

namespace ngl
{
	struct ttab_mergearea :
		public csv<tab_mergearea>
	{
		ttab_mergearea(const ttab_mergearea&) = delete;
		ttab_mergearea& operator=(const ttab_mergearea&) = delete;

		// key: 区服id value: 合并到哪个区服(最终合并的区服)
		std::map<i16_area, i16_area> m_merge1;
		// key: 合服区服id value: 哪些区服在此区服
		std::map<i16_area, std::set<i16_area>> m_merge2;

		void reload()final
		{
			std::cout << "[ttab_mergearea] reload" << std::endl;
			m_merge1.clear();
			m_merge2.clear();

			std::map<i16_area, i16_area> ldirect;
			foreach([&ldirect](tab_mergearea& atab)
				{
					ldirect[static_cast<i16_area>(atab.m_id)] = static_cast<i16_area>(atab.m_mergeid);
				}
			);

			auto resolve_mergeid = [&ldirect](i16_area aarea)
			{
				std::set<i16_area> lvisited;
				i16_area lcurrent = aarea;
				while (true)
				{
					if (!lvisited.insert(lcurrent).second)
					{
						const i16_area lroot = *std::min_element(lvisited.begin(), lvisited.end());
						log_error()->print("ttab_mergearea::reload cycle detected area:[{}] root:[{}]", aarea, lroot);
						return lroot;
					}

					auto it = ldirect.find(lcurrent);
					if (it == ldirect.end() || it->second == lcurrent)
					{
						break;
					}
					lcurrent = it->second;
				}
				return lcurrent;
			};

			for (const auto& [area, _mergeid] : ldirect)
			{
				const i16_area lroot = resolve_mergeid(area);
				m_merge1[area] = lroot;
				m_merge2[lroot].insert(lroot);
				m_merge2[lroot].insert(area);
			}
		}
	public:
		using type_tab = tab_mergearea;

		ttab_mergearea() = default;

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
			const auto it = m_merge1.find(aarea);
			if (it == m_merge1.end())
			{
				return nguid::none_area();
			}
			return it->second;
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
