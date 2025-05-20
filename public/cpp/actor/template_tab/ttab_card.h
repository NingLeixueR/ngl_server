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
		ttab_card operator=(const ttab_card&) = delete;
		using type_tab = tab_card;
		ttab_card()
		{}

		static const tab_card* tab(int32_t aid)
		{
			const ttab_card* ttab = allcsv::get<ttab_card>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
