#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_role.h"
#include "actor_timer.h"
#include "net.pb.h"

namespace ngl
{
	class actor_manage_role : public actor
	{
		actor_manage_role() :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = ACTOR_MANAGE_ROLE,
						.m_area = ttab_servers::tab()->m_area,
						.m_id = nconfig::m_nodeid
					},
					.m_weight = 0x7fffffff,
				})
		{
		}
	public:
		friend class actor_instance<actor_manage_role>;
		static actor_manage_role& getInstance() 
		{ 
			return actor_instance<actor_manage_role>::instance(); 
		}

		virtual ~actor_manage_role() {}

		static void actor_register();

		enum { ACTOR_TYPE = ACTOR_MANAGE_ROLE};

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ROLE_LOGIN& adata);
	private:
	};
}

