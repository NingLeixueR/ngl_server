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
	class actor_role_manage : public actor
	{
		actor_role_manage(const actor_role_manage&) = delete;
		actor_role_manage& operator=(const actor_role_manage&) = delete;

		actor_role_manage();

		std::set<int64_t> m_roleban;
	public:
		friend class actor_instance<actor_role_manage>;
		static actor_role_manage& instance()
		{ 
			return actor_instance<actor_role_manage>::instance();
		}

		virtual ~actor_role_manage() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		//# gm«Î«Û
		using handle_gm = cmd<actor_role, std::string, int, const ngl::json_read&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<np_gm>>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);
	};
}//namespace ngl
