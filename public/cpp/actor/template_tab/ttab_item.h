#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_item :
		public manage_csv<tab_item>
	{
		ttab_item(const ttab_item&) = delete;
		ttab_item& operator=(const ttab_item&) = delete;
		using type_tab = tab_item;
		ttab_item()
		{}

		static const std::map<int, tab_item>& tablecsv()
		{
			const ttab_item* ttab = allcsv::get<ttab_item>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_item* tab(int32_t aid)
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
