#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_recharge :
		public manage_csv<tab_recharge>
	{
		ttab_recharge(const ttab_recharge&) = delete;
		ttab_recharge operator=(const ttab_recharge&) = delete;
		using type_tab = tab_recharge;
		ttab_recharge()
		{}

		static const tab_recharge* tab(int32_t aid)
		{
			const ttab_recharge* ttab = allcsv::get<ttab_recharge>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
