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
	struct ttab_consume :
		public csv<tab_consume>
	{
		ttab_consume(const ttab_consume&) = delete;
		ttab_consume& operator=(const ttab_consume&) = delete;

		void reload()final
		{
			std::cout << "[ttab_consume] reload" << std::endl;
		}
	public:
		using type_tab = tab_consume;

		ttab_consume() = default;

		static ttab_consume& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_consume>();
			}
			return *ncsv::get<ttab_consume>();
		}

		// # std::map<int, tab_consume>& tabs()
		// # tab_consume* tab(int aid)
	};
}//namespace ngl