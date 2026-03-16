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
	struct ttab_equiplv :
		public csv<tab_equiplv>
	{
		ttab_equiplv(const ttab_equiplv&) = delete;
		ttab_equiplv& operator=(const ttab_equiplv&) = delete;

		void reload()final
		{
			std::cout << "[ttab_equiplv] reload" << std::endl;
		}
	public:
		using type_tab = tab_equiplv;

		ttab_equiplv() = default;

		static ttab_equiplv& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_equiplv>();
			}
			return *ncsv::get<ttab_equiplv>();
		}

		// # std::map<int, tab_equiplv>& tabs()
		// # tab_equiplv* tab(int aid)
	};
}//namespace ngl