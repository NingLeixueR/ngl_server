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

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "manage_csv.h"
#include "keyvaluedb.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_keyvalue : 
		public actor
	{
		actor_keyvalue(const actor_keyvalue&) = delete;
		actor_keyvalue& operator=(const actor_keyvalue&) = delete;

		actor_keyvalue();

		keyvaluedb m_keyvaluedb;
	public:
		friend class actor_instance<actor_keyvalue>;
		static actor_keyvalue& instance()
		{
			return actor_instance<actor_keyvalue>::instance();
		}

		virtual ~actor_keyvalue() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		// # GM操作
		using handle_cmd = cmd<actor_keyvalue, std::string, int, ncjson&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);

		bool handle(const message<mforward<np_gm>>& adata);
	};
}//namespace ngl
