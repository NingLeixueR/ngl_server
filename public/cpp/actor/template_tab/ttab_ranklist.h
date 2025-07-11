#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_ranklist :
		public manage_csv<tab_ranklist>
	{
		ttab_ranklist(const ttab_ranklist&) = delete;
		ttab_ranklist& operator=(const ttab_ranklist&) = delete;

		ttab_ranklist()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_ranklist] reload" << std::endl;
		}

	public:
		using type_tab = tab_ranklist;

		static ttab_ranklist& instance()
		{
			static ttab_ranklist ltemp;
			return ltemp;
		}

		std::map<int, tab_ranklist>& tablecsv()
		{
			ttab_ranklist* ttab = allcsv::get<ttab_ranklist>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_ranklist* tab(int32_t aid)
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
