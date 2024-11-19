#pragma once

#include "type.h"
#include "actor.h"
#include "cmd.h"

namespace ngl
{
	// ENUM_EVENTS主要是为了多进程间actor.m_type值不同
	// 可以抽离actor_events到不同进程
	// ENUM_EVENTS与E_EVENTS配对使用
	enum ENUM_EVENTS
	{
		ENUM_EVENTS_LOGIC,		// 对应eevents_logic
	};

	template <ENUM_EVENTS ETYPE, typename E_EVENTS/* 事件枚举类型*/>
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

		static std::array<i64_hashcode, E_EVENTS::count> m_parmtype;
		static std::map<E_EVENTS, std::set<i64_actorid>> m_eventmember;
	public:
		static int32_t id_index()
		{
			assert((int32_t)ETYPE <= (int32_t)(ACTOR_EVENTS_MAX_COUNT - ACTOR_EVENTS));
			return (int32_t)ETYPE;
		}

		friend class actor_instance<actor_events<ETYPE, E_EVENTS>>;
		static actor_events<ETYPE, E_EVENTS>& getInstance()
		{
			return actor_instance<actor_events<ETYPE, E_EVENTS>>::instance();
		}

		struct np_event_register
		{
			E_EVENTS m_type;
			i64_actorid m_actorid;
			def_portocol(np_event_register, m_type, m_actorid)
		};

		static void nregister()
		{
			using type_actor_events = actor_events<ETYPE, E_EVENTS>;
			register_handle_custom<type_actor_events>::template func<
				type_actor_events::np_event_register
			>(true);
		}

		template <typename TPARM>
		static void register_parm(E_EVENTS atype)
		{
			m_parmtype[atype] = typeid(TPARM).hash_code();
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, actor_events<ETYPE, E_EVENTS>, TPARM>
				([atype](actor_events<ETYPE, E_EVENTS>*, message<TPARM>& adata)
				{
					std::set<i64_actorid>* lmember = tools::findmap(m_eventmember, atype);
					if (lmember == nullptr)
						return;
					actor::static_send_actor(*lmember, actorid(), adata.get_shared_data());
				});
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
		static void register_actor_event(E_EVENTS atype, i64_actorid aactorid)
		{
			auto pro = std::make_shared<np_event_register>();
			pro->m_type		= atype;
			pro->m_actorid	= aactorid;
			actor::static_send_actor(actorid(), aactorid, pro);
		}

		// # 触发事件
		template <typename TPARM>
		static bool trigger_event(E_EVENTS atype, const TPARM& apram)
		{
			ngl::log_error()->print(
				"trigger_event {}:E_EVENTS:{}", 
				typeid(TPARM).name(), (int32_t)(atype)
			);
			if (check_parm<TPARM>(atype) == false)
			{
				ngl::log_error()->print("trigger_event fail!!!");
				return false;
			}
			auto pro = std::make_shared<TPARM>(apram);
			actor::static_send_actor(actorid(), nguid::make(), pro);
			return true;
		}

		bool handle(const message<np_event_register>& adata)
		{
			const np_event_register& pro = *adata.get_data();
			ngl::log_error()->print(
				"np_event_register {}:E_EVENTS:{} actor:{}", 
				typeid(E_EVENTS).name(), (int32_t)(pro.m_type), nguid(pro.m_actorid)
			);
			m_eventmember[pro.m_type].insert(pro.m_actorid);
			return true;
		}
	};

	template <ENUM_EVENTS ETYPE, typename E_EVENTS>
	std::array<i64_hashcode, E_EVENTS::count> actor_events<ETYPE, E_EVENTS>::m_parmtype;

	template <ENUM_EVENTS ETYPE, typename E_EVENTS>
	std::map<E_EVENTS, std::set<i64_actorid>> actor_events<ETYPE, E_EVENTS>::m_eventmember;

	// 事件枚举类型
	enum eevents_logic
	{
		eevents_logic_rolelogin,		//玩家登陆
		count,
	};

	// 事件类型
	class np_eevents_logic_rolelogin
	{
	public:
		np_eevents_logic_rolelogin() :
			m_type(eevents_logic_rolelogin),
			m_actorid(0)
		{}

		eevents_logic m_type;
		i64_actorid m_actorid;
		def_portocol(np_eevents_logic_rolelogin, m_type, m_actorid)
	};
}//namespace ngl