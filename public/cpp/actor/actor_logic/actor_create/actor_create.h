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
// File overview: Declares interfaces for actor create.

#pragma once

#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/nactortype.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	// Actorswitch
	// Actor datacannot
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

		// # Specified[Server]oncreate[Actor]
		template <typename T>
		static void switch_process_send(std::shared_ptr<np_actorswitch_process<T>>& pro)
		{
			// # 2 If actor_role togateway
			ENUM_ACTOR ltype = (ENUM_ACTOR)nguid::type(pro->m_actor);
			if (ltype == ENUM_ACTOR::ACTOR_ROLE)
			{
				nguid lguid(pro->m_actor);
				auto lp = (np_actorswitch_process_role*)&(pro.get()->m_pram);
				i64_actorid lactorgatewayid = nguid::make(ACTOR_GATEWAY, tab_self_area, nnodeid::tcount(lp->m_gatewayid));
				send_actor(lactorgatewayid, nguid::make(), pro);
			}
			// # 3 To
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
				// # 1 Toactor before
				i64_actorid lcreateactor = actor_create::actorid(nnodeid::tcount(aserverid));
				send_actor(lcreateactor, nguid::make(), pro);
			}
			else
			{
				switch_process_send(pro);
			}
		}

		template <typename T>
		bool handle([[maybe_unused]] const message<np_actorswitch_process<T>>& adata)
		{
			const auto* lparm = adata.get_data();
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
