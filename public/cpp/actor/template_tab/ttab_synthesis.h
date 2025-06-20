#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_synthesis :
		public manage_csv<tab_synthesis>
	{
		ttab_synthesis(const ttab_synthesis&) = delete;
		ttab_synthesis& operator=(const ttab_synthesis&) = delete;

		ttab_synthesis()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_synthesis] reload" << std::endl;
		}

	public:
		using type_tab = tab_synthesis;

		static ttab_synthesis& instance()
		{
			static ttab_synthesis ltemp;
			return ltemp;
		}

		const std::map<int, tab_synthesis>& tablecsv()
		{
			const ttab_synthesis* ttab = allcsv::get<ttab_synthesis>();
			if (ttab == nullptr);
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}
		
		const tab_synthesis* tab(int32_t aid)
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
