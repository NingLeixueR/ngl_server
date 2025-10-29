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
	int actor_base::m_broadcast = 10000;
	int actor_base::m_broadcasttimer = -1;

	i64_actorid actor_base::actorclient_guid()
	{
		return actor_manage::instance().get_clientguid();
	}

	actor_base::actor_base(const actorparmbase& aparm):
		m_guid(aparm.m_type, aparm.m_area, aparm.m_id)
		, m_isload(aparm.m_manage_dbclient)
	{
		if (aparm.m_manage_dbclient)
		{
			m_dbclient = std::make_unique<actor_manage_dbclient>(this);
			m_dbclient->set_loadfinish_function([this](bool adbishave)
				{
					loaddb_finish(adbishave);
				});
		}
		if (aparm.m_enscript != enscript_none)
		{
			m_enscript = aparm.m_enscript;
			const char* lactorname = em<ENUM_ACTOR>::get_name(type());
			m_script = nscript_manage::malloc(m_enscript, lactorname, std::format("{}.lua", lactorname).c_str());
			if (m_script != nullptr)
			{
				nscript_sysdata lsysdata
				{
					.m_nguid = id_guid()
				};
				nscript_manage::init_sysdata(m_enscript, m_script, lsysdata);
			}
		}
	}

	void actor_base::erase_actor()
	{
		erase_actor_before();
		actor_manage::instance().erase_actor(m_guid);
	}

	bool actor_base::is_single()
	{
		return enum_actor::is_signle(m_guid.type());
	}

	nguid& actor_base::guid()
	{
		return m_guid;
	}
	
	i64_actorid actor_base::id_guid()
	{
		return m_guid;
	}

	i32_actordataid actor_base::id()
	{
		return m_guid.actordataid();
	}

	i16_area actor_base::area()
	{
		return m_guid.area();
	}

	ENUM_ACTOR actor_base::type()
	{
		return m_guid.type();
	}

	bool actor_base::isloadfinish()
	{
		return m_dbclient == nullptr || m_dbclient->isloadfinish();
	}

	void actor_base::add_dbclient(ndbclient_base* adbclient, i64_actorid aid)
	{
		if (m_dbclient == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		m_dbclient->add(adbclient, aid);
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
		actor_manage::instance().push_task_id(id_guid(), apram);
	}

	bool actor_base::nscript_using()
	{
		return m_script != nullptr && m_enscript != enscript_none;
	}

	bool actor_base::nscript_db_loadfinish()
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::db_loadfinish(m_enscript, m_script);
	}

	void actor_base::save()
	{
		m_dbclient->save();
	}

	std::unique_ptr<actor_manage_dbclient>& actor_base::get_actor_manage_dbclient()
	{
		return m_dbclient;
	}

	bool actor_base::isload()
	{
		return m_isload;
	}

	void actor_base::set_db_component(ndb_component* acomponent)
	{
		m_dbcomponent[acomponent->type()] = acomponent;
	}

	void actor_base::db_component_init_data()
	{
		for (const auto& [_, ldbcomponent] : m_dbcomponent)
		{
			ldbcomponent->init_data();
		}
	}

	void actor_base::init_db_component(bool acreate)
	{
		log_error()->print("[{}] init_db_component [{}]", (nguid)id_guid(), acreate?"true":"false");

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

	void actor_base::start_broadcast()
	{
		wheel_parm lparm
		{
			.m_ms = m_broadcast,
			.m_intervalms = [](int64_t)
			{
				return actor_base::m_broadcast;
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

	int32_t actor_base::set_timer(const np_timerparm& aparm)
	{
		auto lparm = std::make_shared<np_timerparm>(aparm);
		return ntimer::addtimer(this, lparm);
	}

	bool actor_base::isbroadcast()
	{
		return m_isbroadcast;
	}

	void actor_base::set_broadcast(bool aisbroadcast)
	{
		m_isbroadcast = aisbroadcast;
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
		m_kcpsession = asession;
	}

	i32_session actor_base::get_kcpssion()
	{
		return m_kcpsession;
	}

	bool actor_base::iskcp()
	{
		net_works lnetwork;
		
		static bool lkcp = ttab_servers::instance().get_nworks(ENET_KCP, lnetwork);
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