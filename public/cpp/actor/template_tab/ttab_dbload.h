#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_dbload :
		public manage_csv<tab_dbload>
	{
		ttab_dbload(const ttab_dbload&) = delete;
		ttab_dbload& operator=(const ttab_dbload&) = delete;
		using type_tab = tab_dbload;

		static std::map<std::string, tab_dbload*> m_name2data;

		ttab_dbload()
		{}

		static const std::map<int, tab_dbload>& tablecsv()
		{
			const ttab_dbload* ttab = allcsv::get<ttab_dbload>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_dbload* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		void reload()final
		{
			for (std::pair<const int, tab_dbload>& ipair : m_tablecsv)
			{
				tab_dbload& tab = ipair.second;
				m_name2data[tab.m_name] = &tab;
			}
		}

		template <typename T>
		static tab_dbload* get_tabdb()
		{
			allcsv::loadcsv<ttab_dbload>();
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
