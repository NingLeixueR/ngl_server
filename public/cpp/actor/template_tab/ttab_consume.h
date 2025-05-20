#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_consume :
		public manage_csv<tab_consume>
	{
		ttab_consume(const ttab_consume&) = delete;
		ttab_consume operator=(const ttab_consume&) = delete;
		using type_tab = tab_consume;
		ttab_consume()
		{}

		static const tab_consume* tab(int32_t aid)
		{
			const ttab_consume* ttab = allcsv::get<ttab_consume>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
