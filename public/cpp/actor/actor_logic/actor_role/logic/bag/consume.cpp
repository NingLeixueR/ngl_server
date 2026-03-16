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


#include "actor/actor_logic/actor_role/logic/bag/consume.h"
#include "actor/actor_logic/actor_role/logic/bag/bag.h"
#include "actor/tab/ttab_consume.h"
#include "tools/tab/csv/ncsv.h"

namespace ngl
{
	bool consume::get(int atid, int acount, std::map<int32_t, int32_t>& amap)
	{
		tab_consume* tab = ttab_consume::instance().tab(atid);
		if (tab == nullptr)
		{
			return false;
		}
		for (std::size_t i = 0; i < tab->m_consumeitems.size(); ++i)
		{
			amap[tab->m_consumeitems[i].m_tid] += tab->m_consumeitems[i].m_count * acount;
		}
		return true;
	}

	bool consume::check(actor_role* arole, std::map<int32_t, int32_t>& amap)
	{
		for (auto [tid, count] : amap)
		{
			if (arole->m_bag.checkbytid(tid, count) == false)
			{
				return false;
			}
		}		
		return true;		
	}

	bool consume::check(actor_role* arole, int atid, int acount)
	{
		std::map<int32_t, int32_t> lmap;
		if (get(atid, acount, lmap) == false)
		{
			return false;
		}
		if (check(arole, lmap) == false)
		{
			return false;
		}
		return true;
	}

	bool consume::use(actor_role* arole, int aid, int acount)
	{
		std::map<int32_t, int32_t> lmap;
		if (get(aid, acount, lmap) == false)
		{
			return false;
		}
		if (check(arole, lmap) == false)
		{
			return false;
		}
		for (auto [tid, count] : lmap)
		{
			arole->m_bag.add_item(tid, count);
		}
		return true;
	}
}// namespace ngl