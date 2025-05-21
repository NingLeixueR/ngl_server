#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_errormessage :
		public manage_csv<tab_errormessage>
	{
		ttab_errormessage(const ttab_errormessage&) = delete;
		ttab_errormessage& operator=(const ttab_errormessage&) = delete;
		using type_tab = tab_errormessage;
		ttab_errormessage()
		{}

		static const std::map<int, tab_errormessage>& tablecsv()
		{
			const ttab_errormessage* ttab = allcsv::get<ttab_errormessage>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_errormessage* tab(int32_t aid)
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
