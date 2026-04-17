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
// File overview: Implements gateway-to-client protocol serialization and delivery.


#include "actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h"
#include "actor/actor_base/core/nregister.h"
#include "actor/actor_logic/nforward.h"

namespace ngl
{
	actor_gateway_g2c::actor_gateway_g2c() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY_G2C,
					.m_area = tab_self_area,
					.m_id = nconfig.tcount(),
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
			})
	{
	}

	ENUM_ACTOR actor_gateway_g2c::actor_type()
	{
		return ACTOR_GATEWAY_G2C;
	}

	i64_actorid actor_gateway_g2c::actorid(i32_actordataid aactordataid)
	{
		return nguid::make(ACTOR_GATEWAY_G2C, tab_self_area, aactordataid);
	}

	void actor_gateway_g2c::init()
	{
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_gateway_g2c::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_gateway_g2c::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_gateway_g2c::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_gateway_g2c::nregister()
	{
		actor::register_timer<actor_gateway_g2c>(&actor_gateway_g2c::timer_handle);

		register_handle<actor_gateway_g2c
			, np_actor_gatewayinfo_updata
		>();

		nforward::g2c();
	}

	void actor_gateway_g2c::get_allclient(std::map<i32_sessionid, i64_actorid>& amap)
	{
		for (const auto& lpair : m_info.info())
		{
			for (const auto& litem : lpair.second)
			{
				const gateway_socket& lsocket = litem.second;
				if (lsocket.m_socket <= 0)
				{
					continue;
				}
				const i64_actorid lactorid = nguid::make(ACTOR_ROBOT, lsocket.m_area, lsocket.m_dataid);
				amap.emplace(lsocket.m_socket, lactorid);
			}
		}
	}

	void actor_gateway_g2c::get_allclientbyarea(std::map<i32_sessionid, i64_actorid>& amap, i16_area aarea)
	{
		const auto* lpmap = tools::findmap(m_info.info(), aarea);
		if (lpmap == nullptr)
		{
			return;
		}
		for (const auto& litem : *lpmap)
		{
			const gateway_socket& lsocket = litem.second;
			if (lsocket.m_socket <= 0)
			{
				continue;
			}
			const i64_actorid lactorid = nguid::make(ACTOR_ROBOT, lsocket.m_area, lsocket.m_dataid);
			amap.emplace(lsocket.m_socket, lactorid);
		}
	}

	bool actor_gateway_g2c::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl
