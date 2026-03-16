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
	struct ttab_activity_drawcompliance :
		public csv<tab_activity_drawcompliance>
	{
		ttab_activity_drawcompliance(const ttab_activity_drawcompliance&) = delete;
		ttab_activity_drawcompliance& operator=(const ttab_activity_drawcompliance&) = delete;

		void reload()final
		{
			std::cout << "[ttab_activity_drawcompliance] reload" << std::endl;
		}
	public:
		using type_tab = tab_activity_drawcompliance;

		ttab_activity_drawcompliance() = default;

		static ttab_activity_drawcompliance& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_activity_drawcompliance>();
			}
			return *ncsv::get<ttab_activity_drawcompliance>();
		}

		// # std::map<int, tab_activity_drawcompliance>& tabs()
		// # tab_activity_drawcompliance* tab(int aid)
	};
}//namespace ngl