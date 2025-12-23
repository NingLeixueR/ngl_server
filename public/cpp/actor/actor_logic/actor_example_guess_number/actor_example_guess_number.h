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
#include "actor_example.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "example.pb.h"
#include "manage_csv.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.pb.h"
#include "ntimer.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	enum eactor_example_guess_number
	{
		guess_number_esetp_maxtime = 30, // 单步骤执行的最大秒数
		guess_number_equit_time = 10,	// 退出的最大秒数
		guess_number_max_number = 101,
		guess_number_min_number = 0,
	};

	class actor_example_guess_number : 
		public actor_example<actor_example_guess_number>
	{
		actor_example_guess_number(const actor_example_guess_number&) = delete;
		actor_example_guess_number& operator=(const actor_example_guess_number&) = delete;

		int32_t m_maxnumber		= 1000;	// 范围最大值
		int32_t m_minnumber		= 0;	// 范围最小值
		int32_t m_index			= 0;	// 猜测的玩家(索引)
		int32_t m_setputc		= 0;	// 步骤时间
		int32_t m_bombvalues	= 0;	// 炸弹值
		int32_t m_bombutc		= 0;	// 触发炸弹值的时间
		i64_actorid m_bombrole	= 0;	// 触发炸弹值的玩家
	public:
		actor_example_guess_number(const std::map<int32_t, i64_actorid>& aroleids, int32_t aindex);

		virtual ~actor_example_guess_number() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		void next_setp();

		void bomb(i64_actorid aroleid = nguid::make());

		bool isfinish();

		void set_finish(i64_actorid abombrole);

		i64_actorid next_guess_role();

		void send_error(i64_actorid aroleid, pbexample::E_GUESS_NUMBER_ERROR aerr);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>>& adata);
		bool handle(const message<np_example_entergame_ready>& adata);
	};
}//namespace ngl