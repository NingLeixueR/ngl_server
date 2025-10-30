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
#include "actor_role.h"
#include "remakes.h"

namespace ngl
{
	void remakes::add_remakes(const char* aremakes)
	{
		m_remakes.push(aremakes);
	}

	void remakes::erase_remakes()
	{
		if (m_remakes.empty() == false)
		{
			m_remakes.pop();
		}
	}

	const char* remakes::get_remakes()
	{
		if (m_remakes.empty() == false)
		{
			return m_remakes.top().c_str();
		}
		return "";
	}

	dremakes::dremakes(actor_role* arole, const char* aremakes)
	{
		m_role->m_remakes.add_remakes(aremakes);
	}

	dremakes::dremakes(actor_role* arole, const std::string& aremakes)
	{
		m_role->m_remakes.add_remakes(aremakes.c_str());
	}

	dremakes::~dremakes()
	{
		m_role->m_remakes.erase_remakes();
	}

	const char* dremakes::get_remake(actor_role* arole)
	{
		return arole->m_remakes.get_remakes();
	}
}//namespace ngl