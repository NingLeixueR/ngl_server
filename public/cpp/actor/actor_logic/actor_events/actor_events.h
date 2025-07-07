#pragma once

#include "actor.h"
#include "type.h"
#include "cmd.h"

namespace ngl
{
	// ENUM_EVENTS��Ҫ��Ϊ�˶���̼�actor.m_typeֵ��ͬ
	// ���Գ���actor_events����ͬ����
	// ENUM_EVENTS��E_EVENTS���ʹ��
	enum ENUM_EVENTS
	{
		ENUM_EVENTS_LOGIC,		// ��Ӧeevents_logic
		ENUM_EVENTS_MAP,		// ��Ӧeevents_map
	};

	template <
		ENUM_EVENTS ETYPE			// �¼�����
		, typename E_EVENTS			// ���¼�����
		, int E_EVENTS_COUNT		// ���¼�����
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
			std::vector<std::pair<E_EVENTS, i64_actorid>> m_vecpair;

			dprotocoljson(np_event_register, m_vecpair)
		};

		static void nregister()
		{
			register_handle_custom<tactor_events>::template func<
				tactor_events::np_event_register
			>(true);
		}

		// # �������ʹ���
		template <typename TPARM>
		static void register_parm(E_EVENTS atype)
		{
			m_parmtype[atype]= typeid(TPARM).hash_code();
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, tactor_events, TPARM>
				(
					[atype](tactor_events*, message<TPARM>& adata)
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

		// # actorע���ע��event
		struct register_actor_event
		{
			static void func(i64_actorid aactorid, E_EVENTS atype)
			{
				auto pro = std::make_shared<np_event_register>();
				pro->m_vecpair.push_back({ atype, aactorid });
				actor::send_actor(actorid(), aactorid, pro);
			}
		};
		using tfun = template_arg_event<register_actor_event>;

		struct nullacctor
		{
			template <typename TPARM>
			bool handle(const message<TPARM>& adata)
			{
				actor::send_actor(actorid(), nguid::make(), adata.get_shared_data());
				return true;
			}
		};
		static nullacctor m_nullacctor;

		// # �����¼�
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
					"np_event_register {}:E_EVENTS:{} actor:{}"
					, typeid(E_EVENTS).name()
					, (int32_t)(item.first)
					, nguid(item.second)
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