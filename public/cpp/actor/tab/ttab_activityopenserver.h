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
	struct ttab_activityopenserver :
		public csv<tab_activityopenserver>
	{
		ttab_activityopenserver(const ttab_activityopenserver&) = delete;
		ttab_activityopenserver& operator=(const ttab_activityopenserver&) = delete;

		void reload()final
		{
			std::cout << "[ttab_activityopenserver] reload" << std::endl;
		}
	public:
		using type_tab = tab_activityopenserver;

		ttab_activityopenserver() = default;

		static ttab_activityopenserver& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_activityopenserver>();
			}
			return *ncsv::get<ttab_activityopenserver>();
		}

		// # std::map<int, tab_activityopenserver>& tabs()
		// # tab_activityopenserver* tab(int aid)
	};
}//namespace ngl