#pragma once

#include "manage_csv.h"

namespace ngl
{
	struct ttab_dbload : public manage_csv<tab_dbload>
	{
		ttab_dbload(const ttab_dbload&) = delete;
		ttab_dbload& operator=(const ttab_dbload&) = delete;

		static std::map<std::string, tab_dbload*> m_name2data;
		static tab_dbload m_universalize;

		ttab_dbload()
		{}

		void reload()final
		{
			for (std::pair<const int, tab_dbload>& ipair : m_tablecsv)
			{
				tab_dbload& tab = ipair.second;
				m_name2data[tab.m_name] = &tab;
			}

			m_universalize.m_id = 0x7fffffff;
			m_universalize.m_name = "db_universalize";
			m_universalize.m_isloadall = true;
			m_universalize.m_network = true;
			m_universalize.m_sendmaxcount = 100;
			m_universalize.m_dbcacheintervalms = 5000;
		}

		template <typename T>
		static tab_dbload* get_tabdb()
		{
			std::string lname = T().descriptor()->full_name();
			ngl::tools::replace("pbdb.", "", lname, lname);
			std::ranges::transform(lname, lname.begin(), toupper);

			tab_dbload** tab = tools::findmap(m_name2data, lname);
			if (tab == nullptr)
			{
				return &m_universalize;
			}
			return *tab;
		}
	};
}// namespace ngl