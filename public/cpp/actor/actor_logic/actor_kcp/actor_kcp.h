#pragma once

#include "actor_manage.h"
#include "actor_role.h"
#include "ndbclient.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"
#include "net.h"

namespace ngl
{
	class actor_kcp : public actor
	{
		actor_kcp(const actor_kcp&) = delete;
		actor_kcp& operator=(const actor_kcp&) = delete;

		actor_kcp();
	public:
		friend class actor_instance<actor_kcp>;
		static actor_kcp& getInstance()
		{
			return actor_instance<actor_kcp>::instance();
		}

		virtual ~actor_kcp() = default;

		static void nregister();

		bool handle(message<np_actor_kcp>& adata);
	};
}//namespace ngl