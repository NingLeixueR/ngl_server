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

#include "nmysql_manage.h"
#include "events_logic.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "actor_events.h"
#include "nmysql_pool.h"
#include "ndb_modular.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_data.h"
#include "ntimer.h"
#include "family.h"
#include "nmysql.h"
#include "type.h"
#include "drop.h"
#include "nlog.h"
#include "drop.h"
#include "ntcp.h"

namespace ngl
{
	class actor_family : 
		public actor
	{
		actor_family(const actor_family&) = delete;
		actor_family& operator=(const actor_family&) = delete;

		friend class familyer;
		friend class family;
		familyer m_familyer;
		family m_family;
		drop<actor_family> m_drop;

		actor_family();
	public:
		friend class actor_instance<actor_family>;
		static actor_family& instance()
		{
			return actor_instance<actor_family>::instance();
		}

		virtual ~actor_family() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		drop<actor_family>& get_drop()
		{
			return m_drop;
		}
		
		// # GM操作
		using handle_cmd = cmd<actor_mail, std::string, int, ncjson&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<mforward<np_gm>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_CEDE>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_CREATE>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_INFO>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_JOIN>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_LEAVE>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_SIGN>>& adata);
		bool handle(const message<np_eevents_logic_rolelogin>& adata);
		bool handle(const message<np_eevents_logic_roleoffline>& adata);
	};
}// namespace ngl
