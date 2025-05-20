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
		ttab_map operator=(const ttab_map&) = delete;
		using type_tab = tab_map;
		ttab_map()
		{}

		static const tab_map* tab(int32_t aid)
		{
			const ttab_map* ttab = allcsv::get<ttab_map>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
