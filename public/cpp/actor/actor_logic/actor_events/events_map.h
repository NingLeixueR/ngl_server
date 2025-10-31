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

#include "actor_events.h"

namespace ngl
{
	// �¼�ö������
	enum eevents_map
	{
		eevents_map_leaveview,		// �뿪��Ұ
		eevents_map_enterview,		// ������Ұ
		eevents_map_death,			// ��λ����
		eevents_map_count,
	};

	// �¼�����
	class np_eevents_map
	{
	public:
		np_eevents_map(eevents_map atype) :
			m_type(atype)
		{}

		eevents_map m_type;
	};

	class np_eevents_map_leaveview :public np_eevents_map
	{
	public:
		np_eevents_map_leaveview() :
			np_eevents_map(eevents_map_leaveview),
			m_leaveunitid(0)
		{}

		i64_actorid m_leaveunitid;
		std::set<i64_actorid> m_unitids;
		dprotocol(np_eevents_map_leaveview, m_type, m_leaveunitid, m_unitids)
	};

	class np_eevents_map_enterview :public np_eevents_map
	{
	public:
		np_eevents_map_enterview() :
			np_eevents_map(eevents_map_enterview),
			m_enterunitid(0)
		{}

		i64_actorid m_enterunitid;
		std::set<i64_actorid> m_unitids;
		dprotocol(np_eevents_map_enterview, m_enterunitid, m_unitids)
	};

	class np_eevents_map_death :public np_eevents_map
	{
	public:
		np_eevents_map_death() :
			np_eevents_map(eevents_map_death),
			m_deathunitid(0)
		{}

		i64_actorid m_deathunitid;
		dprotocol(np_eevents_map_death, m_deathunitid)
	};

	using actor_events_map = actor_events<ENUM_EVENTS_LOGIC, eevents_map, eevents_map_count>;
}