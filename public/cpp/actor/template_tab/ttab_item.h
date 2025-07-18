#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_item :
		public manage_csv<tab_item>
	{
		ttab_item(const ttab_item&) = delete;
		ttab_item& operator=(const ttab_item&) = delete;

		ttab_item()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_item] reload" << std::endl;
		}

	public:
		using type_tab = tab_item;

		static ttab_item& instance()
		{
			static ttab_item ltemp;
			return ltemp;
		}

		const std::map<int, tab_item>& tablecsv()
		{
			ttab_item* ttab = allcsv::get<ttab_item>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_item* tab(int32_t aid)
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
