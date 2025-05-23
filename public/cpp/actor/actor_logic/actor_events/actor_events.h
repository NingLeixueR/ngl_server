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

	template <ENUM_EVENTS ETYPE, typename E_EVENTS/* 事件枚举类型*/, int E_EVENTS_COUNT>
	class actor_events : public actor
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

		static std::array<i64_hashcode, E_EVENTS_COUNT> m_parmtype;
		static std::map<E_EVENTS, std::set<i64_actorid>> m_eventmember;
	public:
		static int32_t id_index()
		{
			assert((int32_t)ETYPE <= (int32_t)(ACTOR_EVENTS_MAX_COUNT - ACTOR_EVENTS));
			return (int32_t)ETYPE;
		}

		using type_actor_events = actor_events<ETYPE, E_EVENTS, E_EVENTS_COUNT>;

		friend class actor_instance<type_actor_events>;
		static type_actor_events& getInstance()
		{
			return actor_instance<type_actor_events>::instance();
		}

		struct np_event_register
		{
			std::vector<std::pair<E_EVENTS, i64_actorid>> m_vecpair;
			def_portocol(np_event_register, m_vecpair)
		};

		static void nregister()
		{
			register_handle_custom<type_actor_events>::template func<
				type_actor_events::np_event_register
			>(true);
		}

		// # 按照类型触发
		template <typename TPARM>
		static void register_parm(E_EVENTS atype)
		{
			m_parmtype[atype]= typeid(TPARM).hash_code();
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, type_actor_events, TPARM>
				(
					[atype](type_actor_events*, message<TPARM>& adata)
					{
						std::set<i64_actorid>* lmember = tools::findmap(m_eventmember, atype);
						if (lmember == nullptr)
						{
							return;
						}
						actor::static_send_actor(*lmember, actorid(), adata.get_shared_data());
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

		// # actor注册关注的event
		class register_actor_event
		{
		public:
			static void func(E_EVENTS atype, i64_actorid aactorid)
			{
				auto pro = std::make_shared<np_event_register>();
				pro->m_vecpair.push_back({ atype, aactorid });
				actor::static_send_actor(actorid(), aactorid, pro);
			}
		};
		using tfun = ngl::template_arg_s<register_actor_event, E_EVENTS, i64_actorid>;

		class nullacctor
		{
		public:
			template <typename TPARM>
			bool handle(const message<TPARM>& adata)
			{
				actor::static_send_actor(actorid(), nguid::make(), adata.get_shared_data());
				return true;
			}
		};
		static nullacctor m_nullacctor;

		// # 触发事件
		template <typename TPARM, typename TACTOR = nullacctor>
		static bool trigger_event(const TPARM& apram, TACTOR* aactor = &m_nullacctor)
		{
			ngl::log_error()->print("trigger_event {}:E_EVENTS:{}", typeid(TPARM).name(), (int32_t)(apram.m_type));
			if (check_parm<TPARM>(apram.m_type) == false)
			{
				ngl::log_error()->print("trigger_event fail!!!");
				return false;
			}
			auto lparm = std::make_shared<TPARM>(apram);
			message<TPARM> lmessage(1, nullptr, lparm);
			aactor->handle(lmessage);
			return true;
		}

		bool handle(const message<np_event_register>& adata)
		{
			const np_event_register& pro = *adata.get_data();
			for (const auto& item : pro.m_vecpair)
			{
				ngl::log_error()->print(
					"np_event_register {}:E_EVENTS:{} actor:{}", typeid(E_EVENTS).name(), (int32_t)(item.first), nguid(item.second)
				);
				m_eventmember[item.first].insert(item.second);
			}
			return true;
		}
	};

	template <ENUM_EVENTS ETYPE, typename E_EVENTS, int E_EVENTS_COUNT>
	std::array<i64_hashcode, E_EVENTS_COUNT> actor_events<ETYPE, E_EVENTS, E_EVENTS_COUNT>::m_parmtype;

	template <ENUM_EVENTS ETYPE, typename E_EVENTS, int E_EVENTS_COUNT>
	std::map<E_EVENTS, std::set<i64_actorid>> actor_events<ETYPE, E_EVENTS, E_EVENTS_COUNT>::m_eventmember;

	template <ENUM_EVENTS ETYPE, typename E_EVENTS, int E_EVENTS_COUNT>
	actor_events<ETYPE, E_EVENTS, E_EVENTS_COUNT>::nullacctor actor_events<ETYPE, E_EVENTS, E_EVENTS_COUNT>::m_nullacctor;


}//namespace ngl