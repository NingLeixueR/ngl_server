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
	struct ttab_familylv :
		public csv<tab_familylv>
	{
		ttab_familylv(const ttab_familylv&) = delete;
		ttab_familylv& operator=(const ttab_familylv&) = delete;

		std::map<int32_t, int32_t> m_failylvexp;		// key:lv value:exp
		std::map<int32_t, int32_t> m_failyrolecount;	// key:lv value:rolecount

		void reload()final
		{
			std::cout << "[ttab_familylv] reload" << std::endl;
			m_failylvexp.clear();
			m_failyrolecount.clear();

			foreach([&](tab_familylv& atab)
				{
					m_failylvexp[atab.m_id] = atab.m_exp;
					m_failyrolecount[atab.m_id] = atab.m_maxmembers;
				}
			);
		}
	public:
		using type_tab = tab_familylv;

		ttab_familylv() = default;

		static ttab_familylv& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_familylv>();
			}
			return *ncsv::get<ttab_familylv>();
		}

		// # std::map<int, tab_familylv>& tabs()
		// # tab_familylv* tab(int aid)

		// Lvget need to
		int32_t* failylvexp(int32_t alv)
		{
			return tools::findmap(m_failylvexp, alv);
		}

		// Lvget
		int32_t* failylvmaxcount(int32_t alv)
		{
			return tools::findmap(m_failyrolecount, alv);
		}
	};
}//namespace ngl