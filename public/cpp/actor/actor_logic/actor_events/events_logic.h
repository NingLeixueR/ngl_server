/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Event logic helpers for evaluating event conditions and triggers.

#pragma once

#include "actor/actor_logic/actor_events/actor_events.h"

namespace ngl
{
	// Event type
	enum eevents_logic
	{
		eevents_logic_null,				// Translated comment.
		eevents_logic_rolelogin,		// Player
		eevents_logic_roleoffline,		// Playerunder
		eevents_logic_rolelevelchange,	// Playerlevel change
		eevents_logic_rolegoldchange,	// Player change
		eevents_logic_count,
	};

	// Eventtype
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
		DPROTOCOL(np_eevents_logic_rolelogin, m_type, m_actorid)
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
		DPROTOCOL(np_eevents_logic_roleoffline, m_type, m_actorid)
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
		DPROTOCOL(np_eevents_logic_rolevaluechange, m_type, m_actorid, m_beforevalue, m_nowvalue)
	};

	using actor_events_logic = actor_events<ENUM_EVENTS_LOGIC, eevents_logic, eevents_logic_count>;
}//namespace ngl