#pragma once

#include "actor_events.h"

namespace ngl
{
	// �¼�ö������
	enum eevents_logic
	{
		eevents_logic_rolelogin,		//��ҵ�½
		eevents_logic_roleoffline,		//�������
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

	class np_eevents_logic_rolelogin :public np_eevents_logic
	{
	public:
		np_eevents_logic_rolelogin() :
			np_eevents_logic(eevents_logic_rolelogin),
			m_actorid(0)
		{}

		i64_actorid m_actorid;
		def_portocol(np_eevents_logic_rolelogin, m_type, m_actorid)
	};

	class np_eevents_logic_roleoffline :public np_eevents_logic
	{
	public:
		np_eevents_logic_roleoffline() :
			np_eevents_logic(eevents_logic_roleoffline),
			m_actorid(0)
		{}

		i64_actorid m_actorid;
		def_portocol(np_eevents_logic_roleoffline, m_type, m_actorid)
	};

	using actor_events_logic = actor_events<ENUM_EVENTS_LOGIC, eevents_logic, eevents_logic_count>;
}//namespace ngl