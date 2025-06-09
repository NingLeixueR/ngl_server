#pragma once

#include "actor_events.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "events_logic.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "friends.h"
#include "ntimer.h"
#include "drop.h"
#include "nlog.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_friends : public actor
	{
		actor_friends(const actor_friends&) = delete;
		actor_friends& operator=(const actor_friends&) = delete;

		friends m_friends;

		actor_friends();
	public:
		friend class actor_instance<actor_friends>;
		static actor_friends& getInstance()
		{
			return actor_instance<actor_friends>::instance();
		}

		virtual ~actor_friends() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		// [请求]好友信息
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND>>& adata);
		// [请求]添加好友
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_ADD>>& adata);
		// [请求]删除好友
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_ERASE>>& adata);
		// [请求]同意/拒绝添加好友
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>>& adata);
		bool handle(const message<np_eevents_logic_rolelogin>& adata);
		bool handle(const message<np_eevents_logic_roleoffline>& adata);
	};
}// namespace ngl
