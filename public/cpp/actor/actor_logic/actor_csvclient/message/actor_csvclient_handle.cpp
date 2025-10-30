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
#include "actor_csvclient.h"
#include "actor_csvserver.h"
#include "actor_manage.h"
namespace ngl
{
	bool actor_csvclient::handle(const message<np_actor_reloadcsv>& adata)
	{
		for (const auto& [key, value] : adata.get_data()->m_csvcontent)
		{
			reload_csv::save(key, value);
		}
		actor_suspendthread ltemp;
		for (const auto& [key, value] : adata.get_data()->m_csvcontent)
		{
			reload_csv::reload(key);
		}
		return true;
	}
}//namespace ngl
