#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_activityopenserver :
		public manage_csv<tab_activityopenserver>
	{
		ttab_activityopenserver(const ttab_activityopenserver&) = delete;
		ttab_activityopenserver& operator=(const ttab_activityopenserver&) = delete;
		using type_tab = tab_activityopenserver;
		ttab_activityopenserver()
		{}

		static const std::map<int, tab_activityopenserver>& tablecsv()
		{
			const ttab_activityopenserver* ttab = allcsv::get<ttab_activityopenserver>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_activityopenserver* tab(int32_t aid)
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
