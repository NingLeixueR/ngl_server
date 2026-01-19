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
#include "ndbclient.h"
#include "nprotocol.h"
#include "tprotocol.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "notice.h"
#include "ncurl.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_gm : 
		public actor
	{
		actor_gm(const actor_gm&) = delete;
		actor_gm& operator=(const actor_gm&) = delete;

		actor_gm();
	public:
		friend class actor_instance<actor_gm>;
		static actor_gm& instance()
		{
			return actor_instance<actor_gm>::instance();
		}

		virtual ~actor_gm() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i16_area area);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		// # 转发给单例
		void sendbytype(ENUM_ACTOR atype, const pack* apack, const ngl::np_gm& apro);

		// # 转发给特定actor
		void sendbyactorid(i64_actorid aactorid, const pack* apack, const ngl::np_gm& apro);

		static bool checklocalbytype(NODE_TYPE atype);

		void init_handle_cmd();
		
		static bool sendtophp(i32_sessionid aidentifier, const ngl::np_gm_response& adata);

		using handle_cmd = cmd<actor_gm, std::string, ncjson&, const message<ngl::np_gm>*>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<mforward<ngl::np_gm_response>>& adata);
		bool handle(const message<ngl::np_gm>& adata);
	};
}// namespace ngl
