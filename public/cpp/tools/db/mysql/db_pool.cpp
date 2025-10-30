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
#include "ttab_servers.h"
#include "db_pool.h"
#include "nlog.h"

namespace ngl
{
	void db_pool::init(const dbarg& adbarg)
	{
		if (!m_vec.empty())
		{
			tools::no_core_dump();
			return;
		}
		const tab_servers* tab = ttab_servers::instance().tab();
		if (tab == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		m_vec.resize(tab->m_threadnum);
		for (int i = 0; i < tab->m_threadnum; ++i)
		{
			m_vec[i] = new db();
			if (!m_vec[i]->connectdb(adbarg))
			{
				tools::no_core_dump();
				continue;
			}
		}
		return;
	}

	db* db_pool::get(int32_t aindex)
	{
		if (aindex >= m_vec.size() && aindex < 0)
		{
			tools::no_core_dump();
			return nullptr;
		}
		return m_vec[aindex];
	}
}// namespace ngl