#include "actor_manage.h"
#include "actor_base.h"
#include "actor_log.h"
#include "ndb_modular.h"
#include "ndbclient.h"
#include "xml.h"
#include "net.h"

namespace ngl
{
	std::shared_ptr<np_actor_logitem> actor_base::m_nonelog = std::make_shared<np_actor_logitem>();

	std::shared_ptr<np_actor_logitem> actor_base::log_debug(const std::source_location& asource)
	{
		if (ELOG_DEBUG >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_DEBUG, type(), ELOG_LOCAL, asource);
		}
		else
		{
			return m_nonelog;
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
			return m_nonelog;
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
			return m_nonelog;
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
			return m_nonelog;
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
			return m_nonelog;
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
			return m_nonelog;
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
			return m_nonelog;
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
			return m_nonelog;
		}
	}

	struct actor_base::impl_actor_base
	{
		nguid										m_guid;				// actor guid
		std::unique_ptr<actor_manage_dbclient>		m_dbclient;			// dbclient组件管理器
		bool										m_isload;			// 数据是否加载完成
		std::map<pbdb::ENUM_DB, ndb_component*>		m_dbcomponent;

		impl_actor_base(actor_base* aactor, const actorparmbase& aparm)
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

		void erase_actor_byid()
		{
			actor_manage::getInstance().erase_actor_byid(m_guid);
		}

		bool is_single()
		{
			return nguid::is_single(m_guid.type());
		}

		nguid& guid()
		{
			return m_guid;
		}

		i64_actorid id_guid()
		{
			return (i64_actorid)(m_guid);
		}

		i32_actordataid id()
		{
			return m_guid.actordataid();
		}

		i16_area area()
		{
			return m_guid.area();
		}

		ENUM_ACTOR type()
		{
			return m_guid.type();
		}

		bool isloadfinish()
		{
			return m_dbclient == nullptr || m_dbclient->isloadfinish();
		}

		void add_dbclient(ndbclient_base* adbclient, i64_actorid aid)
		{
			Try
			{
				Assert(m_dbclient != nullptr)
				m_dbclient->add(adbclient, aid);
			}Catch;
		}

		void save()
		{
			m_dbclient->save();
		}

		std::unique_ptr<actor_manage_dbclient>& get_actor_manage_dbclient()
		{
			return m_dbclient;
		}

		bool isload()
		{
			return m_isload;
		}

		void set_db_component(ndb_component* acomponent)
		{
			m_dbcomponent[acomponent->type()] = acomponent;
		}

		void db_component_init_data()
		{
			for (auto& item : m_dbcomponent)
			{
				item.second->init_data();
			}
		}

		void init_db_component(bool acreate)
		{
			if (acreate)
			{
				for (auto& item : m_dbcomponent)
				{
					item.second->create();
				}
			}
			else
			{
				for (auto& item : m_dbcomponent)
				{
					item.second->init();
				}
			}
		}
	};

	int actor_base::m_broadcast			= 10000;		// 推送全员广播的 单位(毫秒)
	int actor_base::m_broadcasttimer	= -1;			// 推送广播的定时器id

	std::vector<i32_serverid> actor_base::m_gatewayids;

	actor_base::actor_base(const actorparmbase& aparm) :
		m_isbroadcast(false)
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

	void actor_base::push_task_type(ENUM_ACTOR atype, handle_pram& apram, bool aotherserver/* = false*/)
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
		wheel_parm lparm
		{
			.m_ms = m_broadcast,
			.m_intervalms = [](int64_t) {return actor_base::m_broadcast; } ,
			.m_count = 0x7fffffff,
			.m_fun = [](wheel_node* anode)
			{
				nguid lguid;
				lguid.none();
				std::shared_ptr<np_actor_broadcast> pro(new np_actor_broadcast());
				handle_pram lpram;
				handle_pram::create<np_actor_broadcast, false>(lpram, lguid, lguid, pro);
				actor_manage::getInstance().broadcast_task(lpram);
			}
		};
		m_broadcasttimer = twheel::wheel().addtimer(lparm);
	}

	int32_t actor_base::set_timer(const timerparm& aparm)
	{
		std::shared_ptr<timerparm> lptr(new timerparm(aparm));
		return ntimer::addtimer(this, lptr);
	}

	bool actor_base::isbroadcast()
	{
		return m_isbroadcast;
	}

	void actor_base::set_broadcast(bool aisbroadcast)
	{
		m_isbroadcast = aisbroadcast;
	}

	// 创建一个群发分组(可以指定ActorType,主要是为了区分客户端与普通actor)
	int actor_base::add_group(ENUM_ACTOR aactortype/* = ACTOR_NONE*/)
	{
		group_info& linfo = m_group[++m_currentoffset];
		linfo.m_actortype = aactortype;
		linfo.m_actorlist.clear();
		return m_currentoffset;
	}

	// 移除一个分组
	void actor_base::remove_group(int agroupid)
	{
		m_group.erase(agroupid);
	}

	// 将成员加入某个群发分组
	bool actor_base::add_group_member(int agroupid, i64_actorid amember)
	{
		auto itor = m_group.find(agroupid);
		if (itor == m_group.end())
		{
			log_error()->print("add_group_member not find groupid[{}]", agroupid);
			return false;
		}

		nguid lguid(amember);
		ENUM_ACTOR ltype = itor->second.m_actortype;
		if (ltype != ACTOR_NONE)
		{
			if (itor->second.m_actortype != lguid.type())
			{
				log_error()->print("m_actortype != lguid.type()==[{}]([{}]!=[{}])"
					, agroupid
					, (int)ltype
					, (int)lguid.type()
				);
				return false;
			}
		}
		itor->second.m_actorlist.insert(amember);
		return true;
	}

	// 将成员从某个群发分组中移除
	void actor_base::remove_group_member(int agroupid, i64_actorid amember)
	{
		auto itor = m_group.find(agroupid);
		if (itor == m_group.end())
			return;
		itor->second.m_actorlist.erase(amember);
	}

	void actor_base::set_kcpssion(i32_session asession)
	{
		m_kcpsession = asession;
	}

	bool actor_base::iskcp()
	{
		static bool lkcp = (ttab_servers::nworks(ENET_KCP) != nullptr) ? true : false;
		return lkcp;
	}

	const char* actor_base::kcpsessionmd5()
	{
		return "";
	}

	bool actor_base::connect_kcp(int16_t anum, const std::string& aip, i16_port aprot)
	{
		if (iskcp() == false)
			return false;
		std::string lkcpsessionmd5 = kcpsessionmd5();
		if (lkcpsessionmd5 == "")
			return false;
		
		nets::kcp(anum)->connect(lkcpsessionmd5, id_guid(), aip, aprot, [this](i32_session asession)
			{
				m_kcpsession = asession;
				log_error()->print("kcp m_kcpsession = {}", m_kcpsession);
			});
		return true;
	}
}//namespace ngl