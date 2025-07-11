#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_card :
		public manage_csv<tab_card>
	{
		ttab_card(const ttab_card&) = delete;
		ttab_card& operator=(const ttab_card&) = delete;

		ttab_card()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_card] reload" << std::endl;
		}

	public:
		using type_tab = tab_card;

		static ttab_card& instance()
		{
			static ttab_card ltemp;
			return ltemp;
		}

		std::map<int, tab_card>& tablecsv()
		{
			ttab_card* ttab = allcsv::get<ttab_card>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_card* tab(int32_t aid)
		{
			auto lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}
	};
}//namespace ngl
