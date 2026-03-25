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
		for (const auto& litem : tab->m_consumeitems)
		{
			amap[litem.m_tid] += litem.m_count * acount;
		}
		return true;
	}

	bool consume::check(actor_role* arole, std::map<int32_t, int32_t>& amap)
	{
		for (const auto& [ltid, lcount] : amap)
		{
			if (arole->m_bag.checkbytid(ltid, lcount) == false)
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
		for (const auto& [ltid, lcount] : lmap)
		{
			arole->m_bag.add_item(ltid, lcount);
		}
		return true;
	}
}// namespace ngl
