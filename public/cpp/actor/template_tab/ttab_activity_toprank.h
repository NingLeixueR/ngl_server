#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_activity_toprank :
		public manage_csv<tab_activity_toprank>
	{
		ttab_activity_toprank(const ttab_activity_toprank&) = delete;
		ttab_activity_toprank& operator=(const ttab_activity_toprank&) = delete;
		using type_tab = tab_activity_toprank;

		// key:activityid //value.key:rank 
		static std::map<int32_t, std::map<int32_t, const tab_activity_toprank*>> m_activityrank;

		ttab_activity_toprank()
		{}

		static const std::map<int, tab_activity_toprank>& tablecsv()
		{
			const ttab_activity_toprank* ttab = allcsv::get<ttab_activity_toprank>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_activity_toprank* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		void reload()final
		{
			for (std::pair<const int, tab_activity_toprank>& pair : m_tablecsv)
			{
				m_activityrank[pair.second.m_activityid][pair.second.m_rank] = &pair.second;
			}
		}

		static bool rankreward(int32_t aactivityid, int32_t arank, int32_t& amailid, int32_t& areward)
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
