#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_plays :
		public manage_csv<tab_plays>
	{
		ttab_plays(const ttab_plays&) = delete;
		ttab_plays& operator=(const ttab_plays&) = delete;
		using type_tab = tab_plays;
		ttab_plays()
		{}

		static const std::map<int, tab_plays>& tablecsv()
		{
			const ttab_plays* ttab = allcsv::get<ttab_plays>();
			tools::core_dump(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_plays* tab(int32_t aid)
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
