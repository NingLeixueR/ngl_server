#pragma once

#include "actor_manage.h"
#include "ndbclient.h"
#include "actor_timer.h"
#include "nactortype.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "net.h"

namespace ngl
{
	// actor切换进程(暂时actor内部临时数据无法迁移)
	class actor_create : public actor
	{
		actor_create();
	public:
		friend class actor_instance<actor_create>;
		static actor_create& getInstance()
		{
			return actor_instance<actor_create>::instance();
		}

		virtual ~actor_create() {}

		static void nregister();

		// 在指定[Server]上创建[Actor]
		template <typename T>
		static void switch_process_send(std::shared_ptr<actor_switch_process<T>>& pro)
		{
			// #### 2 如果是actor_role发给gateway
			ENUM_ACTOR ltype = (ENUM_ACTOR)nguid::type(pro->m_actor);
			if (ltype == ENUM_ACTOR::ACTOR_ROLE)
			{
				nguid lguid(pro->m_actor);
				actor_switch_process_role* lp = (actor_switch_process_role*)&(pro.get()->m_pram);
				i64_actorid lactorgatewayid = nguid::make(ACTOR_GATEWAY, tab_self_area, lp->m_gatewayid);
				actor_base::static_send_actor(lactorgatewayid, nguid::make(), pro);
			}
			//// #### 3 发给去的进程
			i64_actorid lactortoserverid = nguid::make(ACTOR_CREATE, tab_self_area, pro->m_toserverid);
			actor_base::static_send_actor(lactortoserverid, nguid::make(), pro);
		}

		template <typename T>
		static void switch_process(i64_actorid aactor, i32_serverid aserverid, i32_serverid atoserverid, T& adata)
		{
			if (aserverid == atoserverid)
				return;

			std::shared_ptr<actor_switch_process<T>> pro(new actor_switch_process<T>
				{
					.m_actor = aactor,
					.m_serverid = aserverid,
					.m_toserverid = atoserverid,
					.m_pram = adata,
				});

			if (aserverid > 0)
			{
				// #### 1 发给actor目前所在的进程
				i64_actorid lcreateactor = nguid::make(ACTOR_CREATE, tab_self_area, aserverid);
				actor_base::static_send_actor(lcreateactor, nguid::make(), pro);
			}
			else
			{
				switch_process_send(pro);
			}
		}

		template <typename T>
		bool handle(message<actor_switch_process<T>>& adata)
		{
			auto lparm = adata.m_data;
			if (lparm->m_toserverid == nconfig::m_nodeid)
			{
				nguid lguid(lparm->m_actor);
				actor_base::create(lguid.type(), lguid.actordataid(), &lparm->m_pram);
			}
			else if (lparm->m_serverid == nconfig::m_nodeid)
			{
				actor_manage::getInstance().erase_actor_byid(lparm->m_actor, [lparm]()
					{
						std::shared_ptr<actor_switch_process<T>> pro(new actor_switch_process<T>(*lparm));
						actor_create::switch_process_send<T>(pro);
					});
			}
			return true;
		}
	};
}