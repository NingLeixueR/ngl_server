#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_mail :
		public manage_csv<tab_mail>
	{
		ttab_mail(const ttab_mail&) = delete;
		ttab_mail& operator=(const ttab_mail&) = delete;

		ttab_mail()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_mail] reload" << std::endl;
		}

	public:
		using type_tab = tab_mail;

		static ttab_mail& instance()
		{
			static ttab_mail ltemp;
			return ltemp;
		}

		const std::map<int, tab_mail>& tablecsv()
		{
			ttab_mail* ttab = allcsv::get<ttab_mail>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_mail* tab(int32_t aid)
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
