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
	struct ttab_activity_toprank :
		public csv<tab_activity_toprank>
	{
		ttab_activity_toprank(const ttab_activity_toprank&) = delete;
		ttab_activity_toprank& operator=(const ttab_activity_toprank&) = delete;

		// key:activityid //value.key:rank 
		std::map<int32_t, std::map<int32_t, const tab_activity_toprank*>> m_activityrank;

		void reload()final
		{
			std::cout << "[ttab_activity_toprank] reload" << std::endl;
			foreach([&](tab_activity_toprank& atab)
				{
					m_activityrank[atab.m_activityid][atab.m_rank] = &atab;
				}
			);
		}
	public:
		using type_tab = tab_activity_toprank;

		ttab_activity_toprank() = default;

		static ttab_activity_toprank& instance()
		{
			static std::once_flag lfirst;
			std::call_once(lfirst, []()
				{
					ncsv::loadcsv<ttab_activity_toprank>();
				}
			);
			return *ncsv::get<ttab_activity_toprank>();
		}

		// # std::map<int, tab_activity_toprank>& tabs()
		// # tab_activity_toprank* tab(int aid)

		bool rankreward(int32_t aactivityid, int32_t arank, int32_t& amailid, int32_t& areward)
		{
			auto lprankmap = tools::findmap(m_activityrank, aactivityid);
			if (lprankmap == nullptr)
			{
				return false;
			}
			auto reward_it = lprankmap->lower_bound(arank);
			if (reward_it == lprankmap->end())
			{
				return false;
			}
			amailid = reward_it->second->m_mailid;
			areward = reward_it->second->m_dropid;
			return true;
		}
	};
}//namespace ngl
