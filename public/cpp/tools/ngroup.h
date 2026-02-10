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

#include "nprotocol.h"

#include <map>
#include <set>

namespace ngl
{
	class ngroup
	{
		std::map<int, std::set<i64_actorid>>		m_group;
		int											m_currentoffset = 0;
	public:
		inline ngroup() :
			m_currentoffset(0)
		{}

		// # 创建一个分组
		inline int create(ENUM_ACTOR atype = ACTOR_NONE)
		{
			auto& linfo = m_group[++m_currentoffset];
			linfo.clear();
			return m_currentoffset;
		}

		// # 移除一个分组
		inline void remove(int agroupid)
		{
			m_group.erase(agroupid);
		}

		// # 在分组中添加一个成员
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

		// # 在分组中移除一个成员
		inline void remove_member(int agroupid, i64_actorid amember)
		{
			std::set<i64_actorid>* lsets = tools::findmap(m_group, agroupid);
			if (lsets == nullptr)
			{
				return;
			}
			lsets->erase(amember);
		}

		// # 获取分组中的所有成员
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
}//namespace ngl