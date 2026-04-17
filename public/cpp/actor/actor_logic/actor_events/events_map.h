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
// File overview: Event-to-handler mapping that routes event types to their processing functions.

#pragma once

#include "actor/actor_logic/actor_events/actor_events.h"

namespace ngl
{
	// Event type
	enum eevents_map
	{
		eevents_map_leaveview,		// Leaveview range
		eevents_map_enterview,		// View range
		eevents_map_death,			// Translated comment.
		eevents_map_count,
	};

	// Eventtype
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
		DPROTOCOL(np_eevents_map_leaveview, m_type, m_leaveunitid, m_unitids)
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
		DPROTOCOL(np_eevents_map_enterview, m_enterunitid, m_unitids)
	};

	class np_eevents_map_death :public np_eevents_map
	{
	public:
		np_eevents_map_death() :
			np_eevents_map(eevents_map_death),
			m_deathunitid(0)
		{}

		i64_actorid m_deathunitid;
		DPROTOCOL(np_eevents_map_death, m_deathunitid)
	};

	using actor_events_map = actor_events<ENUM_EVENTS_LOGIC, eevents_map, eevents_map_count>;
}