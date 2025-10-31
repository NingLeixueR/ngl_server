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
#include "actor_log.h"
namespace ngl
{
	bool actor_log::handle(const message<np_logitem>& adata)
	{
		const np_logitem* ldata = adata.get_data();
		auto itor = m_logarr.find(ldata->m_type);
		if (itor == m_logarr.end())
		{
			return true;
		}
		itor->second->printf(ldata);
		return true;
	}
}//namespace ngl
