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

namespace ngl
{
	class bi
	{
		template <typename T>
		static void add(std::string& astr, const T& avalue)
		{
			if (!astr.empty())
			{
				astr += '|';
			}
			astr += tools::lexical_cast<std::string>(avalue);
		}

		template <typename T, typename ...ARGS>
		static void add(std::string& astr, const T& avalue, const ARGS&... args)
		{
			push(astr, avalue);
			push(astr, args...);
		}

	public:
		template <typename T, typename ...ARGS>
		static std::string push(const T& avalue, const ARGS&... args)
		{
			std::string lstr;
			push(lstr, args...);
			return lstr;
		}
	};

	struct bi_item
	{
		i32_serverid	m_serverid = 0;
		i64_actorid		m_roleid = 0;
		int32_t			m_itemid = 0;
		int32_t			m_tid = 0;
		int32_t			m_count = 0;
		int32_t			m_time = 0;
		int32_t			m_adddec = 0;// 1:增加 2:减少
		std::string		m_mask;

		void print()
		{
			log_bi()->print("item|{}", bi::push(m_serverid, m_roleid, m_itemid, m_tid, m_count, m_time, m_adddec, m_mask));
		}
	};


}