#include "actor_manage.h"
#include "ndb_modular.h"
#include "actor_base.h"
#include "actor_log.h"
#include "ndbclient.h"
#include "xml.h"
#include "net.h"

namespace ngl
{
	i64_actorid actor_base::actorclient_guid()
	{
		return actor_client::actorid();
	}

	//# 日志相关
	std::shared_ptr<np_actor_logitem> g_actor_nonelog = std::make_shared<np_actor_logitem>();

	std::shared_ptr<np_actor_logitem> actor_base::log_debug(const std::source_location& asource)
	{
		if (ELOG_DEBUG >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_DEBUG, type(), ELOG_LOCAL, asource);
		}
		else
		{
			return g_actor_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> actor_base::log_debug_net(const std::source_location& asource)
	{
		if (ELOG_DEBUG >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_DEBUG, type(), ELOG_NETWORK, asource);
		}
		else
		{
			return g_actor_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> actor_base::log_info(const std::source_location& asource)
	{
		if (ELOG_INFO >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_INFO, type(), ELOG_LOCAL, asource);
		}
		else
		{
			return g_actor_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> actor_base::log_info_net(const std::source_location& asource)
	{
		if (ELOG_INFO >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_INFO, type(), ELOG_NETWORK, asource);
		}
		else
		{
			return g_actor_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> actor_base::log_warn(const std::source_location& asource)
	{
		if (ELOG_WARN >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_WARN, type(), ELOG_LOCAL, asource);
		}
		else
		{
			return g_actor_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> actor_base::log_warn_net(const std::source_location& asource)
	{
		if (ELOG_WARN >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_WARN, type(), ELOG_NETWORK, asource);
		}
		else
		{
			return g_actor_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> actor_base::log_error(const std::source_location& asource)
	{
		if (ELOG_ERROR >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_ERROR, type(), ELOG_LOCAL, asource);
		}
		else
		{
			return g_actor_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> actor_base::log_error_net(const std::source_location& asource)
	{
		if (ELOG_ERROR >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_ERROR, type(), ELOG_NETWORK, asource);
		}
		else
		{
			return g_actor_nonelog;
		}
	}

	struct actor_base::impl_group
	{
		impl_group() = delete;
		impl_group(const impl_group&) = delete;
		impl_group& operator=(const impl_group&) = delete;

		struct ginfo
		{
			ENUM_ACTOR				m_actortype;
			std::set<i64_actorid>	m_actorlist;
		};
		std::map<int, ginfo>		m_group;
		int							m_currentoffset;
		actor_base*					m_actor;
	public:
		inline impl_group(actor_base* aactor):
			m_currentoffset(0),
			m_actor(aactor)
		{}

		inline int create_group(ENUM_ACTOR aactortype)
		{
			ginfo& linfo = m_group[++m_currentoffset];
			linfo.m_actortype = aactortype;
			linfo.m_actorlist.clear();
			return m_currentoffset;
		}

		inline void remove_group(int agroupid)
		{
			m_group.erase(agroupid);
		}

		inline bool add_group_member(int agroupid, i64_actorid amember)
		{
			ginfo* lginfo = tools::findmap(m_group, agroupid);
			if (lginfo == nullptr)
			{
				m_actor->log_error()->print(
					"add_group_member not find groupid[{}]",
					agroupid
				);
				return false;
			}
			nguid lguid(amember);
			ENUM_ACTOR ltype = lginfo->m_actortype;
			if (ltype != ACTOR_NONE)
			{
				if (lginfo->m_actortype != lguid.type())
				{
					m_actor->log_error()->print("m_actortype != lguid.type()==[{}]([{}]!=[{}])"
						, agroupid
						, (int)ltype
						, (int)lguid.type()
					);
					return false;
				}
			}
			lginfo->m_actorlist.insert(amember);
			return true;
		}

		inline void remove_group_member(int agroupid, i64_actorid amember)
		{
			ginfo* lginfo = tools::findmap(m_group, agroupid);
			if (lginfo == nullptr)
				return;
			lginfo->m_actorlist.erase(amember);
		}

		inline bool get_group(int agroupid, std::pair<std::set<i64_actorid>*, ENUM_ACTOR>& apair)
		{
			ginfo* lginfo = tools::findmap(m_group, agroupid);
			if (lginfo == nullptr)
				return false;
			apair.first = &lginfo->m_actorlist;
			apair.second = lginfo->m_actortype;
			return true;
		}
	};

	struct actor_base::impl_actor_base
	{
		nguid										m_guid;				// actor guid
		std::unique_ptr<actor_manage_dbclient>		m_dbclient;			// dbclient组件管理器
		bool										m_isload;			// 数据是否加载完成
		std::map<pbdb::ENUM_DB, ndb_component*>		m_dbcomponent;
		actor_base*									m_actor;

		i32_session									m_kcpsession;

		//# 间隔一段时间发起的全员(所有actor)广播
		//# 可以在这个广播里推送一些需要处理的任务,例如 保存数据
		//# 推送全员广播的 单位(毫秒)
		static int									m_broadcast;	
		//# 推送广播的定时器id
		static int									m_broadcasttimer;
		//# 是否接收广播消息
		bool										m_isbroadcast;			

		inline impl_actor_base(actor_base* aactor, const actorparmbase& aparm):
			m_kcpsession(-1),
			m_isbroadcast(false),
			m_actor(aactor)
		{
			m_guid		= nguid(aparm.m_type, aparm.m_area, aparm.m_id);
			m_dbclient	= nullptr;
			m_isload	= aparm.m_manage_dbclient;
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
			return (i64_actorid)(m_guid);
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
			}Catch;
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
			for (std::pair<const pbdb::ENUM_DB, ndb_component*>& item : m_dbcomponent)
			{
				item.second->init_data();
			}
		}

		inline void init_db_component(bool acreate)
		{
			if (acreate)
			{
				for (std::pair<const pbdb::ENUM_DB, ndb_component*>& item : m_dbcomponent)
				{
					item.second->create();
				}
			}
			else
			{
				for (std::pair<const pbdb::ENUM_DB, ndb_component*>& item : m_dbcomponent)
				{
					item.second->init();
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
				.m_fun = [](const wheel_node* anode)
				{
					auto pro = std::make_shared<np_actor_broadcast>();
					handle_pram lpram = handle_pram::create<
						np_actor_broadcast, false>(nguid::make(), nguid::make(), pro);
					actor_manage::getInstance().broadcast_task(lpram);
				}
			};
			m_broadcasttimer = (int32_t)twheel::wheel().addtimer(lparm);
		}
	};

	int actor_base::impl_actor_base::m_broadcast			= 10000;
	int actor_base::impl_actor_base::m_broadcasttimer		= -1;

	actor_base::actor_base(const actorparmbase& aparm)
	{
		m_impl_actor_base.make_unique(this, aparm);
		m_impl_group.make_unique(this);
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

	int actor_base::create_group(ENUM_ACTOR aactortype)
	{
		return m_impl_group()->create_group(aactortype);
	}

	void actor_base::remove_group(int agroupid)
	{
		m_impl_group()->remove_group(agroupid);
	}

	bool actor_base::add_group_member(int agroupid, i64_actorid amember)
	{
		return m_impl_group()->add_group_member(agroupid, amember);
	}

	void actor_base::remove_group_member(int agroupid, i64_actorid amember)
	{
		m_impl_group()->remove_group_member(agroupid, amember);
	}

	bool actor_base::get_group(int agroupid, std::pair<std::set<i64_actorid>*, ENUM_ACTOR>& apair)
	{
		return m_impl_group()->get_group(agroupid, apair);
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
			return false;
		std::string lkcpsession = kcp_session();
		if (lkcpsession == "")
			return false;
		
		nets::kcp(anum)->connect(lkcpsession, id_guid(), aip, aprot, [this](i32_session asession)
			{
				set_kcpssion(asession);
				log_error()->print("kcp m_kcpsession = {}", get_kcpssion());
			});
		return true;
	}
}//namespace ngl