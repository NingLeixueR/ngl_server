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
	struct ttab_card :
		public csv<tab_card>
	{
		ttab_card(const ttab_card&) = delete;
		ttab_card& operator=(const ttab_card&) = delete;

		void reload()final
		{
			std::cout << "[ttab_card] reload" << std::endl;
		}
	public:
		using type_tab = tab_card;

		ttab_card() = default;

		static ttab_card& instance()
		{
			static std::once_flag lfirst;
			std::call_once(lfirst, []()
				{
					ncsv::loadcsv<ttab_card>();
				}
			);
			return *ncsv::get<ttab_card>();
		}

		// # std::map<int, tab_card>& tabs()
		// # tab_card* tab(int aid)
	};
}//namespace ngl
