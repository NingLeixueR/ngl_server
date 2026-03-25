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
// File overview: Implements logic for bag.


#include "actor/actor_logic/actor_role/logic/bag/autoitem.h"
#include "actor/actor_logic/actor_role/actor_role.h"

namespace ngl
{
	void autoitem::add(int32_t atid, int32_t acount)
	{
		m_addstackitems[atid] += acount;
	}

	void autoitem::add(int32_t aid)
	{
		m_addnostackitems.emplace_back(aid);
	}

	void autoitem::del(int32_t aid, int32_t acount)
	{
		m_delstackitems[aid] += acount;
	}

	void autoitem::del(int32_t aid)
	{
		m_addnostackitems.emplace_back(aid);
	}

	bool autoitem::empty()
	{
		return m_addstackitems.empty() && m_addnostackitems.empty() && m_delstackitems.empty() && m_delnostackitems.empty();
	}

	void autoitem::clear()
	{
		m_addstackitems.clear();
		m_addnostackitems.clear();
		m_delstackitems.clear();
		m_delnostackitems.clear();
	}

	const std::map<int32_t, int32_t>& autoitem::delstackitems()
	{
		return m_delstackitems;
	}

	const std::list<int32_t>& autoitem::delnostackitems()
	{
		return m_delnostackitems;
	}

	const std::map<int32_t, int32_t>& autoitem::addstackitems()
	{
		return m_addstackitems;
	}

	const std::list<int32_t>& autoitem::addnostackitems()
	{
		return m_addnostackitems;
	}
}// namespace ngl
