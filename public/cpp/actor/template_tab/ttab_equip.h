#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_equip :
		public manage_csv<tab_equip>
	{
		ttab_equip(const ttab_equip&) = delete;
		ttab_equip& operator=(const ttab_equip&) = delete;
		using type_tab = tab_equip;
		ttab_equip()
		{}

		static const std::map<int, tab_equip>& tablecsv()
		{
			const ttab_equip* ttab = allcsv::get<ttab_equip>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_equip* tab(int32_t aid)
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
