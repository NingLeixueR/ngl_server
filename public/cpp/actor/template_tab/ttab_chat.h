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
		ttab_chat& operator=(const ttab_chat&) = delete;
		using type_tab = tab_chat;
		ttab_chat()
		{}

		static const std::map<int, tab_chat>& tablecsv()
		{
			const ttab_chat* ttab = allcsv::get<ttab_chat>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_chat* tab(int32_t aid)
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
