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
	enum eevents_logic
	{
		eevents_logic_null,				// δ��ֵ
		eevents_logic_rolelogin,		// ��ҵ�½
		eevents_logic_roleoffline,		// �������
		eevents_logic_rolelevelchange,	// ��ҵȼ������仯
		eevents_logic_rolegoldchange,	// ��ҽ�ҷ����仯
		eevents_logic_count,
	};

	// �¼�����
	class np_eevents_logic
	{
	public:
		np_eevents_logic(eevents_logic atype) :
			m_type(atype)
		{}

		eevents_logic m_type;
	};

	class np_eevents_logic_rolelogin :
		public np_eevents_logic
	{
	public:
		np_eevents_logic_rolelogin() :
			np_eevents_logic(eevents_logic_rolelogin),
			m_actorid(0)
		{}

		i64_actorid m_actorid;
		dprotocol(np_eevents_logic_rolelogin, m_type, m_actorid)
	};

	class np_eevents_logic_roleoffline :
		public np_eevents_logic
	{
	public:
		np_eevents_logic_roleoffline() :
			np_eevents_logic(eevents_logic_roleoffline),
			m_actorid(0)
		{}

		i64_actorid m_actorid;
		dprotocol(np_eevents_logic_roleoffline, m_type, m_actorid)
	};

	class np_eevents_logic_rolevaluechange :
		public np_eevents_logic
	{
	public:
		np_eevents_logic_rolevaluechange() :
			np_eevents_logic(eevents_logic_null),
			m_actorid(0),
			m_beforevalue(0),
			m_nowvalue(0)
		{}

		np_eevents_logic_rolevaluechange(eevents_logic atype) :
			np_eevents_logic(atype),
			m_actorid(0),
			m_beforevalue(0),
			m_nowvalue(0)
		{}

		i64_actorid m_actorid;
		int32_t m_beforevalue;
		int32_t m_nowvalue;
		dprotocol(np_eevents_logic_rolevaluechange, m_type, m_actorid, m_beforevalue, m_nowvalue)
	};

	using actor_events_logic = actor_events<ENUM_EVENTS_LOGIC, eevents_logic, eevents_logic_count>;
}//namespace ngl