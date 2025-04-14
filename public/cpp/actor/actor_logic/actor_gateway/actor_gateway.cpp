﻿#include "actor_gateway_c2g.h"
#include "actor_gateway_g2c.h"
#include "actor_gateway.h"
#include "actor_server.h"
#include "nregister.h"

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
					.m_id	= nconfig::m_nodeid, 
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
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

	void actor_gateway::nregister()
	{
		register_handle_custom<actor_gateway>::func<
			np_actorrole_login
			, np_gateway_close_session
			, np_actorswitch_process<np_actorswitch_process_role>
			, np_actor_session_close
			, np_actor_kcp
		>(false);

		register_handle_proto<actor_gateway>::func<
			pbnet::PROBUFF_NET_ROLE_LOGIN
			, pbnet::PROBUFF_NET_KCPSESSION
		>(false);
	}

	void actor_gateway::sync_actorserver_gatewayid(const nguid& aguid, bool aisremove)
	{
		np_actor_gatewayid_updata pro
		{
			.m_isremove = aisremove,
			.m_actorid = nguid::make(ACTOR_ROLE, aguid.area(), aguid.actordataid()),
			.m_gatewayid = ttab_servers::tab()->m_id,
		};

		for (i32_serverid iserverid : ttab_servers::tab()->m_actorserver)
		{
			i64_actorid lactorserve = actor_server::actorid();
			actor_base::send_server(iserverid, pro, lactorserve, nguid::make());
		}
	}

	void actor_gateway::update_gateway_info(const std::shared_ptr<np_actor_gatewayinfo_updata>& apro)
	{	
		send_actor(actor_gateway_c2g::actorid(id()), apro);
		send_actor(actor_gateway_g2c::actorid(id()), apro);
	}

	void actor_gateway::session_close(gateway_socket* ainfo)
	{
		Assert(ainfo != nullptr);

		ainfo->m_socket = 0;
		i32_actordataid lroleid = ainfo->m_dataid;
		i16_area larea = ainfo->m_area;

		wheel_parm lparm
		{
			.m_ms = sysconfig::sessionwait() * 1000,
			.m_intervalms = [](int64_t) {return sysconfig::sessionwait() * 1000; } ,
			.m_count = 1,
			.m_fun = [this, lroleid, larea](const wheel_node* anode)
			{
				auto pro = std::make_shared<np_gateway_close_session>(np_gateway_close_session
					{
						.m_roleid = lroleid,
						.m_area = larea,
					});
				send_actor(id_guid(), pro);
			}
		};
		twheel::wheel().addtimer(lparm);

		sync_actorserver_gatewayid(nguid::make(ACTOR_ROLE, larea, lroleid), true);
	}	
}//namespace ngl