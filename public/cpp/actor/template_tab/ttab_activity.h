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
		ttab_activity operator=(const ttab_activity&) = delete;
		using type_tab = tab_activity;
		ttab_activity()
		{}

		static const tab_activity* tab(int32_t aid)
		{
			const ttab_activity* ttab = allcsv::get<ttab_activity>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
