#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_ranklist :
		public manage_csv<tab_ranklist>
	{
		ttab_ranklist(const ttab_ranklist&) = delete;
		ttab_ranklist& operator=(const ttab_ranklist&) = delete;
		using type_tab = tab_ranklist;
		ttab_ranklist()
		{}

		static const std::map<int, tab_ranklist>& tablecsv()
		{
			const ttab_ranklist* ttab = allcsv::get<ttab_ranklist>();
			tools::core_dump(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_ranklist* tab(int32_t aid)
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
