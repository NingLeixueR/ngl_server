#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_map :
		public manage_csv<tab_map>
	{
		ttab_map(const ttab_map&) = delete;
		ttab_map& operator=(const ttab_map&) = delete;

		ttab_map()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_map] reload" << std::endl;
		}

	public:
		using type_tab = tab_map;

		static ttab_map& instance()
		{
			static ttab_map ltemp;
			return ltemp;
		}

		const std::map<int, tab_map>& tablecsv()
		{
			ttab_map* ttab = allcsv::get<ttab_map>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_map* tab(int32_t aid)
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
