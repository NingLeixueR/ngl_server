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
#include "actor_csvserver.h"
namespace ngl
{
	bool actor_csvserver::handle(const message<np_actor_csv_verify_version>& adata)
	{
		log_error()->print("############actor_reloadcsv_distribute::handle###########");
		auto lparm = adata.get_data();
		auto lpack = adata.get_pack();
		np_actor_reloadcsv pro;
		const auto& lversion = allcsv::all();
		for (const auto& apair : lversion)
		{
			auto itor = lparm->m_verify.find(apair.first);
			if (itor != lparm->m_verify.end() && itor->second != apair.second->verify())
			{
				continue;
			}
			reload_csv::readcsv(apair.first, pro.m_csvcontent[apair.first]);
		}
		if (pro.m_csvcontent.empty() == false)
		{
			send(lpack->m_id, pro, lpack->m_head.get_request_actor(), id_guid());
		}
		return true;
	}
}//namespace ngl
