#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_mail :
		public manage_csv<tab_mail>
	{
		ttab_mail(const ttab_mail&) = delete;
		ttab_mail& operator=(const ttab_mail&) = delete;
		using type_tab = tab_mail;
		ttab_mail()
		{}

		static const std::map<int, tab_mail>& tablecsv()
		{
			const ttab_mail* ttab = allcsv::get<ttab_mail>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_mail* tab(int32_t aid)
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
