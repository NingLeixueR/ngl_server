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

#include "ttab_specialid.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "example.pb.h"
#include "manage_csv.h"
#include "nsp_write.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.pb.h"
#include "ntimer.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	enum eactor_example_manage
	{
		exampleindex_max = pbexample::EPLAY_TYPE_MAX + 1,
		example_waittime = 30,	// 等待进入游戏的最大秒数
	};

	struct playinfo
	{
		i64_actorid m_actorexampleid = nguid::make();
		pbexample::EPLAY_TYPE m_type = pbexample::EPLAY_NULL;
		int32_t m_createexample = 0;
		std::map<i64_actorid, bool> m_role_enter_example; // 玩家是否进入例子游戏
		std::set<i64_actorid> m_roles;
	};

	class actor_example_manage : 
		public actor
	{
		actor_example_manage(const actor_example_manage&) = delete;
		actor_example_manage& operator=(const actor_example_manage&) = delete;

		using type_tuple = std::tuple<pbexample::EPLAY_TYPE, i64_actorid>;

		std::array<int32_t, exampleindex_max> m_exampleindex;
		std::map<pbexample::EPLAY_TYPE, std::map<i64_actorid, playinfo>> m_info;
		std::map<pbexample::EPLAY_TYPE, std::map<i64_actorid, playinfo>> m_finishinfo;		
		std::map<i64_actorid, type_tuple> m_playerexample; // key:roleid value:actor_exmple_xxx

		actor_example_manage();
	public:
		friend class actor_instance<actor_example_manage>;
		static actor_example_manage& instance()
		{
			return actor_instance<actor_example_manage>::instance();
		}

		virtual ~actor_example_manage() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		void enter_game(playinfo* applayinfo, i64_actorid aroleid, pbexample::ECROSS across, pbexample::EPLAY_TYPE atype);

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>>& adata);
		bool handle(const message<np_create_example>& adata);
		bool handle(const message<np_example_equit>& adata);
		bool handle(const message<np_login_request_info>& adata);
	};
}//namespace ngl
