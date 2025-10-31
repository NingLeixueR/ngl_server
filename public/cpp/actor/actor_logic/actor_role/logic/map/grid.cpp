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
#include "grid.h"

namespace ngl
{
	bool grid::enter(unit* aunit)
	{
		return m_unitlist.insert(aunit->id()).second;
	}

	void grid::leave(unit* aunit)
	{
		m_unitlist.erase(aunit->id());
	}

	std::set<i64_actorid>* grid::get_unitlist()
	{
		return &m_unitlist;
	}
}