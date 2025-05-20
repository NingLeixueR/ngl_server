#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_chat :
		public manage_csv<tab_chat>
	{
		ttab_chat(const ttab_chat&) = delete;
		ttab_chat operator=(const ttab_chat&) = delete;
		using type_tab = tab_chat;
		ttab_chat()
		{}

		static const tab_chat* tab(int32_t aid)
		{
			const ttab_chat* ttab = allcsv::get<ttab_chat>();
			if (ttab == nullptr)
			{
				 return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(aid);
			return &itor->second;
		}
	};
}//namespace ngl
