#pragma once

#include "actor_manage.h"
#include "actor_role.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_manage_role : public actor
	{
		actor_manage_role(const actor_manage_role&) = delete;
		actor_manage_role& operator=(const actor_manage_role&) = delete;

		actor_manage_role();
	public:
		friend class actor_instance<actor_manage_role>;
		static actor_manage_role& getInstance() 
		{ 
			return actor_instance<actor_manage_role>::instance(); 
		}

		virtual ~actor_manage_role() {}

		static void nregister();

		static i64_actorid actorid();
				
		bool handle(message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);
	};
}//namespace ngl

