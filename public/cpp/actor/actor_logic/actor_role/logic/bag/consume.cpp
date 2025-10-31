/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "manage_csv.h"
#include "consume.h"
#include "bag.h"

namespace ngl
{
	bool consume::get(int atid, int acount, std::map<int32_t, int32_t>& amap)
	{
		tab_consume* tab = allcsv::tab<tab_consume>(atid);
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
		for (const auto& [tid, count] : amap)
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
		for (const auto& [tid, count] : lmap)
		{
			arole->m_bag.add_item(tid, count);
		}
		return true;
	}
}// namespace ngl