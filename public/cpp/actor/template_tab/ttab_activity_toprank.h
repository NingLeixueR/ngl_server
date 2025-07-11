#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_activity_toprank :
		public manage_csv<tab_activity_toprank>
	{
		ttab_activity_toprank(const ttab_activity_toprank&) = delete;
		ttab_activity_toprank& operator=(const ttab_activity_toprank&) = delete;

		// key:activityid //value.key:rank 
		std::map<int32_t, std::map<int32_t, const tab_activity_toprank*>> m_activityrank;

		ttab_activity_toprank()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_activity_toprank] reload" << std::endl;
			for (std::pair<const int, tab_activity_toprank>& pair : m_tablecsv)
			{
				m_activityrank[pair.second.m_activityid][pair.second.m_rank] = &pair.second;
			}
		}

	public:
		using type_tab = tab_activity_toprank;

		static ttab_activity_toprank& instance()
		{
			static ttab_activity_toprank ltemp;
			return ltemp;
		}

		std::map<int, tab_activity_toprank>& tablecsv()
		{
			ttab_activity_toprank* ttab = allcsv::get<ttab_activity_toprank>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_activity_toprank* tab(int32_t aid)
		{
			auto lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

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
