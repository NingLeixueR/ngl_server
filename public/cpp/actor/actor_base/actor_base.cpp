#include "actor_manage.h"
#include "ndb_modular.h"
#include "actor_base.h"
#include "actor_log.h"
#include "ndbclient.h"
#include "ngroup.h"
#include "xml.h"
#include "net.h"

namespace ngl
{
	i64_actorid actor_base::actorclient_guid()
	{
		return actor_client::actorid();
	}

	//# ��־���
	const std::shared_ptr<np_actor_logitem> g_actor_nonelog = std::make_shared<np_actor_logitem>();

	tools_log::tools_log(actor_base* aactor /*= nullptr*/) :
		m_actor(aactor)
	{}

	void tools_log::set_logactor(actor_base* aactor)
	{
		m_actor = aactor;
	}

	std::shared_ptr<np_actor_logitem> tools_log::get_log(const std::source_location& asource, ELOGLEVEL aloglevel, bool anet)const
	{
		if(m_actor == nullptr || !np_actor_logitem::check_level(aloglevel))
		{
			return g_actor_nonelog;
		}
		return std::make_shared<np_actor_logitem>(aloglevel, m_actor->type(), anet ? ELOG_NETWORK : ELOG_LOCAL, asource);
	}

	std::shared_ptr<np_actor_logitem> tools_log::log_debug(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_DEBUG, false);
	}

	std::shared_ptr<np_actor_logitem> tools_log::log_debug_net(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_DEBUG, true);
	}

	std::shared_ptr<np_actor_logitem> tools_log::log_info(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_INFO, false);
	}

	std::shared_ptr<np_actor_logitem> tools_log::log_info_net(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_INFO, true);
	}

	std::shared_ptr<np_actor_logitem> tools_log::log_warn(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_WARN, false);
	}

	std::shared_ptr<np_actor_logitem> tools_log::log_warn_net(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_WARN, true);
	}

	std::shared_ptr<np_actor_logitem> tools_log::log_error(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_ERROR, false);
	}

	std::shared_ptr<np_actor_logitem> tools_log::log_error_net(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_ERROR, true);
	}

	struct actor_base::impl_actor_base
	{
		nguid										m_guid;				// actor guid
		std::unique_ptr<actor_manage_dbclient>		m_dbclient;			// dbclient���������
		bool										m_isload;			// �����Ƿ�������
		std::map<pbdb::ENUM_DB, ndb_component*>		m_dbcomponent;
		actor_base*									m_actor;

		i32_session									m_kcpsession = -1;

		//# ���һ��ʱ�䷢���ȫԱ(����actor)�㲥
		//# ����������㲥������һЩ��Ҫ���������,���� ��������
		//# ����ȫԱ�㲥�� ��λ(����)
		static int									m_broadcast;	
		//# ���͹㲥�Ķ�ʱ��id
		static int									m_broadcasttimer;
		//# �Ƿ���չ㲥��Ϣ
		bool										m_isbroadcast = false;			

		inline impl_actor_base(actor_base* aactor, const actorparmbase& aparm):
			m_guid(aparm.m_type, aparm.m_area, aparm.m_id),
			m_isload(aparm.m_manage_dbclient),
			m_actor(aactor)
		{
			if (aparm.m_manage_dbclient)
			{
				m_dbclient = std::make_unique<actor_manage_dbclient>(aactor);
				m_dbclient->set_loadfinish_function([aactor](bool adbishave)
					{
						aactor->loaddb_finish(adbishave);
					});
			}
		}

		inline void erase_actor_byid()const
		{
			m_actor->erase_actor_before();
			actor_manage::getInstance().erase_actor_byid(m_guid);
		}

		inline bool is_single()const
		{
			return enum_actor::is_signle(m_guid.type());
		}

		inline nguid& guid()
		{
			return m_guid;
		}

		inline i64_actorid id_guid()const
		{
			return m_guid;
		}

		inline i32_actordataid id()const
		{
			return m_guid.actordataid();
		}

		inline i16_area area()const
		{
			return m_guid.area();
		}

		inline ENUM_ACTOR type()const
		{
			return m_guid.type();
		}

		inline bool isloadfinish()const
		{
			return m_dbclient == nullptr || m_dbclient->isloadfinish();
		}

		inline void add_dbclient(ndbclient_base* adbclient, i64_actorid aid)
		{
			Try
			{
				Assert(m_dbclient != nullptr)
				m_dbclient->add(adbclient, aid);
			}Catch
		}

		inline void save()
		{
			m_dbclient->save();
		}

		inline std::unique_ptr<actor_manage_dbclient>& get_actor_manage_dbclient()
		{
			return m_dbclient;
		}

		inline bool isload()const
		{
			return m_isload;
		}

		inline void set_db_component(ndb_component* acomponent)
		{
			m_dbcomponent[acomponent->type()] = acomponent;
		}

		inline void db_component_init_data()
		{
			for (const auto [_, ldbcomponent] : m_dbcomponent)
			{
				ldbcomponent->init_data();
			}
		}

		inline void init_db_component(bool acreate)const
		{
			if (acreate)
			{
				for (const auto [key, value] : m_dbcomponent)
				{
					value->create();
				}
			}
			else
			{
				for (const auto [key, value] : m_dbcomponent)
				{
					value->init();
				}
			}
		}

		inline void set_kcpssion(i32_session asession)
		{
			m_kcpsession = asession;
		}

		inline i32_session get_kcpssion()const
		{
			return m_kcpsession;
		}

		inline int32_t set_timer(const timerparm& aparm)
		{
			auto lparm = std::make_shared<timerparm>(aparm);
			return ntimer::addtimer(m_actor, lparm);
		}

		inline bool isbroadcast()const
		{
			return m_isbroadcast;
		}

		inline void set_broadcast(bool aisbroadcast)
		{
			m_isbroadcast = aisbroadcast;
		}

		static void start_broadcast()
		{
			wheel_parm lparm
			{
				.m_ms = m_broadcast,
				.m_intervalms = [](int64_t) 
				{
					return actor_base::impl_actor_base::m_broadcast; 
				} ,
				.m_count = 0x7fffffff,
				.m_fun = [](const wheel_node*)
				{
					auto pro = std::make_shared<np_actor_broadcast>();
					handle_pram lpram = handle_pram::create<np_actor_broadcast, false>(nguid::make(), nguid::make(), pro);
					actor_manage::getInstance().broadcast_task(lpram);
				}
			};
			m_broadcasttimer = (int32_t)twheel::wheel().addtimer(lparm);
		}
	};

	int actor_base::impl_actor_base::m_broadcast			= 10000;
	int actor_base::impl_actor_base::m_broadcasttimer		= -1;

	actor_base::actor_base(const actorparmbase& aparm):
		tools_log(this)
	{
		m_impl_actor_base.make_unique(this, aparm);
	}

	void actor_base::erase_actor_byid()
	{
		m_impl_actor_base()->erase_actor_byid();
	}

	bool actor_base::is_single()
	{
		return m_impl_actor_base()->is_single();
	}

	nguid& actor_base::guid()
	{
		return m_impl_actor_base()->guid();
	}
	
	i64_actorid actor_base::id_guid()
	{
		return m_impl_actor_base()->id_guid();
	}

	i32_actordataid actor_base::id()
	{
		return m_impl_actor_base()->id();
	}

	i16_area actor_base::area()
	{
		return m_impl_actor_base()->area();
	}

	ENUM_ACTOR actor_base::type()
	{
		return m_impl_actor_base()->type();
	}

	bool actor_base::isloadfinish()
	{
		return m_impl_actor_base()->isloadfinish();
	}

	void actor_base::add_dbclient(ndbclient_base* adbclient, i64_actorid aid)
	{
		m_impl_actor_base()->add_dbclient(adbclient, aid);
	}

	actor_base::~actor_base()
	{
	}

	void actor_base::erase_actor_byid(const nguid& aguid)
	{
		actor_manage::getInstance().erase_actor_byid(aguid);
	}

	void actor_base::push_task_id(const nguid& aguid, handle_pram& apram, bool abool)
	{
		actor_manage::getInstance().push_task_id(aguid, apram, abool);
	}

	void actor_base::push_task_type(ENUM_ACTOR atype, handle_pram& apram, bool aotherserver/* = false*/)const
	{
		actor_manage::getInstance().push_task_type(atype, apram, aotherserver);
	}

	void actor_base::push_task_id(handle_pram& apram, bool abool)
	{
		actor_manage::getInstance().push_task_id(m_impl_actor_base()->m_guid, apram, abool);
	}

	void actor_base::save()
	{
		m_impl_actor_base()->m_dbclient->save();
	}

	std::unique_ptr<actor_manage_dbclient>& actor_base::get_actor_manage_dbclient()
	{
		return m_impl_actor_base()->m_dbclient;
	}

	bool actor_base::isload()
	{
		return m_impl_actor_base()->isload();
	}

	void actor_base::set_db_component(ndb_component* acomponent)
	{
		m_impl_actor_base()->set_db_component(acomponent);
	}

	void actor_base::db_component_init_data()
	{
		m_impl_actor_base()->db_component_init_data();
	}

	void actor_base::init_db_component(bool acreate)
	{
		m_impl_actor_base()->init_db_component(acreate);
	}

	void actor_base::start_broadcast()
	{
		impl_actor_base::start_broadcast();
	}

	int32_t actor_base::set_timer(const timerparm& aparm)
	{
		return m_impl_actor_base()->set_timer(aparm);
	}

	bool actor_base::isbroadcast()
	{
		return m_impl_actor_base()->isbroadcast();
	}

	void actor_base::set_broadcast(bool aisbroadcast)
	{
		m_impl_actor_base()->set_broadcast(aisbroadcast);
	}

	int actor_base::create_group()
	{
		return m_group.create_group();
	}

	void actor_base::remove_group(int agroupid)
	{
		m_group.remove_group(agroupid);
	}

	bool actor_base::add_group_member(int agroupid, i64_actorid amember)
	{
		return m_group.add_group_member(agroupid, amember);
	}

	void actor_base::remove_group_member(int agroupid, i64_actorid amember)
	{
		m_group.remove_group_member(agroupid, amember);
	}

	const std::set<i64_actorid>* actor_base::get_group(int agroupid)
	{
		return m_group.get_group(agroupid);
	}

	void actor_base::set_kcpssion(i32_session asession)
	{
		m_impl_actor_base()->set_kcpssion(asession);
	}

	i32_session actor_base::get_kcpssion()
	{
		return m_impl_actor_base()->get_kcpssion();
	}

	bool actor_base::iskcp()
	{
		static bool lkcp = (ttab_servers::nworks(ENET_KCP) != nullptr) ? true : false;
		return lkcp;
	}

	const char* actor_base::kcp_session()
	{
		return "";
	}

	bool actor_base::connect_kcp(int16_t anum, const std::string& aip, i16_port aprot)
	{
		if (iskcp() == false)
		{
			return false;
		}
		std::string lkcpsession = kcp_session();
		if (lkcpsession == "")
		{
			return false;
		}
		nets::kcp(anum)->connect(lkcpsession, id_guid(), aip, aprot, [this](i32_session asession)
			{
				set_kcpssion(asession);
				log_error()->print("kcp m_kcpsession = {}", get_kcpssion());
			});
		return true;
	}
}//namespace ngl