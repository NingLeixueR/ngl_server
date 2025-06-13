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

		// key: 区服id value: 合并到哪个区服
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
			for (std::pair<const int, tab_mergearea>& pair : m_tablecsv)
			{
				i16_area larea = (i16_area)pair.second.m_id;
				i16_area lmergeid = (i16_area)pair.second.m_mergeid;

				m_merge1[larea] = lmergeid;
				m_merge2[lmergeid].insert(larea);
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
			const ttab_mergearea* ttab = allcsv::get<ttab_mergearea>();
			tools::no_core_dump(ttab != nullptr);
			return ttab->m_tablecsv;
		}

		const tab_mergearea* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		// 哪些区服在此区服
		std::set<i16_area>* mergelist(i16_area aarea)
		{
			tools::no_core_dump(m_merge2.contains(aarea));
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
	};
}//namespace ngl
