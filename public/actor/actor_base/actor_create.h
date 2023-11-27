#pragma once

#include "actor_manage.h"
#include "actor_db_client.h"
#include "actor_timer.h"
#include "actor_enum.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "net.h"

namespace ngl
{

	// actor�л�����(��ʱactor�ڲ���ʱ�����޷�Ǩ��)
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

		static void actor_register();

		enum { ACTOR_TYPE = ACTOR_CREATE};

		// ��ָ��[Server]�ϴ���[Actor]
		template <typename T>
		static void switch_process_send(std::shared_ptr<actor_switch_process<T>>& pro)
		{
			// #### 2 �����actor_role����gateway
			ENUM_ACTOR ltype = (ENUM_ACTOR)actor_guid::type(pro->m_actor);
			if (ltype == ENUM_ACTOR::ACTOR_ROLE)
			{
				actor_guid lguid(pro->m_actor);
				actor_switch_process_role* lp = (actor_switch_process_role*)&(pro.get()->m_pram);
				actor_base::static_send_actor(actor_guid::make(ACTOR_GATEWAY, tab_self_area, lp->m_gatewayid),
					actor_guid::make(),
					pro);
			}
			//// #### 3 ����ȥ�Ľ���
			actor_base::static_send_actor(
				actor_guid::make(ACTOR_CREATE, tab_self_area, pro->m_toserverid),
				actor_guid::make(),
				pro);
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
				// #### 1 ����actorĿǰ���ڵĽ���
				actor_base::static_send_actor(
					actor_guid::make(ACTOR_CREATE, tab_self_area, aserverid),
					actor_guid::make(),
					pro);
			}
			else
			{
				switch_process_send(pro);
			}
		}

		template <typename T>
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_switch_process<T>& adata)
		{
			if (adata.m_toserverid == nconfig::m_nodeid)
			{
				actor_guid lguid(adata.m_actor);
				actor_base::create(lguid.type(), lguid.actordataid(), &adata.m_pram);
			}
			else if (adata.m_serverid == nconfig::m_nodeid)
			{
				actor_manage::getInstance().erase_actor_byid(adata.m_actor, [adata]()
					{
						std::shared_ptr<actor_switch_process<T>> pro(new actor_switch_process<T>(adata));
						actor_create::switch_process_send<T>(pro);
					});
			}
			return true;
		}

	};




}