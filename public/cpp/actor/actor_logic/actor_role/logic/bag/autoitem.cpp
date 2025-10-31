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
#include "autoitem.h"

namespace ngl
{
	void autoitem::add(int32_t atid, int32_t acount)
	{
		m_addstackitems[atid] += acount;
	}

	void autoitem::add(int32_t aid)
	{
		m_addnostackitems.push_back(aid);
	}

	void autoitem::del(int32_t aid, int32_t acount)
	{
		m_delstackitems[aid] += acount;
	}

	void autoitem::del(int32_t aid)
	{
		m_addnostackitems.push_back(aid);
	}
}// namespace ngl