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

#include "manage_csv.h"
#include "tools.h"
#include "nguid.h"
#include "nlog.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_mergearea :
		public manage_csv<tab_mergearea>
	{
		ttab_mergearea(const ttab_mergearea&) = delete;
		ttab_mergearea& operator=(const ttab_mergearea&) = delete;

		// key: 区服id value: 合并到哪个区服(最终合并的区服)
		std::map<i16_area, i16_area> m_merge1;
		// key: 合服区服id value: 哪些区服在此区服
		std::map<i16_area, std::set<i16_area>> m_merge2;

		ttab_mergearea()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_mergearea] reload" << std::endl;
			// 1,2,3,4,5,6,7,8,9,10
			// 1->2, 3->4, 5->6, 7->8, 9->10
			// 2->4, 6->8
			// 4->10, 8->10
			// {{1,2},{2,4},{3,4},{4,10},{5,6},{6,8},{7,8},{8,10},{9.10}}
			// {10,{1,2,3,4,5,6,7,8,9,10}}
			for (std::pair<const int, tab_mergearea>& pair : m_tablecsv)
			{
				i16_area larea = (i16_area)pair.second.m_id;
				i16_area lmergeid = (i16_area)pair.second.m_mergeid;

				std::set<i16_area>* lpset = tools::findmap(m_merge2, larea);
				if (lpset == nullptr)
				{
					m_merge1[larea] = lmergeid;
					m_merge2[lmergeid].insert(larea);
				}
				else
				{
					m_merge2[lmergeid].insert(lpset->begin(), lpset->end());
					m_merge2[lmergeid].insert(larea);
					m_merge1[larea] = lmergeid;
					if (larea != lmergeid)
					{
						m_merge2.erase(larea);
					}					
				}				
			}
		}

	public:
		using type_tab = tab_mergearea;

		static ttab_mergearea& instance()
		{
			static ttab_mergearea ltemp;
			return ltemp;
		}

		const std::map<int, tab_mergearea>& tablecsv()
		{
			ttab_mergearea* ttab = allcsv::get<ttab_mergearea>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_mergearea* tab(int32_t aid)
		{
			auto itor = tablecsv().find(aid);
			if (itor == tablecsv().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		// 哪些区服在此区服
		std::set<i16_area>* mergelist(i16_area aarea)
		{
			if (!m_merge2.contains(aarea))
			{
				return nullptr;
			}
			return &m_merge2[aarea];
		}

		i16_area mergeid(i16_area aarea)
		{
			i16_area* ret = tools::findmap(m_merge1, aarea);
			if (ret == nullptr)
			{
				return nguid::none_area();
			}
			return *ret;
		}

		void foreach(const std::function<void(i16_area, std::set<i16_area>&)>& afun)
		{
			for (std::pair<const i16_area, std::set<i16_area>>& item : m_merge2)
			{
				afun(item.first, item.second);
			}
		}
	};
}//namespace ngl
