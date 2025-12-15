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
#include "actor_gateway_g2c.h"
#include "nregister.h"
#include "nforward.h"

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
					.m_id = nconfig::m_tcount,
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

		register_handle<actor_gateway_g2c>::func<
			np_actor_gatewayinfo_updata
		>(e_ready_all);

		nforward::g2c();
	}

	void actor_gateway_g2c::get_allclient(std::map<i32_sessionid, i64_actorid>& amap)
	{
		for (const auto& itemfor1 : m_info.info())
		{
			for (const auto& itemfor2 : itemfor1.second)
			{
				i64_actorid lactorid = nguid::make(ACTOR_ROBOT, itemfor2.second.m_area, itemfor2.second.m_dataid);
				amap.insert(std::make_pair(itemfor2.second.m_socket, lactorid));
			}
		}
	}

	void actor_gateway_g2c::get_allclientbyarea(std::map<i32_sessionid, i64_actorid>& amap, i16_area aarea)
	{
		auto itemitor = m_info.info().find(aarea);
		if (itemitor != m_info.info().end())
		{
			for (const auto& itemfor1 : itemitor->second)
			{
				i64_actorid lactorid = nguid::make(ACTOR_ROBOT, itemfor1.second.m_area, itemfor1.second.m_dataid);
				amap.insert(std::make_pair(itemfor1.second.m_socket, lactorid));
			}
		}
	}

	bool actor_gateway_g2c::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_gateway_g2c::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl