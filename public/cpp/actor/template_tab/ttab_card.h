#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_card :
		public manage_csv<tab_card>
	{
		ttab_card(const ttab_card&) = delete;
		ttab_card& operator=(const ttab_card&) = delete;
		using type_tab = tab_card;
		ttab_card()
		{}

		static const std::map<int, tab_card>& tablecsv()
		{
			const ttab_card* ttab = allcsv::get<ttab_card>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_card* tab(int32_t aid)
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
