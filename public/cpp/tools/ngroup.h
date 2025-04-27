#pragma once

#include "nprotocol.h"

#include <map>
#include <set>

namespace ngl
{
	class ngroup
	{
		struct info
		{
			ENUM_ACTOR m_type;
			std::set<i64_actorid> m_actorlist;
		};
		std::map<int, info>				m_group;
		int								m_currentoffset;
	public:
		inline ngroup() :
			m_currentoffset(0)
		{}

		// # 创建一个分组
		inline int create(ENUM_ACTOR atype = ACTOR_NONE)
		{
			auto& linfo = m_group[++m_currentoffset];
			linfo.m_actorlist.clear();
			linfo.m_type = atype;
			return m_currentoffset;
		}

		// # 移除一个分组
		inline void remove(int agroupid)
		{
			m_group.erase(agroupid);
		}

		inline bool add_member(int agroupid, i64_actorid amember)
		{
			info* linfo = tools::findmap(m_group, agroupid);
			if (linfo == nullptr)
			{
				log_error()->print("add_group_member not find groupid[{}]", agroupid);
				return false;
			}
			if (linfo->m_type != ACTOR_NONE)
			{
				amember = nguid::make_type(amember, linfo->m_type);
			}
			linfo->m_actorlist.insert(amember);
			return true;
		}

		inline void remove_member(int agroupid, i64_actorid amember)
		{
			info* linfo = tools::findmap(m_group, agroupid);
			if (linfo == nullptr)
			{
				return;
			}
			if (linfo->m_type != ACTOR_NONE)
			{
				amember = nguid::make_type(amember, linfo->m_type);
			}
			linfo->m_actorlist.erase(amember);
		}

		inline const std::set<i64_actorid>* get_group(int agroupid)
		{
			info* linfo = tools::findmap(m_group, agroupid);
			if (linfo == nullptr)
			{
				return nullptr;
			}
			return &linfo->m_actorlist;
		}
	};
}//namespace ngl