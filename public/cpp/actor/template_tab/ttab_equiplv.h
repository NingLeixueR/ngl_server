#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_equiplv :
		public manage_csv<tab_equiplv>
	{
		ttab_equiplv(const ttab_equiplv&) = delete;
		ttab_equiplv& operator=(const ttab_equiplv&) = delete;
		using type_tab = tab_equiplv;
		ttab_equiplv()
		{}

		static const std::map<int, tab_equiplv>& tablecsv()
		{
			const ttab_equiplv* ttab = allcsv::get<ttab_equiplv>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_equiplv* tab(int32_t aid)
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
