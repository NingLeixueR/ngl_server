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
	struct ttab_mail :
		public csv<tab_mail>
	{
		ttab_mail(const ttab_mail&) = delete;
		ttab_mail& operator=(const ttab_mail&) = delete;

		void reload()final
		{
			std::cout << "[ttab_mail] reload" << std::endl;
		}
	public:
		using type_tab = tab_mail;

		ttab_mail() = default;

		static ttab_mail& instance()
		{
			static std::once_flag lfirst;
			std::call_once(lfirst, []()
				{
					ncsv::loadcsv<ttab_mail>();
				}
			);
			return *ncsv::get<ttab_mail>();
		}

		// # std::map<int, tab_mail>& tabs()
		// # tab_mail* tab(int aid)
	};
}//namespace ngl
