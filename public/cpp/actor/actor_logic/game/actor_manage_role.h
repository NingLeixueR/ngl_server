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
		actor_manage_role();
	public:
		friend class actor_instance<actor_manage_role>;
		static actor_manage_role& getInstance() 
		{ 
			return actor_instance<actor_manage_role>::instance(); 
		}

		virtual ~actor_manage_role() {}

		static void nregister();
				
		bool handle(message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);
	};
}

