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
		return actor_manage::instance().get_clientguid();
	}

	//# 日志相关
	const std::shared_ptr<nactor_logitem> g_actor_nonelog = std::make_shared<nactor_logitem>();

	tools_log::tools_log(actor_base* aactor /*= nullptr*/) :
		m_actor(aactor)
	{}

	void tools_log::set_logactor(actor_base* aactor)
	{
		m_actor = aactor;
	}

	std::shared_ptr<nactor_logitem> tools_log::get_log(const std::source_location& asource, ELOGLEVEL aloglevel, bool anet)const
	{
		if(m_actor == nullptr || !nactor_logitem::check_level(aloglevel))
		{
			return g_actor_nonelog;
		}
		return std::make_shared<nactor_logitem>(aloglevel, m_actor->type(), anet ? ELOG_NETWORK : ELOG_LOCAL, asource);
	}

	std::shared_ptr<nactor_logitem> tools_log::log_debug(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_DEBUG, false);
	}

	std::shared_ptr<nactor_logitem> tools_log::log_debug_net(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_DEBUG, true);
	}

	std::shared_ptr<nactor_logitem> tools_log::log_info(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_INFO, false);
	}

	std::shared_ptr<nactor_logitem> tools_log::log_info_net(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_INFO, true);
	}

	std::shared_ptr<nactor_logitem> tools_log::log_warn(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_WARN, false);
	}

	std::shared_ptr<nactor_logitem> tools_log::log_warn_net(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_WARN, true);
	}

	std::shared_ptr<nactor_logitem> tools_log::log_error(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_ERROR, false);
	}

	std::shared_ptr<nactor_logitem> tools_log::log_error_net(const std::source_location& asource/* = std::source_location::current()*/)const
	{
		return get_log(asource, ELOG_ERROR, true);
	}

	struct actor_base::impl_actor_base
	{
		nguid										m_guid = nguid::make();			// actor guid
		std::unique_ptr<actor_manage_dbclient>		m_dbclient = nullptr;			// dbclient组件管理器
		bool										m_isload = false;				// 数据是否加载完成
		std::map<pbdb::ENUM_DB, ndb_component*>		m_dbcomponent;
		actor_base*									m_actor = nullptr;

		i32_session									m_kcpsession = -1;

		//# 间隔一段时间发起的全员(所有actor)广播
		//# 可以在这个广播里推送一些需要处理的任务,例如 保存数据
		//# 推送全员广播的 单位(毫秒)
		static int									m_broadcast;	
		//# 推送广播的定时器id
		static int									m_broadcasttimer;
		//# 是否接收广播消息
		bool										m_isbroadcast = false;	

		std::shared_ptr<nscript>					m_script = nullptr;

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
			if (aparm.m_enscript != enscript_none)
			{
				m_script = nscript::malloc_script(aparm.m_enscript, aactor);

				const char* lactorname = em<ENUM_ACTOR>::get_name(type());
				m_script->init(lactorname, std::format("{}.lua", lactorname).c_str());

				nscript_sysdata lsysdata
				{
					.m_nguid = tools::lexical_cast<std::string>(id_guid())
				};
				std::string lsysdatajson;
				tools::custom2json(lsysdata, lsysdatajson);
				m_script->init_sysdata(lsysdatajson.c_str());
			}
		}

		inline void erase_actor()const
		{
			m_actor->erase_actor_before();
			actor_manage::instance().erase_actor(m_guid);
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
			if (m_dbclient == nullptr)
			{
				tools::no_core_dump();
				return;
			}
			m_dbclient->add(adbclient, aid);
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
			for (const auto& [_, ldbcomponent] : m_dbcomponent)
			{
				ldbcomponent->init_data();
			}
		}

		inline void init_db_component(bool acreate)const
		{
			for (const auto& [key, value] : m_dbcomponent)
			{
				if (acreate)
				{
					value->create();
				}
				else
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

		inline int32_t set_timer(const np_timerparm& aparm)
		{
			auto lparm = std::make_shared<np_timerparm>(aparm);
			return ntimer::addtimer(m_actor, lparm);
		}

		inline bool nscript_using()
		{
			return m_script != nullptr;
		}

		inline bool nscript_push_data(const char* adbname, const char* adata_source, const char* adatajson, bool aedit)
		{
			if (!nscript_using())
			{
				return false;
			}
			return m_script->push_data(adbname, adata_source, adatajson, aedit);
		}

		inline bool nscript_handle(const char* aname, const char* ajson)
		{
			if (!nscript_using())
			{
				return false;
			}
			return m_script->handle(aname, ajson);
		}

		inline bool nscript_check_outdata(const char* adbname, i64_accountid aactorid, std::string& adatajson)
		{
			if (!nscript_using())
			{
				return false;
			}
			return m_script->check_outdata(adbname, aactorid, adatajson);
		}

		inline bool nscript_check_outdata_del(const char* adbname, i64_actorid aactorid)
		{
			if (!nscript_using())
			{
				return false;			
			}
			std::string ldatajson;
			return m_script->check_outdata_del(adbname, aactorid, ldatajson);
		}

		inline bool nscript_check_outdata_del(const char* adbname, std::vector<int64_t>& avec)
		{
			if (!nscript_using())
			{
				return false;
			}

			std::string ljson;
			if (!m_script->check_outdata_del(adbname, -1, ljson))
			{
				return false;
			}

			json_read lread(ljson.c_str());
			std::vector<std::string> lvec;
			if (!lread.read(adbname, lvec))
			{
				return false;
			}
			for (std::string& item : lvec)
			{
				avec.push_back(tools::lexical_cast<int64_t>(item));
			}
			return true;
		}

		inline void nscript_db_loadfinish()
		{
			if (!nscript_using())
			{
				return;
			}
			m_script->db_loadfinish();
		}

		std::map<std::string, std::function<void(const char*)>> m_dbnsp;

		inline void nscript_correlation_dbnsp(
			const char* aname
			, const std::function<void(const char*)>& afun
		)
		{
			if (!nscript_using())
			{
				return;
			}
			m_dbnsp[aname] = afun;
		}

		inline void nscript_change_dbnsp()
		{
			if (!nscript_using())
			{
				return;
			}
			std::string ljson;
			m_script->dbnsp_auto_save(ljson);
			for (const auto& apair : m_dbnsp)
			{
				apair.second(ljson.c_str());
			}
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
					handle_pram lpram = handle_pram::create<np_actor_broadcast, false>(
						nguid::make(), nguid::make(), pro
					);
					actor_manage::instance().broadcast_task(lpram);
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

	void actor_base::erase_actor()
	{
		m_impl_actor_base()->erase_actor();
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

	void actor_base::erase_actor(const nguid& aguid)
	{
		actor_manage::instance().erase_actor(aguid);
	}

	void actor_base::push_task_id(const nguid& aguid, handle_pram& apram)
	{
		actor_manage::instance().push_task_id(aguid, apram);
	}

	void actor_base::push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram)
	{
		actor_manage::instance().push_task_id(asetguid, apram);
	}

	void actor_base::push_task_type(ENUM_ACTOR atype, handle_pram& apram)
	{
		actor_manage::instance().push_task_type(atype, apram);
	}

	void actor_base::push_task_id(handle_pram& apram)
	{
		actor_manage::instance().push_task_id(m_impl_actor_base()->m_guid, apram);
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

	bool actor_base::nscript_using()
	{
		return m_impl_actor_base()->nscript_using();
	}

	bool actor_base::nscript_push_data(const char* adbname, const char* adata_source, const char* adatajson, bool aedit /*= false*/)
	{
		return m_impl_actor_base()->nscript_push_data(adbname, adata_source, adatajson, aedit);
	}

	bool actor_base::nscript_handle(const char* aname, const char* ajson)
	{
		return m_impl_actor_base()->nscript_handle(aname, ajson);
	}

	bool actor_base::nscript_check_outdata(const char* adbname, i64_actorid aactorid, std::string& adatajson)
	{
		return m_impl_actor_base()->nscript_check_outdata(adbname, aactorid, adatajson);
	}

	bool actor_base::nscript_check_outdata_del(const char* adbname, i64_actorid aactorid)
	{
		return m_impl_actor_base()->nscript_check_outdata_del(adbname, aactorid);
	}

	bool actor_base::nscript_check_outdata_del(const char* adbname, std::vector<i64_actorid>& aactorid)
	{
		return m_impl_actor_base()->nscript_check_outdata_del(adbname, aactorid);
	}

	void actor_base::nscript_db_loadfinish()
	{
		return m_impl_actor_base()->nscript_db_loadfinish();
	}

	void actor_base::nscript_correlation_dbnsp(
		const char* aname
		, const std::function<void(const char*)>& afun
	)
	{
		return m_impl_actor_base()->nscript_correlation_dbnsp(aname, afun);
	}

	void actor_base::nscript_change_dbnsp()
	{
		return m_impl_actor_base()->nscript_change_dbnsp();
	}

	void actor_base::start_broadcast()
	{
		impl_actor_base::start_broadcast();
	}

	int32_t actor_base::set_timer(const np_timerparm& aparm)
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

	int32_t actor_base::create_group(ENUM_ACTOR atype/*= ACTOR_NONE*/)
	{
		return m_group.create(atype);
	}

	void actor_base::remove_group(int32_t agroupid)
	{
		m_group.remove(agroupid);
	}

	bool actor_base::add_group_member(int32_t agroupid, i64_actorid amember)
	{
		return m_group.add_member(agroupid, amember);
	}

	void actor_base::remove_group_member(int32_t agroupid, i64_actorid amember)
	{
		m_group.remove_member(agroupid, amember);
	}

	const std::set<i64_actorid>* actor_base::get_group(int32_t agroupid)
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
		static bool lkcp = (ttab_servers::instance().nworks(ENET_KCP) != nullptr) ? true : false;
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