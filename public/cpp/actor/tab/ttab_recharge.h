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
	struct ttab_recharge :
		public csv<tab_recharge>
	{
		ttab_recharge(const ttab_recharge&) = delete;
		ttab_recharge& operator=(const ttab_recharge&) = delete;

		void reload()final
		{
			std::cout << "[ttab_recharge] reload" << std::endl;
		}
	public:
		using type_tab = tab_recharge;

		ttab_recharge() = default;

		static ttab_recharge& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_recharge>();
			}
			return *ncsv::get<ttab_recharge>();
		}

		// # std::map<int, tab_recharge>& tabs()
		// # tab_recharge* tab(int aid)
	};
}//namespace ngl