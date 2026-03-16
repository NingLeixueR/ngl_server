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
#include "tools/nfilterword.h"
#include "tools/type.h"

namespace ngl
{
	struct ttab_filterword :
		public csv<tab_filterword>
	{
		ttab_filterword(const ttab_filterword&) = delete;
		ttab_filterword& operator=(const ttab_filterword&) = delete;

		void reload()final
		{
			std::cout << "[ttab_filterword] reload" << std::endl;
			nfilterword<std::string>::instance().clear();
			nfilterword<std::string>::instance().init();
			foreach([](tab_filterword& atab)
				{
					nfilterword<std::string>::instance().load(atab.m_content);
				}
			);
			nfilterword<std::string>::instance().build();
		}
	public:
		using type_tab = tab_filterword;

		ttab_filterword() = default;

		static ttab_filterword& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_filterword>();
			}
			return *ncsv::get<ttab_filterword>();
		}

		// # std::map<int, tab_filterword>& tabs()
		// # tab_filterword* tab(int aid)
	};
}//namespace ngl