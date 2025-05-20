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
		ttab_item operator=(const ttab_item&) = delete;
		using type_tab = tab_item;
		ttab_item()
		{}

		static const tab_item* tab(int32_t aid)
		{
			const ttab_item* ttab = allcsv::get<ttab_item>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
