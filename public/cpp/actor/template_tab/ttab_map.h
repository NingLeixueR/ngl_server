#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_map :
		public manage_csv<tab_map>
	{
		ttab_map(const ttab_map&) = delete;
		ttab_map& operator=(const ttab_map&) = delete;
		using type_tab = tab_map;
		ttab_map()
		{}

		static const std::map<int, tab_map>& tablecsv()
		{
			const ttab_map* ttab = allcsv::get<ttab_map>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_map* tab(int32_t aid)
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
