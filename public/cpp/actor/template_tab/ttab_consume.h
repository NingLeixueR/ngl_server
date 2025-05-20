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
		ttab_consume& operator=(const ttab_consume&) = delete;
		using type_tab = tab_consume;
		ttab_consume()
		{}

		static const std::map<int, tab_consume>& tablecsv()
		{
			const ttab_consume* ttab = allcsv::get<ttab_consume>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_consume* tab(int32_t aid)
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
