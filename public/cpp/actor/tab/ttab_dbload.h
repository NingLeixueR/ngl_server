/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for tab.

#pragma once

#include "tools/tab/csv/ncsv.h"
#include "tools/tab/xml/xml.h"
#include "tools/type.h"

namespace ngl
{
	struct ttab_dbload :
		public csv<tab_dbload>
	{
		ttab_dbload(const ttab_dbload&) = delete;
		ttab_dbload& operator=(const ttab_dbload&) = delete;

		std::map<std::string, tab_dbload*> m_name2data;

		void reload()final
		{
			std::cout << "[ttab_dbload] reload" << std::endl;
			foreach([&](tab_dbload& atab)
				{
					std::string lname = atab.m_name;
					tools::transform_tolower(lname);
					m_name2data[lname] = &atab;
				}
			);
		}
	public:
		using type_tab = tab_dbload;

		ttab_dbload() = default;

		static ttab_dbload& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_dbload>();
			}
			return *ncsv::get<ttab_dbload>();
		}

		// # std::map<int, tab_dbload>& tabs()
		// # tab_dbload* tab(int aid)

		template <typename T>
		tab_dbload* get_tabdb()
		{
			tab_dbload** tab = tools::findmap(m_name2data, tools::type_name<T>());
			if (tab == nullptr)
			{
				return nullptr;
			}
			return *tab;
		}
	};
}//namespace ngl