/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

		const std::map<int, tab_dbload>* tablecsv()
		{
			ttab_dbload* ttab = allcsv::get<ttab_dbload>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
				return nullptr;
			}
			return &ttab->m_tablecsv;
		}

		const tab_dbload* tab(int32_t aid)
		{
			auto lpmap = tablecsv();
			if (lpmap == nullptr)
			{
				return nullptr;
			}
			auto itor = lpmap->find(aid);
			if (itor == lpmap->end())
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
