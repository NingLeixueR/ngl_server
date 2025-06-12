#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_activity_drawcompliance :
		public manage_csv<tab_activity_drawcompliance>
	{
		ttab_activity_drawcompliance(const ttab_activity_drawcompliance&) = delete;
		ttab_activity_drawcompliance& operator=(const ttab_activity_drawcompliance&) = delete;

		ttab_activity_drawcompliance()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_activity_drawcompliance] reload" << std::endl;
		}

	public:
		using type_tab = tab_activity_drawcompliance;

		static ttab_activity_drawcompliance& instance()
		{
			static ttab_activity_drawcompliance ltemp;
			return ltemp;
		}

		const std::map<int, tab_activity_drawcompliance>& tablecsv()
		{
			const ttab_activity_drawcompliance* ttab = allcsv::get<ttab_activity_drawcompliance>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}

		const tab_activity_drawcompliance* tab(int32_t aid)
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
