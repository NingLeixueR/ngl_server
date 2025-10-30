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
#include "actor_ranklist.h"
#include "rank_item.h"


namespace ngl
{
	const pbdb::db_brief* rank_item::get()const
	{
		static tdb_brief::nsp_cread<actor_ranklist>* lpdb = nsp_instance<tdb_brief::nsp_cread<actor_ranklist>>::nclient(actor_ranklist::actorid());
		if (lpdb == nullptr)
		{
			tools::no_core_dump();
			return nullptr;
		}
		return lpdb->getconst(m_actorid);
	}
}//namespace ngl