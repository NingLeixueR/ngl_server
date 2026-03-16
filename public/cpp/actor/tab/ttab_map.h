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
	struct ttab_map :
		public csv<tab_map>
	{
		ttab_map(const ttab_map&) = delete;
		ttab_map& operator=(const ttab_map&) = delete;

		void reload()final
		{
			std::cout << "[ttab_map] reload" << std::endl;
		}
	public:
		using type_tab = tab_map;

		ttab_map() = default;

		static ttab_map& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_map>();
			}
			return *ncsv::get<ttab_map>();
		}

		// # std::map<int, tab_map>& tabs()
		// # tab_map* tab(int aid)
	};
}//namespace ngl