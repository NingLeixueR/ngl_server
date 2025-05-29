#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_activityalways :
		public manage_csv<tab_activityalways>
	{
		ttab_activityalways(const ttab_activityalways&) = delete;
		ttab_activityalways& operator=(const ttab_activityalways&) = delete;
		using type_tab = tab_activityalways;
		ttab_activityalways()
		{}

		static const std::map<int, tab_activityalways>& tablecsv()
		{
			const ttab_activityalways* ttab = allcsv::get<ttab_activityalways>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_activityalways* tab(int32_t aid)
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
