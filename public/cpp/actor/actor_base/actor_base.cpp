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
	bool nready::check_readybit(int32_t anum, int32_t aready)
	{
		if (((1 << anum) & aready) != 0)
		{
			auto itor = m_readyfun.find(1 << anum);
			if (itor == m_readyfun.end())
			{
				return true;
			}
			if (!itor->second())
			{
				return false;
			}
			m_readyfun.erase(itor);
		}
		return true;
	}

	bool nready::is_ready(int32_t aready /*= e_ready_all*/)
	{
		if (aready == e_ready_null || m_readyfun.empty())
		{
			return true;
		}
		if (aready == e_ready_all)
		{
			for (auto itor = m_readyfun.begin(); itor != m_readyfun.end();++itor)
			{
				if (!itor->second())
				{
					return false;
				}
				itor = m_readyfun.erase(itor);
			}
			return true;
		}
		for (int32_t i = 1; i <= 32; ++i)
		{
			if (!check_readybit(i, aready))
			{
				return false;
			}
		}
		return true;
	}

	void nready::set_ready(enum_ready aready, const std::function<bool()>& afun)
	{
		m_readyfun[aready] = afun;
	}

	void nready::set_readybycustom(const std::function<bool()>& afun)
	{
		if (m_custom > e_max_custom)
		{
			log_error()->print("set_readybycustom fail [{}]", m_custom);
			return;
		}
		auto lvalue = (enum_ready)(e_ready_custom << m_custom);
		if (m_readyfun.contains(lvalue))
		{
			log_error()->print("set_readybycustom fail [{}:{}]", m_custom, (int32_t)lvalue);
			return;
		}
		m_readyfun[lvalue] = afun;
		++m_custom;
	}

	int actor_base::m_broadcast = 10 * localtime::MILLISECOND;
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
			m_dbclient->set_loadfinish_function([this](pbdb::ENUM_DB atype, enum_dbstat astat)
				{
					loaddb_finish(atype, astat);
				});
			ready().set_ready(e_ready_db, [this]() 
				{
					return m_dbclient == nullptr || m_dbclient->isloadfinish();
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

	nready& actor_base::ready()
	{
		return m_ready;
	}

	void actor_base::erase_actor()
	{
		erase_actor_before();
		actor_manage::instance().erase_actor(m_guid);
	}

	bool actor_base::is_single()const
	{
		return enum_actor::is_signle(m_guid.type());
	}

	const nguid& actor_base::guid()const
	{
		return m_guid;
	}
	
	i64_actorid actor_base::id_guid()const
	{
		return m_guid;
	}

	i32_actordataid actor_base::id()const
	{
		return m_guid.actordataid();
	}

	i16_area actor_base::area()const
	{
		return m_guid.area();
	}

	ENUM_ACTOR actor_base::type()const
	{
		return m_guid.type();
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

	actor_base::~actor_base(){}

	void actor_base::erase_actor(const nguid& aguid)
	{
		actor_manage::instance().erase_actor(aguid);
	}

	bool actor_base::nscript_using()const
	{
		return m_script != nullptr && m_enscript != enscript_none;
	}

	bool actor_base::nscript_db_loadfinish()const
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

	bool actor_base::send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		i32_session lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return nets::send_pack(lsession, apack);
	}

	bool actor_base::send(i32_sessionid asession, std::shared_ptr<pack>& apack)
	{
		return nets::send_pack(asession, apack);
	}

	void actor_base::send_actor(const nguid& aguid, const std::shared_ptr<pack>& adata)
	{
		handle_pram lpram = handle_pram::create(aguid, nguid::make(), adata);
		actor_manage::instance().push_task_id(aguid, lpram);
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
				handle_pram lpram = handle_pram::create<np_actor_broadcast, false>(nguid::make(), nguid::make(), pro);
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

	bool actor_base::isbroadcast()const
	{
		return m_isbroadcast;
	}

	void actor_base::set_broadcast(bool aisbroadcast)
	{
		m_isbroadcast = aisbroadcast;
	}

	bool actor_base::connect_kcp(int16_t anum, const std::string& aip, i16_port aprot, i64_actorid aactoridserver, std::string& akcpsession)const
	{
		if (nconfig::node_type() != ROBOT)
		{//不允许服务器主动进行kcp连接
			return false;
		}
		nets::kcp(anum)->connect(akcpsession, aactoridserver, id_guid(), aip, aprot, [this](i32_session asession)
			{
				log_error()->print("kcp {} m_kcpsession = {}", (nguid)id_guid(), asession);
			}
		);
		return true;
	}

	bool actor_base::sendpack_kcp(i64_actorid aactorid, std::shared_ptr<pack>& adata, i16_port auport/* = 0*/)
	{
		ukcp* lpukcp = nets::kcp(auport);
		if (lpukcp == nullptr)
		{
			return false;
		}
		return lpukcp->send_server(aactorid, adata);
	}

	bool actor_base::sendpack_kcp(const std::set<i64_actorid>& aactorids, std::shared_ptr<pack>& adata, i16_port auport/* = 0*/)
	{
		ukcp* lpukcp = nets::kcp(auport);
		if (lpukcp == nullptr)
		{
			return false;
		}
		return lpukcp->send_server(aactorids, adata);
	}
}//namespace ngl