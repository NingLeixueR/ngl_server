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
// File overview: Declares interfaces for tools.


#pragma once

#include "tools/tools/tools_thread.h"
#include "tools/log/nlog.h"
#include "tools/tools.h"

#include <limits>
#include <map>

namespace ngl::tools
{
	struct nhashcode
	{
		int64_t m_hashcode = 0; // Raw typeid(T).hash_code().
		int32_t m_index = 0;    // Collision-disambiguation index for identical hash_code() values.

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
		static std::map<int64_t, std::map<std::string, int32_t>> m_hash; // hash_code -> type name -> stable index.
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
					// Reuse the previously assigned collision index for this exact type name.
					return nhashcode(lcode, itor->second);
				}
				const auto lnext = lmap.size() + 1;
				if (lnext > static_cast<std::size_t>(std::numeric_limits<int32_t>::max()))
				{
					log_error()->print("nhash::code index overflow [{}]", lnext);
					tools::no_core_dump();
					return nhashcode(lcode, 0);
				}
				lindex = static_cast<int32_t>(lnext);
				lmap[tools::type_name<T>()] = lindex;
			}
			return nhashcode(lcode, lindex);
		}
	};
}//namespace ngl
