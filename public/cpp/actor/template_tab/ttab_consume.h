#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_consume :
		public manage_csv<tab_consume>
	{
		ttab_consume(const ttab_consume&) = delete;
		ttab_consume& operator=(const ttab_consume&) = delete;

		ttab_consume()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_consume] reload" << std::endl;
		}

	public:
		using type_tab = tab_consume;

		static ttab_consume& instance()
		{
			static ttab_consume ltemp;
			return ltemp;
		}

		const std::map<int, tab_consume>& tablecsv()
		{
			ttab_consume* ttab = allcsv::get<ttab_consume>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_consume* tab(int32_t aid)
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
