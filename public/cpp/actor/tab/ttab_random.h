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
#include "tools/log/nlog.h"
#include "tools/log/nlog.h"
#include "tools/type.h"

namespace ngl
{
	struct ttab_random :
		public csv<tab_random>
	{
		ttab_random(const ttab_random&) = delete;
		ttab_random& operator=(const ttab_random&) = delete;

		void reload()final
		{
			std::cout << "[ttab_random] reload" << std::endl;
			// ## All whether
			foreach([&](tab_random& atab)
				{
					std::set<int32_t> lset;
					if (is_loop(atab.m_id, lset))
					{
						tools::no_core_dump();
					}
				});
		}
	public:
		using type_tab = tab_random;

		ttab_random() = default;

		static ttab_random& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_random>();
			}
			return *ncsv::get<ttab_random>();
		}

		// # std::map<int, tab_random>& tabs()
		// # tab_random* tab(int aid)

		bool is_loop(int32_t aid, std::set<int32_t>& aset)
		{
			const tab_random* ltab = tab(aid);
			if (ltab == nullptr)
			{
				return false;
			}
			if (aset.insert(ltab->m_id).second == false)
			{
				return false;
			}
			for (int32_t id : ltab->m_childrandomids)
			{
				if (is_loop(id, aset) == false)
				{
					return false;
				}
			}
			return true;
		}
	};
}//namespace ngl