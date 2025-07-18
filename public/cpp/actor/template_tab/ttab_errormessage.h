#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_errormessage :
		public manage_csv<tab_errormessage>
	{
		ttab_errormessage(const ttab_errormessage&) = delete;
		ttab_errormessage& operator=(const ttab_errormessage&) = delete;

		ttab_errormessage()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_errormessage] reload" << std::endl;
		}

	public:
		using type_tab = tab_errormessage;

		static ttab_errormessage& instance()
		{
			static ttab_errormessage ltemp;
			return ltemp;
		}

		const std::map<int, tab_errormessage>& tablecsv()
		{
			ttab_errormessage* ttab = allcsv::get<ttab_errormessage>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_errormessage* tab(int32_t aid)
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
