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
		ttab_equip operator=(const ttab_equip&) = delete;
		using type_tab = tab_equip;
		ttab_equip()
		{}

		static const tab_equip* tab(int32_t aid)
		{
			const ttab_equip* ttab = allcsv::get<ttab_equip>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
