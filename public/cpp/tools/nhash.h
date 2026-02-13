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

#include "threadtools.h"
#include "tools.h"

#include <map>

namespace ngl
{
	struct nhashcode
	{
		int64_t m_hashcode = 0;
		int32_t m_index = 0;

		nhashcode(int64_t ahashcode, int32_t aindex) :
			m_hashcode(ahashcode),
			m_index(aindex)
		{}

		auto operator<=>(const nhashcode& ar)const
		{
			return tools::less_member(m_hashcode, ar.m_hashcode, m_index, ar.m_index);
		}
	};

	class nhash
	{
		static std::map<int64_t, std::map<std::string, int32_t>> m_hash;
		static std::shared_mutex m_mutex;
	public:
		template <typename T>
		static nhashcode code()
		{
			static int64_t lcode = typeid(T).hash_code();
			int32_t lindex = 0;
			{
				lock_write(m_mutex);
				std::map<std::string, int32_t>& lmap = m_hash[lcode];
				auto itor = lmap.find(tools::type_name<T>());
				if (itor != lmap.end())
				{
					return nhashcode(lcode, itor->second);
				}
				lindex = lmap.size() + 1;
				lmap[tools::type_name<T>()] = lindex;
			}
			return nhashcode(lcode, lindex);
		}
	};
}//namespace ngl