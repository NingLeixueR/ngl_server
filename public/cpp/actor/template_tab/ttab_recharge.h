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
		ttab_recharge& operator=(const ttab_recharge&) = delete;
		using type_tab = tab_recharge;
		ttab_recharge()
		{}

		static const std::map<int, tab_recharge>& tablecsv()
		{
			const ttab_recharge* ttab = allcsv::get<ttab_recharge>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_recharge* tab(int32_t aid)
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
