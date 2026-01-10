/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "actor.h"
#include "type.h"
#include "cmd.h"

namespace ngl
{
	// ENUM_EVENTS主要是为了多进程间actor.m_type值不同
	// 可以抽离actor_events到不同进程
	// ENUM_EVENTS与E_EVENTS配对使用
	enum ENUM_EVENTS
	{
		ENUM_EVENTS_LOGIC,		// 对应eevents_logic
		ENUM_EVENTS_MAP,		// 对应eevents_map
	};

	template <
		ENUM_EVENTS ETYPE			// 事件类型
		, typename E_EVENTS			// 子事件类型
		, int E_EVENTS_COUNT		// 子事件数量
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
			dprotocol(np_event_register, m_events, m_actorid)
		};

		static void nregister()
		{
			register_handle<tactor_events
				, tactor_events::np_event_register
			> (e_ready_all);
		}

		// # 按照类型触发
		template <typename TPARM>
		static void register_parm(E_EVENTS atype)
		{
			m_parmtype[atype]= typeid(TPARM).hash_code();
			actor::register_actor_s<tactor_events, TPARM>(e_ready_all, [atype](tactor_events*, const message<TPARM>& adata)
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
		static void event_funcs(np_event_register& apro, ARGS... atypes)
		{
			(apro.m_events.push_back(atypes), ...);
		}

		template <typename ...ARGS>
		static void event_func(i64_actorid aactorid, ARGS... atypes)
		{
			auto pro = std::make_shared<np_event_register>();
			pro->m_actorid = aactorid;

			event_funcs(*pro, atypes...);

			actor::send_actor(actorid(), aactorid, pro);
		}

		// # 触发事件
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

		bool handle(const message<np_event_register>& adata)
		{
			const np_event_register& pro = *adata.get_data();
			for (auto ltype : pro.m_events)
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