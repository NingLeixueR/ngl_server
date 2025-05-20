#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_equiplv :
		public manage_csv<tab_equiplv>
	{
		ttab_equiplv(const ttab_equiplv&) = delete;
		ttab_equiplv operator=(const ttab_equiplv&) = delete;
		using type_tab = tab_equiplv;
		ttab_equiplv()
		{}

		static const tab_equiplv* tab(int32_t aid)
		{
			const ttab_equiplv* ttab = allcsv::get<ttab_equiplv>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
