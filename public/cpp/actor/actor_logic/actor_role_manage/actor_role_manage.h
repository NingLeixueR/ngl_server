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
	class actor_role_manage : 
		public actor
	{
		actor_role_manage(const actor_role_manage&) = delete;
		actor_role_manage& operator=(const actor_role_manage&) = delete;

		actor_role_manage();

		std::set<i64_actorid> m_roleban;
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

		//# 执行handle之后调用
		virtual void handle_after(handle_pram&);

		//# gm请求
		using handle_gm = cmd<actor_role, std::string, int, ngl::njson_read&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<np_gm>>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);
	};
}//namespace ngl
