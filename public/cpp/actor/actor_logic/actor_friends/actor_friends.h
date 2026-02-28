/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "actor/actor_logic/actor_events/actor_events.h"
#include "actor/actor_logic/actor_events/events_logic.h"
#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/actor_logic/actor_friends/friends.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/actor_create.h"
#include "actor/actor_base/ndb_modular.h"
#include "actor/actor_base/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "tools/log/nlog.h"
#include "net/tcp/ntcp.h"

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
		static actor_friends& instance()
		{
			return actor_instance<actor_friends>::instance();
		}

		virtual ~actor_friends() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_ADD>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_ERASE>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>>& adata);
		bool handle(const message<np_eevents_logic_rolelogin>& adata);
		bool handle(const message<np_eevents_logic_roleoffline>& adata);
	};
}// namespace ngl
