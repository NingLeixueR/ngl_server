/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements gateway actor construction and client session management.


#include "actor/actor_logic/actor_gateway_c2g/actor_gateway_c2g.h"
#include "actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h"
#include "actor/actor_logic/actor_gateway/actor_gateway.h"
#include "actor/actor_logic/actor_server/actor_server.h"
#include "actor/actor_base/core/nregister.h"

namespace ngl
{
	actor_gateway::actor_gateway() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY,
					.m_area = tab_self_area,
					.m_id	= nconfig.tcount(),
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff,
				.m_normalwarn = 0x7fffffff,
				.m_highwarn = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_gateway::actor_type()
	{
		return ACTOR_GATEWAY;
	}

	i64_actorid actor_gateway::actorid(i32_actordataid aactordataid)
	{
		return nguid::make(actor_type(), tab_self_area, aactordataid);
	}

	void actor_gateway::init()
	{
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_gateway::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_gateway::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_gateway::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_gateway::nregister()
	{
		actor::register_timer<actor_gateway>(&actor_gateway::timer_handle);

		register_handle<actor_gateway
			, np_actorrole_login
			, np_gateway_close_session
			, np_actorswitch_process<np_actorswitch_process_role>
			, np_actor_session_close
			, np_actor_kcp
			, pbnet::PROBUFF_NET_ROLE_LOGIN
			, pbnet::PROBUFF_NET_KCPSESSION
		>();
	}

	void actor_gateway::sync_actorserver_gatewayid(const nguid& aguid, bool aisremove)
	{
		np_actor_gatewayid_updata pro
		{
			.m_isremove = aisremove,
			.m_actorid = nguid::make(ACTOR_ROLE, aguid.area(), aguid.actordataid()),
			.m_gatewayid = nconfig.nodeid(),
		};

		for (int32_t tid : ttab_servers::instance().const_tab()->m_actorserver)
		{
			i64_actorid lactorserve = actor_server::actorid();
			actor::send_server(nnodeid::nodeid(static_cast<int16_t>(tid), 1), pro, lactorserve, nguid::make());
		}
	}

	void actor_gateway::update_gateway_info(const std::shared_ptr<np_actor_gatewayinfo_updata>& apro)
	{	
		actor::send_actor(actor_gateway_c2g::actorid(id()), id_guid(), apro);
		actor::send_actor(actor_gateway_g2c::actorid(id()), id_guid(), apro);
	}

	void actor_gateway::session_close(gateway_socket* ainfo)
	{
		if (ainfo == nullptr)
		{
			tools::no_core_dump();
			return;
		}

		ainfo->m_socket = 0;
		i32_actordataid lroleid = ainfo->m_dataid;
		i16_area larea = ainfo->m_area;

		tools::wheel_parm lparm
		{
			.m_ms = sysconfig::sessionwait() * tools::time::MILLISECOND,
			.m_intervalms = [](int64_t) {return sysconfig::sessionwait() * tools::time::MILLISECOND; } ,
			.m_count = 1,
				.m_fun = [this, lroleid, larea](const tools::wheel_node*)
				{
					auto pro = std::make_shared<np_gateway_close_session>(np_gateway_close_session
					{
						.m_roleid = lroleid,
						.m_area = larea,
					}
				);
				actor::send_actor(id_guid(), nguid::make(), pro);
			}
		};
		tools::twheel::wheel().addtimer(lparm);

		sync_actorserver_gatewayid(nguid::make(ACTOR_ROLE, larea, lroleid), true);
	}	

	bool actor_gateway::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl
