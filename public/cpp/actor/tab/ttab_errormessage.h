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
	struct ttab_errormessage :
		public csv<tab_errormessage>
	{
		ttab_errormessage(const ttab_errormessage&) = delete;
		ttab_errormessage& operator=(const ttab_errormessage&) = delete;

		void reload()final
		{
			std::cout << "[ttab_errormessage] reload" << std::endl;
		}
	public:
		using type_tab = tab_errormessage;

		ttab_errormessage() = default;

		static ttab_errormessage& instance()
		{
			static std::once_flag lfirst;
			std::call_once(lfirst, []()
				{
					ncsv::loadcsv<ttab_errormessage>();
				}
			);
			return *ncsv::get<ttab_errormessage>();
		}

		// # std::map<int, tab_errormessage>& tabs()
		// # tab_errormessage* tab(int aid)
	};
}//namespace ngl
