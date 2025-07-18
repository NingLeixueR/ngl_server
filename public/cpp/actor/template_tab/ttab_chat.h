#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_chat :
		public manage_csv<tab_chat>
	{
		ttab_chat(const ttab_chat&) = delete;
		ttab_chat& operator=(const ttab_chat&) = delete;

		ttab_chat()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_chat] reload" << std::endl;
		}

	public:
		using type_tab = tab_chat;

		static ttab_chat& instance()
		{
			static ttab_chat ltemp;
			return ltemp;
		}

		const std::map<int, tab_chat>& tablecsv()
		{
			ttab_chat* ttab = allcsv::get<ttab_chat>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_chat* tab(int32_t aid)
		{
			auto itor = tablecsv().find(aid);
			if (itor == tablecsv().end())
			{
				return nullptr;
			}
			return &itor->second;
		}
	};
}//namespace ngl
