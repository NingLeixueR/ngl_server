#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_activity_drawcompliance :
		public manage_csv<tab_activity_drawcompliance>
	{
		ttab_activity_drawcompliance(const ttab_activity_drawcompliance&) = delete;
		ttab_activity_drawcompliance& operator=(const ttab_activity_drawcompliance&) = delete;
		using type_tab = tab_activity_drawcompliance;
		ttab_activity_drawcompliance()
		{}

		static const std::map<int, tab_activity_drawcompliance>& tablecsv()
		{
			const ttab_activity_drawcompliance* ttab = allcsv::get<ttab_activity_drawcompliance>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_activity_drawcompliance* tab(int32_t aid)
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
