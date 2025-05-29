#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_openserveractivity :
		public manage_csv<tab_openserveractivity>
	{
		ttab_openserveractivity(const ttab_openserveractivity&) = delete;
		ttab_openserveractivity& operator=(const ttab_openserveractivity&) = delete;
		using type_tab = tab_openserveractivity;
		ttab_openserveractivity()
		{}

		static const std::map<int, tab_openserveractivity>& tablecsv()
		{
			const ttab_openserveractivity* ttab = allcsv::get<ttab_openserveractivity>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_openserveractivity* tab(int32_t aid)
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
