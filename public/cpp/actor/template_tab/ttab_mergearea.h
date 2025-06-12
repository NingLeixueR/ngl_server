#pragma once

#include "manage_csv.h"
#include "tools.h"
#include "nguid.h"
#include "nlog.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_mergearea :
		public manage_csv<tab_mergearea>
	{
		ttab_mergearea(const ttab_mergearea&) = delete;
		ttab_mergearea& operator=(const ttab_mergearea&) = delete;
		using type_tab = tab_mergearea;

		// key: 区服id value: 合并到哪个区服
		static std::map<i16_area, i16_area> m_merge1;
		// key: 合服区服id value: 哪些区服在此区服
		static std::map<i16_area, std::set<i16_area>> m_merge2;
		static bool m_load;

		ttab_mergearea()
		{}

		static const std::map<int, tab_mergearea>& tablecsv()
		{
			const ttab_mergearea* ttab = allcsv::get<ttab_mergearea>();
			tools::core_dump(ttab == nullptr);
			return ttab->m_tablecsv;
		}

		static const tab_mergearea* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		void reload()final
		{
			for (std::pair<const int, tab_mergearea>& pair : m_tablecsv)
			{
				i16_area larea = (i16_area)pair.second.m_id;
				i16_area lmergeid = (i16_area)pair.second.m_mergeid;

				m_merge1[larea] = lmergeid;
				m_merge2[lmergeid].insert(larea);
			}
		}

		// 哪些区服在此区服
		static std::set<i16_area>* mergelist(i16_area aarea)
		{
			allcsv::loadcsv<ttab_mergearea>();
			return tools::findmap(m_merge2, aarea);
		}

		static i16_area mergeid(i16_area aarea)
		{
			allcsv::loadcsv<ttab_mergearea>();
			i16_area* ret = tools::findmap(m_merge1, aarea);
			if (ret == nullptr)
			{
				return nguid::none_area();
			}
			return *ret;
		}
	};
}//namespace ngl
