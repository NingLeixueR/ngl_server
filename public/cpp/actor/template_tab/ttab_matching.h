#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_matching :
		public manage_csv<tab_matching>
	{
		ttab_matching(const ttab_matching&) = delete;
		ttab_matching operator=(const ttab_matching&) = delete;
		using type_tab = tab_matching;
		ttab_matching()
		{}

		static const tab_matching* tab(int32_t aid)
		{
			const ttab_matching* ttab = allcsv::get<ttab_matching>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
