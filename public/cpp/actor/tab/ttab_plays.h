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
	struct ttab_plays :
		public csv<tab_plays>
	{
		ttab_plays(const ttab_plays&) = delete;
		ttab_plays& operator=(const ttab_plays&) = delete;

		void reload()final
		{
			std::cout << "[ttab_plays] reload" << std::endl;
		}
	public:
		using type_tab = tab_plays;

		ttab_plays() = default;

		static ttab_plays& instance()
		{
			static std::once_flag lfirst;
			std::call_once(lfirst, []()
				{
					ncsv::loadcsv<ttab_plays>();
				}
			);
			return *ncsv::get<ttab_plays>();
		}

		// # std::map<int, tab_plays>& tabs()
		// # tab_plays* tab(int aid)
	};
}//namespace ngl
