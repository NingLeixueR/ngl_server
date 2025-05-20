#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_matching :
		public manage_csv<tab_matching>
	{
		ttab_matching(const ttab_matching&) = delete;
		ttab_matching& operator=(const ttab_matching&) = delete;
		using type_tab = tab_matching;
		ttab_matching()
		{}

		static const std::map<int, tab_matching>& tablecsv()
		{
			const ttab_matching* ttab = allcsv::get<ttab_matching>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_matching* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}
	};
}//namespace ngl
