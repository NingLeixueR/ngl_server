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
// File overview: Group/batch operation helpers for partitioning work across containers.

#pragma once

#include "actor/protocol/nprotocol.h"

#include <map>
#include <set>

namespace ngl::tools
{
	class group
	{
		std::map<int, std::set<i64_actorid>>		m_group;
		int											m_currentoffset = 0;
	public:
		inline group() :
			m_currentoffset(0)
		{}

		// Create and return a new empty group id.
		inline int create([[maybe_unused]] ENUM_ACTOR atype = ACTOR_NONE)
		{
			auto& linfo = m_group[++m_currentoffset];
			linfo.clear();
			return m_currentoffset;
		}

		// Remove a group and all of its members.
		inline void remove(int agroupid)
		{
			m_group.erase(agroupid);
		}

		// Add one actor id into an existing group.
		inline bool add_member(int agroupid, i64_actorid amember)
		{
			std::set<i64_actorid>* lsets = tools::findmap(m_group, agroupid);
			if (lsets == nullptr)
			{
				log_error()->print("add_group_member not find groupid[{}]", agroupid);
				return false;
			}			
			lsets->insert(amember);
			return true;
		}

		// Remove one actor id from an existing group.
		inline void remove_member(int agroupid, i64_actorid amember)
		{
			std::set<i64_actorid>* lsets = tools::findmap(m_group, agroupid);
			if (lsets == nullptr)
			{
				return;
			}
			lsets->erase(amember);
		}

		// Return the full member set for one group.
		inline const std::set<i64_actorid>* get_group(int agroupid)
		{
			std::set<i64_actorid>* lsets = tools::findmap(m_group, agroupid);
			if (lsets == nullptr)
			{
				return nullptr;
			}
			return lsets;
		}
	};
}//namespace ngl::tools
