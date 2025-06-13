#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_matching :
		public manage_csv<tab_matching>
	{
		ttab_matching(const ttab_matching&) = delete;
		ttab_matching& operator=(const ttab_matching&) = delete;

		ttab_matching()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_matching] reload" << std::endl;
		}

	public:
		using type_tab = tab_matching;

		static ttab_matching& instance()
		{
			static ttab_matching ltemp;
			return ltemp;
		}

		const std::map<int, tab_matching>& tablecsv()
		{
			const ttab_matching* ttab = allcsv::get<ttab_matching>();
			tools::no_core_dump(ttab != nullptr);
			return ttab->m_tablecsv;
		}

		const tab_matching* tab(int32_t aid)
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
