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
	class actor_kcp : 
		public actor
	{
		actor_kcp(const actor_kcp&) = delete;
		actor_kcp& operator=(const actor_kcp&) = delete;

		actor_kcp();
	public:
		friend class actor_instance<actor_kcp>;
		static actor_kcp& instance()
		{
			return actor_instance<actor_kcp>::instance();
		}

		virtual ~actor_kcp() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<np_actor_kcp>& adata);
	};
}//namespace ngl
