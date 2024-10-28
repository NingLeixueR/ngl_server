#pragma once

#include "nprotocol.h"

#include <map>
#include <set>

namespace ngl
{
	template <typename TMEMBER>
	class ngroup
	{
		struct ginfo
		{
			std::set<TMEMBER>	m_actorlist;
		};
		std::map<int, ginfo>		m_group;
		int							m_currentoffset;
	public:
		ngroup() :
			m_currentoffset(0)
		{}

		// # 创建一个分组
		inline int create_group()
		{
			ginfo& linfo = m_group[++m_currentoffset];
			linfo.m_actorlist.clear();
			return m_currentoffset;
		}

		// # 移除一个分组
		inline void remove_group(int agroupid)
		{
			m_group.erase(agroupid);
		}

		inline bool add_group_member(int agroupid, const TMEMBER& amember)
		{
			ginfo* lginfo = tools::findmap(m_group, agroupid);
			if (lginfo == nullptr)
			{
				log_error()->print(
					"add_group_member not find groupid[{}]", agroupid
				);
				return false;
			}
			lginfo->m_actorlist.insert(amember);
			return true;
		}

		inline void remove_group_member(int agroupid, const TMEMBER& amember)
		{
			ginfo* lginfo = tools::findmap(m_group, agroupid);
			if (lginfo == nullptr)
				return;
			lginfo->m_actorlist.erase(amember);
		}

		inline const std::set<i64_actorid>* get_group(int agroupid)
		{
			ginfo* lginfo = tools::findmap(m_group, agroupid);
			if (lginfo == nullptr)
				return nullptr;
			return &lginfo->m_actorlist;
		}
	};
}//namespace ngl