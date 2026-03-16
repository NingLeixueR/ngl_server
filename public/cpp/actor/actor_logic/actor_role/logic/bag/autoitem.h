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
// File overview: Declares interfaces for bag.

#pragma once

#include "tools/type.h"

#include <vector>
#include <list>
#include <map>

namespace ngl
{
	class actor_role;
	
	class autoitem
	{
		std::map<int32_t, int32_t>	m_delstackitems;
		std::list<int32_t>			m_delnostackitems;
		std::map<int32_t, int32_t>	m_addstackitems;
		std::list<int32_t>			m_addnostackitems;
	public:
		void add(int32_t atid, int32_t acount);
		void add(int32_t aid);
		
		void del(int32_t aid, int32_t acount);
		void del(int32_t aid);

		const std::map<int32_t, int32_t>& delstackitems();
		const std::list<int32_t>& delnostackitems();

		const std::map<int32_t, int32_t>& addstackitems();
		const std::list<int32_t>& addnostackitems();
		
		bool empty();
		
		void clear();

	};
}// namespace ngl