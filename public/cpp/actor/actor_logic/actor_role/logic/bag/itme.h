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
#pragma once

#include "manage_csv.h"
#include "db.pb.h"

namespace ngl
{
	class actor_role;
	
	class item_create
	{
		static std::map<EItemType, item_create*> m_data;
	protected:
		item_create(EItemType atype)
		{
			m_data[atype] = this;
		}

		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec) = 0;
	public:
		static bool create(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			tab_item* tab = allcsv::tab<tab_item>(atid);
			if (tab == nullptr)
			{
				return false;
			}
			auto itor = m_data.find((EItemType)tab->m_type);
			if (itor == m_data.end())
			{
				return false;
			}
			return itor->second->mode(arole, atid, acount, avec);
		}
	};
}// namespace ngl