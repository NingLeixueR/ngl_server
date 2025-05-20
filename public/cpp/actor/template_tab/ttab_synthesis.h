#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_synthesis :
		public manage_csv<tab_synthesis>
	{
		ttab_synthesis(const ttab_synthesis&) = delete;
		ttab_synthesis operator=(const ttab_synthesis&) = delete;
		using type_tab = tab_synthesis;
		ttab_synthesis()
		{}

		static const tab_synthesis* tab(int32_t aid)
		{
			const ttab_synthesis* ttab = allcsv::get<ttab_synthesis>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
