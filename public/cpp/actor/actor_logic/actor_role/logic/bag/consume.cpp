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
#include "ttab_consume.h"
#include "consume.h"
#include "ncsv.h"
#include "bag.h"

namespace ngl
{
	bool consume::get(int atid, int acount, std::map<int32_t, int32_t>& amap)
	{
		tab_consume* tab = ttab_consume::instance().tab(atid);
		if (tab == nullptr)
		{
			return false;
		}
		for (int i = 0; i < tab->m_consumeitems.size(); ++i)
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