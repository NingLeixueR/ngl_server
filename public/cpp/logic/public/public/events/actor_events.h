#pragma once

#include "type.h"
#include "actor.h"
#include "cmd.h"

namespace ngl
{
	// ENUM_EVENTS��Ҫ��Ϊ�˶���̼�actor.m_typeֵ��ͬ
	// ���Գ���actor_events����ͬ����
	// ENUM_EVENTS��E_EVENTS���ʹ��
	enum ENUM_EVENTS
	{
		ENUM_EVENTS_LOGIC,		// ��Ӧeevents_logic
	};

	template <ENUM_EVENTS ETYPE, typename E_EVENTS/* �¼�ö������*/>
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
			std::vector<std::pair<E_EVENTS, i64_actorid>> m_vecpair;
			def_portocol(np_event_register, m_vecpair)
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
					{
						return;
					}
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

		// # actorע���ע��event
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

		// # �����¼�
		template <typename TPARM>
		static bool trigger_event(E_EVENTS atype, const TPARM& apram)
		{
			ngl::log_error()->print("trigger_event {}:E_EVENTS:{}", typeid(TPARM).name(), (int32_t)(atype));
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

	template <ENUM_EVENTS ETYPE, typename E_EVENTS>
	std::array<i64_hashcode, E_EVENTS::count> actor_events<ETYPE, E_EVENTS>::m_parmtype;

	template <ENUM_EVENTS ETYPE, typename E_EVENTS>
	std::map<E_EVENTS, std::set<i64_actorid>> actor_events<ETYPE, E_EVENTS>::m_eventmember;

	// �¼�ö������
	enum eevents_logic
	{
		eevents_logic_rolelogin,		//��ҵ�½
		eevents_logic_roleoffline,		//�������
		count,
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

	using actor_events_logic = actor_events<ENUM_EVENTS_LOGIC, eevents_logic>;
}//namespace ngl