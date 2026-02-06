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
#pragma once

#include "actor_manage.h"
#include "nactortype.h"
#include "ndbclient.h"
#include "db_data.h"
#include "nmysql_pool.h"
#include "nmysql.h"
#include "ntimer.h"
#include "ntcp.h"

namespace ngl
{
	// actor切换进程
	// actor内部临时数据无法迁移
	class actor_create : 
		public actor
	{
		actor_create(const actor_create&) = delete;
		actor_create& operator=(const actor_create&) = delete;

		actor_create();
	public:
		friend class actor_instance<actor_create>;
		static actor_create& instance()
		{
			return actor_instance<actor_create>::instance();
		}

		virtual ~actor_create() = default;

		static void nregister();

		static i64_actorid actorid(i32_serverid aserverid);

		// # 在指定[Server]上创建[Actor]
		template <typename T>
		static void switch_process_send(std::shared_ptr<np_actorswitch_process<T>>& pro)
		{
			// # 2 如果是actor_role发给gateway
			ENUM_ACTOR ltype = (ENUM_ACTOR)nguid::type(pro->m_actor);
			if (ltype == ENUM_ACTOR::ACTOR_ROLE)
			{
				nguid lguid(pro->m_actor);
				auto lp = (np_actorswitch_process_role*)&(pro.get()->m_pram);
				i64_actorid lactorgatewayid = nguid::make(ACTOR_GATEWAY, tab_self_area, nnodeid::tcount(lp->m_gatewayid));
				send_actor(lactorgatewayid, nguid::make(), pro);
			}
			// # 3 发给去的进程
			i64_actorid lactortoserverid = actor_create::actorid(nnodeid::tcount(pro->m_toserverid));
			send_actor(lactortoserverid, nguid::make(), pro);
		}

		template <typename T>
		static void switch_process(i64_actorid aactor, i32_serverid aserverid, i32_serverid atoserverid, T& adata)
		{
			if (aserverid == atoserverid)
			{
				return;
			}

			auto pro = std::make_shared<np_actorswitch_process<T>>();
			pro->m_actor		= aactor;
			pro->m_serverid		= aserverid;
			pro->m_toserverid	= atoserverid;
			pro->m_pram			= adata;

			if (aserverid > 0)
			{
				// # 1 发给actor目前所在的进程
				i64_actorid lcreateactor = actor_create::actorid(nnodeid::tcount(aserverid));
				send_actor(lcreateactor, nguid::make(), pro);
			}
			else
			{
				switch_process_send(pro);
			}
		}

		template <typename T>
		bool handle(const message<np_actorswitch_process<T>>& adata)
		{
			auto lparm = adata.get_data();
			if (lparm->m_toserverid == nconfig.nodeid())
			{
				nguid lguid(lparm->m_actor);
				actor_base::create(lguid.type(), lguid.area(), lguid.actordataid(), (void*)&lparm->m_pram);
			}
			else if (lparm->m_serverid == nconfig.nodeid())
			{
				actor_manage::instance().erase_actor(lparm->m_actor, [lparm]()
					{
						auto pro = std::make_shared<np_actorswitch_process<T>>(*lparm);
						actor_create::switch_process_send<T>(pro);
					}
				);
			}
			return true;
		}
	};
}//namespace ngl