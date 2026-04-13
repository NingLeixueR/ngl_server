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
	struct ttab_activity :
		public csv<tab_activity>
	{
		ttab_activity(const ttab_activity&) = delete;
		ttab_activity& operator=(const ttab_activity&) = delete;

		void reload()final
		{
			std::cout << "[ttab_activity] reload" << std::endl;
		}
	public:
		using type_tab = tab_activity;

		ttab_activity() = default;

		static ttab_activity& instance()
		{
			static std::once_flag lfirst;
			std::call_once(lfirst, []()
				{
					ncsv::loadcsv<ttab_activity>();
				}
			);	
			return *ncsv::get<ttab_activity>();
		}

		// # std::map<int, tab_activity>& tabs()
		// # tab_activity* tab(int aid)
	};
}//namespace ngl
