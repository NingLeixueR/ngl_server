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
		ttab_mail operator=(const ttab_mail&) = delete;
		using type_tab = tab_mail;
		ttab_mail()
		{}

		static const tab_mail* tab(int32_t aid)
		{
			const ttab_mail* ttab = allcsv::get<ttab_mail>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
