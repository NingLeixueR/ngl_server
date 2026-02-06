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
#include "actor_manage.h"
#include "actor_create.h"
#include "nmysql_pool.h"
#include "ndb_modular.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "tprotocol.h"
#include "db_data.h"
#include "ntimer.h"
#include "notice.h"
#include "nmysql.h"
#include "ncurl.h"
#include "ncsv.h"
#include "ntcp.h"

namespace ngl
{
	struct protocols
	{
		std::map<i32_protocolnum, std::string> m_data;

		DPROTOCOL(protocols, m_data)
	};

	class actor_gmclient : 
		public actor
	{
		actor_gmclient(const actor_gmclient&) = delete;
		actor_gmclient& operator=(const actor_gmclient&) = delete;

		actor_gmclient();
	public:
		friend class actor_instance<actor_gmclient>;
		static actor_gmclient& instance()
		{
			return actor_instance<actor_gmclient>::instance();
		}

		virtual ~actor_gmclient() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i16_area area);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		static void allprotocol(protocols& apro);

		// # GM操作
		using handle_cmd = cmd<actor_gmclient, std::string, int, ncjson&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<mforward<np_gm>>& adata);
	};
}// namespace ngl
