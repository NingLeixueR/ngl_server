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
// File overview: Events actor that manages timed game events and activity scheduling.

#pragma once

#include "actor/actor_base/core/actor.h"
#include "tools/tools/tools_cmd.h"
#include "tools/type.h"

namespace ngl
{
	// ENUM_EVENTS determines the actor's m_type offset.
	// Multiple actor_events instances can coexist by using
	// different ENUM_EVENTS and E_EVENTS type combinations.
	enum ENUM_EVENTS
	{
		ENUM_EVENTS_LOGIC,		// Corresponds to eevents_logic
		ENUM_EVENTS_MAP,		// Corresponds to eevents_map
	};

	template <
		ENUM_EVENTS ETYPE			// Event category type
		, typename E_EVENTS			// Event enum type
		, int E_EVENTS_COUNT		// Number of event types
	>
	class actor_events : 
		public actor
	{
		actor_events(const actor_events&) = delete;
		actor_events& operator=(const actor_events&) = delete;

		actor_events():
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = (ENUM_ACTOR)(ACTOR_EVENTS + id_index()),
						.m_area = tab_self_area,
						.m_id = id_index()
					},
					.m_weight = 0x7fffffff,
				})
		{}

		static std::array<i64_hashcode, E_EVENTS_COUNT>		m_parmtype;
		static std::map<E_EVENTS, std::set<i64_actorid>>	m_eventmember;
	public:
		static int32_t id_index()
		{
			if constexpr ((int32_t)ETYPE >= (int32_t)(ACTOR_EVENTS_MAX_COUNT - ACTOR_EVENTS))
			{
				tools::no_core_dump();
				return -1;
			}
			else
			{
				return (int32_t)ETYPE;
			}
		}

		using tactor_events = actor_events<ETYPE, E_EVENTS, E_EVENTS_COUNT>;

		friend class actor_instance<tactor_events>;
		static tactor_events& instance()
		{
			return actor_instance<tactor_events>::instance();
		}

		struct np_event_register
		{
			std::vector<E_EVENTS> m_events;
			i64_actorid m_actorid;
			DPROTOCOL(np_event_register, m_events, m_actorid)
		};

		static void nregister()
		{
			register_handle<tactor_events
				, tactor_events::np_event_register
			> ();
		}

		// Register a parameter type for an event type
		template <typename TPARM>
		static void register_parm(E_EVENTS atype)
		{
			m_parmtype[atype]= typeid(TPARM).hash_code();
			actor::register_actor_s<tactor_events, TPARM>([atype](tactor_events*, const message<TPARM>& adata)
				{
					std::set<i64_actorid>* lmember = tools::findmap(m_eventmember, atype);
					if (lmember == nullptr)
					{
						return;
					}
					actor::send_actor(*lmember, actorid(), adata.get_shared_data());
				}
			);
		}

		template <typename TPARM>
		static bool check_parm(E_EVENTS atype)
		{
			return m_parmtype[atype] == typeid(TPARM).hash_code();
		}

		static i64_actorid actorid()
		{
			return nguid::make((ENUM_ACTOR)(ACTOR_EVENTS + id_index()), tab_self_area, id_index());
		}

		template <typename ...ARGS>
		static void event_func(i64_actorid aactorid, ARGS... atypes)
		{
			auto pro = std::make_shared<np_event_register>();
			pro->m_actorid = aactorid;
			pro->m_events.reserve(sizeof...(ARGS));

			(pro->m_events.emplace_back(atypes), ...);

			actor::send_actor(actorid(), aactorid, pro);
		}

		// Trigger an event and dispatch to registered listeners
		struct tnactor {};
		template <typename TPARM, typename TACTOR = tnactor>
		static bool trigger_event(const TPARM& apram, TACTOR* aactor = nullptr)
		{
			ngl::log_error()->print("trigger_event {}:E_EVENTS:{}", typeid(TPARM).name(), (int32_t)(apram.m_type));
			if (check_parm<TPARM>(apram.m_type) == false)
			{
				ngl::log_error()->print("trigger_event fail!!!");
				return false;
			}
			auto lparm = std::make_shared<TPARM>(apram);

			if constexpr (std::is_same<TACTOR, tnactor>::value)
			{
				if (aactor == nullptr)
				{
					actor::send_actor(actorid(), nguid::make(), lparm);
					return true;
				}
				return false;
			}
			else
			{
				message<TPARM> lmessage(1, nullptr, lparm);
				aactor->handle(lmessage);
				return true;
			}
		}

		bool handle([[maybe_unused]] const message<np_event_register>& adata)
		{
			const np_event_register& pro = *adata.get_data();
			for (const E_EVENTS ltype : pro.m_events)
			{
				m_eventmember[ltype].insert(pro.m_actorid);
			}
			return true;
		}
	};

	template <ENUM_EVENTS ETYPE, typename E_EVENTS, int E_EVENTS_COUNT>
	std::array<i64_hashcode, E_EVENTS_COUNT> actor_events<ETYPE, E_EVENTS, E_EVENTS_COUNT>::m_parmtype;

	template <ENUM_EVENTS ETYPE, typename E_EVENTS, int E_EVENTS_COUNT>
	std::map<E_EVENTS, std::set<i64_actorid>> actor_events<ETYPE, E_EVENTS, E_EVENTS_COUNT>::m_eventmember;
}//namespace ngl
