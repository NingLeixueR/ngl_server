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
#include "nsp_server.h"
#include "nsp_write.h"
#include "nsp_read.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "notice.h"
#include "ntimer.h"
#include "gcmd.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_notice : 
		public actor
	{
		actor_notice(const actor_notice&) = delete;
		actor_notice& operator=(const actor_notice&) = delete;

		notice m_notice;

		actor_notice();
	public:
		friend class actor_instance<actor_notice>;
		static actor_notice& instance()
		{
			return actor_instance<actor_notice>::instance();
		}

		virtual ~actor_notice() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		static bool sendnotice(const std::string& anotice, int32_t astarttime, int32_t afinishtime);

		using handle_cmd = cmd<actor_notice, std::string, int, ngl::njson_read&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<np_gm>>& adata);
		// [请求]获取公告列表
		bool handle(const message<mforward<pbnet::PROBUFF_NET_NOTICE>>& adata);
		bool handle(const message<np_actor_addnotice>& adata);
	};
}//namespace ngl
