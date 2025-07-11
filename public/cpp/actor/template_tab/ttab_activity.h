#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_activity :
		public manage_csv<tab_activity>
	{
		ttab_activity(const ttab_activity&) = delete;
		ttab_activity& operator=(const ttab_activity&) = delete;

		ttab_activity()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_activity] reload" << std::endl;
		}

	public:
		using type_tab = tab_activity;

		static ttab_activity& instance()
		{
			static ttab_activity ltemp;
			return ltemp;
		}

		std::map<int, tab_activity>& tablecsv()
		{
			ttab_activity *ttab = allcsv::get<ttab_activity>();
			if (ttab == nullptr);
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_activity* tab(int32_t aid)
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
