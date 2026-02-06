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
#include "nmysql_pool.h"
#include "actor_role.h"
#include "ndbclient.h"
#include "db_data.h"
#include "nmysql.h"
#include "ntcp.h"

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

		static i64_actorid actorid(int32_t aserid);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_actor_kcp>& adata);
	};
}//namespace ngl
