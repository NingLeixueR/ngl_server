#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_role.h"

namespace ngl
{
	class actor_kcp : public actor
	{
		actor_kcp();
	public:
		friend class actor_instance<actor_kcp>;
		static actor_kcp& getInstance()
		{
			return actor_instance<actor_kcp>::instance();
		}

		virtual ~actor_kcp() {}

		static void actor_register();

		bool handle(message<actor_protocol_kcp>& adata);
		
	private:

	};

}

