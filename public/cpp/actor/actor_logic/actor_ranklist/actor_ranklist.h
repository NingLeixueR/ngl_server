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
#include "db_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "ranklist.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "notice.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_ranklist : 
		public actor
	{
		actor_ranklist(const actor_ranklist&) = delete;
		actor_ranklist& operator=(const actor_ranklist&) = delete;

		ranklist m_ranklist;

		actor_ranklist();
	public:
		friend class actor_instance<actor_ranklist>;
		static actor_ranklist& instance()
		{
			return actor_instance<actor_ranklist>::instance();
		}

		virtual ~actor_ranklist() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		virtual void erase_actor_before();

		static void nregister();

		using handle_cmd = cmd<actor_ranklist, std::string, int, ncjson&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<mforward<np_gm>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_RANKLIST>>& adata);
		bool handle(const message<np_activityrank_operator>& adata);
		bool handle(const message<np_get_rank>& adata);
	};
}// namespace ngl
