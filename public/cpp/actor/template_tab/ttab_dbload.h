#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_dbload :
		public manage_csv<tab_dbload>
	{
		ttab_dbload(const ttab_dbload&) = delete;
		ttab_dbload& operator=(const ttab_dbload&) = delete;

		std::map<std::string, tab_dbload*> m_name2data;

		ttab_dbload()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_dbload] reload" << std::endl;
			for (std::pair<const int, tab_dbload>& ipair : m_tablecsv)
			{
				tab_dbload& tab = ipair.second;
				m_name2data[tab.m_name] = &tab;
			}
		}

	public:
		using type_tab = tab_dbload;

		static ttab_dbload& instance()
		{
			static ttab_dbload ltemp;
			return ltemp;
		}

		std::map<int, tab_dbload>& tablecsv()
		{
			ttab_dbload* ttab = allcsv::get<ttab_dbload>();
			if (ttab == nullptr);
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_dbload* tab(int32_t aid)
		{
			auto lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		template <typename T>
		tab_dbload* get_tabdb()
		{
			std::string lname = T().descriptor()->full_name();
			ngl::tools::replace("pbdb.", "", lname, lname);
			std::ranges::transform(lname, lname.begin(), toupper);

			tab_dbload** tab = tools::findmap(m_name2data, lname);
			if (tab == nullptr)
			{
				return nullptr;
			}
			return *tab;
		}
	};
}//namespace ngl
