/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "ncsv.h"
#include "type.h"
#include "xml.h"

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
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_activity_toprank>();
			}
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