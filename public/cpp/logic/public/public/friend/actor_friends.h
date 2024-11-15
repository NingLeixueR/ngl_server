#pragma once

#include "actor_events.h"
#include "actor_manage.h"
#include "actor_create.h"
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

		virtual void init()
		{
			m_friends.set(this);
		}

		static void nregister();

		virtual ~actor_friends() {}

		virtual void loaddb_finish(bool adbishave) 
		{
			actor_events<eevents_logic>::register_actor_event(eevents_logic_rolelogin, actorid(tab_self_area));
		}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_FRIENDS;
		}

		static i64_actorid actorid(i16_area area)
		{
			return nguid::make(ACTOR_FRIENDS, area, nguid::none_actordataid());
		}

		bool handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND>>& adata);

		// # 添加好友
		bool handle(const message<mforward<pbnet::PROBUFF_NET_ADDFRIEND>>& adata);

		// # 同意/拒绝好友申请
		bool handle(const message<mforward<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND>>& adata);

		// # 删除好友
		bool handle(const message<mforward<pbnet::PROBUFF_NET_ERASEFRIEND>>& adata);

		// # 好友上线
		bool handle(const message<np_eevents_logic_rolelogin>& adata);
	};
}// namespace ngl