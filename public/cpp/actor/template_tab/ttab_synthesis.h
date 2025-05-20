#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_synthesis :
		public manage_csv<tab_synthesis>
	{
		ttab_synthesis(const ttab_synthesis&) = delete;
		ttab_synthesis& operator=(const ttab_synthesis&) = delete;
		using type_tab = tab_synthesis;
		ttab_synthesis()
		{}

		static const std::map<int, tab_synthesis>& tablecsv()
		{
			const ttab_synthesis* ttab = allcsv::get<ttab_synthesis>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_synthesis* tab(int32_t aid)
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
