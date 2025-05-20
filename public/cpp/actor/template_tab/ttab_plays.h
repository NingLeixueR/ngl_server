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
		ttab_plays operator=(const ttab_plays&) = delete;
		using type_tab = tab_plays;
		ttab_plays()
		{}

		static const tab_plays* tab(int32_t aid)
		{
			const ttab_plays* ttab = allcsv::get<ttab_plays>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
