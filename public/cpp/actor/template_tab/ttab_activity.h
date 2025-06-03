#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_activity :
		public manage_csv<tab_activity>
	{
		ttab_activity(const ttab_activity&) = delete;
		ttab_activity& operator=(const ttab_activity&) = delete;
		using type_tab = tab_activity;
		ttab_activity()
		{}

		static const std::map<int, tab_activity>& tablecsv()
		{
			const ttab_activity* ttab = allcsv::get<ttab_activity>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_activity* tab(int32_t aid)
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
