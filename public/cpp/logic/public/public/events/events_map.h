#pragma once

#include "actor_events.h"

namespace ngl
{
	// 事件枚举类型
	enum eevents_map
	{
		eevents_map_leaveview,		// 离开视野
		eevents_map_enterview,		// 进入视野
		eevents_map_count,
	};

	// 事件类型
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
		def_portocol(np_eevents_map_leaveview, m_type, m_leaveunitid, m_unitids)
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
		def_portocol(np_eevents_map_enterview, m_enterunitid, m_unitids)
	};

	using actor_events_map = actor_events<ENUM_EVENTS_LOGIC, eevents_map, eevents_map_count>;
}